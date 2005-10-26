/*  AudioTrack.cxx
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

#include <iostream>

#include "AudioTrack.H"
#include "AudioFileQT.H"
#include "AudioClip.H"

namespace nle
{

AudioTrack::AudioTrack( int num )
	: TrackBase( num )
{
}
AudioTrack::~AudioTrack()
{
}
void AudioTrack::addFile( int64_t position, const char* filename )
{
	IAudioFile *af = new AudioFileQT( filename );
	if ( !af->ok() ) {
		delete af;
		std::cerr << "Error: AudioTrack::add_audio" << std::endl;
		return;
	}
	Clip *clp = new AudioClip( this, position, af );
	addClip( clp );
}
void AudioTrack::sort()
{
	Track::sort();
	m_current = m_clips;
}
static int audio_track_sound_length_helper( void* p, void* data )
{
	int64_t l;
	int64_t* max = (int64_t*)data;
	clip_node* node = (clip_node*)p;
	l = node->clip->length() + node->clip->position();
	if ( l > *max ) {
		*max = l;
	}
	return 0;
}
int64_t AudioTrack::soundLength()
{
	int64_t max = 0;
	sl_map( m_clips, audio_track_sound_length_helper, &max );
	return max;
}



} /* namespace nle */
