/*  Timeline.cxx
 *
 *  Copyright (C) 2005 Richard Spindler <richard.spindler AT gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <cmath>
#include <cassert>

#include "Timeline.H"
#include "VideoTrack.H"
#include "AudioTrack.H"
#include "VideoClip.H"
#include "timeline/Track.H"
#include "SwitchBoard.H"
#include "render_helper.H"

namespace nle
{

Timeline* g_timeline = 0;

int g_trackId = 0;
int getTrackId() { return g_trackId++; }

Timeline::Timeline()
	: TimelineBase()
{
	VideoTrack *vt;
	AudioTrack *at;
	vt = new VideoTrack( getTrackId() );
	addTrack( vt );
	vt = new VideoTrack( getTrackId() );
	addTrack( vt );
	at = new AudioTrack( getTrackId() );
	addTrack( at );
	at = new AudioTrack( getTrackId() );
	addTrack( at );
	m_playPosition = 0;
	m_samplePosition = 0;

	g_timeline = this; //Singleton sucks, this is better :)

	m_seekPosition = 0;
	m_soundLength = 0;
	m_changed = false;
}
Timeline::~Timeline()
{
	g_timeline = NULL;
}
void reset_helper( Track* track ) { track->sort(); }
static int video_length_helper( void* p, void* data )
{
	int64_t l;
	int64_t* max = (int64_t*)data;
	track_node* node = (track_node*)p;
	if ( node->track->type() == TRACK_TYPE_VIDEO ) {
		l = node->track->length();
		if ( l > *max ) {
			*max = l;
		}
	}
	return 0;
}
static int audio_length_helper( void* p, void* data )
{
	int64_t l;
	int64_t* max = (int64_t*)data;
	track_node* node = (track_node*)p;
	if ( AudioTrack* t = dynamic_cast<AudioTrack*>(node->track) ) {
		l = t->soundLength();
		if ( l > *max ) {
			*max = l;
		}
	}
	return 0;
}

void Timeline::sort()
{
	TimelineBase::sort();
	m_playPosition = m_seekPosition;
	m_samplePosition = int64_t( m_seekPosition * ( 48000 / g_fps ) );
	{
		int64_t audio_max = 0;
		int64_t video_max = 0;
		sl_map( m_allTracks, audio_length_helper, &audio_max );
		sl_map( m_allTracks, video_length_helper, &video_max );
		video_max = (int64_t)( video_max * ( 48000 / g_fps ) );
		m_soundLength = video_max > audio_max ? video_max : audio_max;
	}
}
frame_struct* Timeline::getFrame( int64_t position )
{
	return nextFrame( position );
}
frame_struct* Timeline::nextFrame( int64_t position )
{
	static int64_t last_frame = -1;
	frame_struct* res = NULL;
	if ( position < 0 || last_frame < 0 || last_frame + 1 == position ) {
		m_playPosition++;
	} else {
		m_playPosition = position;
	}
	last_frame = m_playPosition;
	for ( track_node *p = m_allTracks; p; p = p->next ) {
		VideoTrack* current = dynamic_cast<VideoTrack*>(p->track);
		if ( !current ) {
			continue;
		}
		res = current->getFrame( m_playPosition - 1 );
		if ( res )
			return res;
	}
	return res;
}
frame_struct** Timeline::getFormattedFrameStack( int64_t position )
{
	static frame_struct* frameStack[8]; //At most 8 Frames, ought to be enough for everyone ;)
	int cnt = 0;

	if ( position < 0 ) {
		position = m_seekPosition;
		m_seekPosition++;
	}
	
	m_playPosition = position;
	
	for ( track_node *p = m_allTracks; p; p = p->next ) {
		VideoTrack* current = dynamic_cast<VideoTrack*>(p->track);
		if ( !current ) {
			continue;
		}
		frame_struct** fs = current->getFormattedFrameStack( position );
		
		for ( int i = 0; fs[i] && cnt <=7 ; i++ ) {
			frameStack[cnt] = fs[i];
			cnt++;
		}
		if ( cnt == 7 ) {
			break;
		}
	}
	frameStack[cnt] = 0;
	return frameStack;
}
frame_struct** Timeline::getFrameStack( int64_t position )
{
	static frame_struct* frameStack[8]; //At most 8 Frames, ought to be enough for everyone ;)
	int cnt = 0;

	if ( position < 0 ) {
		position = m_seekPosition;
		m_seekPosition++;
	}
	
	m_playPosition = position;
	
	for ( track_node *p = m_allTracks; p; p = p->next ) {
		VideoTrack* current = dynamic_cast<VideoTrack*>(p->track);
		if ( !current ) {
			continue;
		}
		frame_struct** fs = current->getFrameStack( position );
		
		for ( int i = 0; fs[i] && cnt <=7 ; i++ ) {
			frameStack[cnt] = fs[i];
			cnt++;
		}
		if ( cnt == 7 ) {
			break;
		}
	}
	frameStack[cnt] = 0;
	return frameStack;
}
int Timeline::fillBuffer( float* output, unsigned long frames )
{
	static float buffer1[23040*2] = {0};
	static float buffer2[23040*2] = {0};
	unsigned int rv;
	unsigned int max_frames = 0;
	track_node* p = m_allTracks;
	assert( frames <= 23040);
//	p = p->next->next->next;
	if ( !p )
		return 0;
	rv = (dynamic_cast<TrackBase*>(p->track))->fillBuffer( buffer1, frames, m_samplePosition );
	max_frames = rv;
	p = p->next;
	if ( !p ) { //Only one Track
		for ( unsigned long i = 0; i < frames * 2; i += 2 ) {
			output[i] = buffer1[i];
			output[i+1] = buffer1[i+1];
		}
		while ( max_frames < frames && m_samplePosition + max_frames < m_soundLength ) {
			output[max_frames] = 0.0;
			max_frames++;
		}
// 		this is done via sampleseek() from the audio thread
//		m_samplePosition += max_frames;
		return max_frames;
	}
	assert( p );
	assert( p->track );
	TrackBase* tb = dynamic_cast<TrackBase*>(p->track);
	rv = tb->fillBuffer( buffer2, frames, m_samplePosition );
	max_frames = rv > max_frames ? rv : max_frames;
	mixChannels( buffer1, buffer2, output, frames );
	p = p->next;
	while ( p ) {
		rv = (dynamic_cast<TrackBase*>(p->track))->fillBuffer( buffer1, frames, m_samplePosition );
		max_frames = rv > max_frames ? rv : max_frames;
		mixChannels( output, buffer1, output, frames );
		p = p->next;
	}
	while ( max_frames < frames && m_samplePosition + max_frames < m_soundLength ) {
		output[max_frames] = 0.0;
		max_frames++;
	}
//	this is done via sampleseek() from the audio thread
//	m_samplePosition += max_frames;

	return max_frames;
}
void Timeline::getBlendedFrame( frame_struct* dst )
{
	getBlendedFrame( -1, dst );
}
void Timeline::getBlendedFrame( int64_t position, frame_struct* dst )
{
	frame_struct** fs = getFormattedFrameStack( position );
	frame_struct tmp_frame;
	tmp_frame.x = tmp_frame.y = 0;
	tmp_frame.w = dst->w;
	tmp_frame.h = dst->h;
	tmp_frame.RGB = new unsigned char[dst->w * dst->h * 4];
	memset( tmp_frame.RGB, 0, dst->w * dst->h * 4 );
	memset( dst->RGB, 0, dst->w * dst->h * 3 );
	

	int start = 0;
	int stop = 0;
	
	for ( int i = 0; fs[i]; i++ ) {
		if ( fs[i]->has_alpha_channel || fs[i]->alpha < 1.0 ) {
			start = i + 1;
		} else {
			break;
		}
	}
	if ( !fs[start] ) {
		start--;
		if ( start >= 0 ) {
			int len = fs[start]->w * fs[start]->h * 4;
			unsigned char *src, *dest, *end;
			src = fs[start]->RGB;
			dest = dst->RGB;
			end = fs[start]->RGB + len;
			while ( src < end ) {
				dest[0] = src[0];
				dest[1] = src[1];
				dest[2] = src[2];
				dest += 3;
				src += 4;
			}
		}
	} else {
		memcpy( dst->RGB, fs[start]->RGB, dst->w * dst->h * 3 );
		if ( fs[start]->has_alpha_channel || fs[start]->alpha < 1.0 ) {
			cout << "NO ALPHA CHANNEL FOR start" << endl;
		}
	}
	start--;
	for ( int i = start; i >= stop; i-- ) {
		if ( fs[i]->has_alpha_channel ) {
			blend_alpha( dst->RGB, dst->RGB, fs[i]->RGB, fs[i]->alpha, dst->w * dst->h );
			continue;
		}
		blend( dst->RGB, fs[i]->RGB, dst->RGB, fs[i]->alpha, dst->w * dst->h );
	}
	
	// 0: ganz oben
	// 1: 
	// 2: ganz unten, zuerst blitten

	delete tmp_frame.RGB;
}
void Timeline::prepareFormat( int w, int h, int aspect_w, int aspect_h, float aspect, int analog_blank )
{
	for ( track_node *p = m_allTracks; p; p = p->next ) {
		VideoTrack* current = dynamic_cast<VideoTrack*>(p->track);
		if ( !current ) {
			continue;
		}
		current->prepareFormat( w, h, aspect_w, aspect_h, aspect, analog_blank );
	}

}
void Timeline::unPrepareFormat()
{
	for ( track_node *p = m_allTracks; p; p = p->next ) {
		VideoTrack* current = dynamic_cast<VideoTrack*>(p->track);
		if ( !current ) {
			continue;
		}
		current->unPrepareFormat();
	}
}




} /* namespace nle */
