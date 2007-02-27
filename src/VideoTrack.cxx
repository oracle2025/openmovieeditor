/*  VideoTrack.cxx
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

#include <cassert>

#include <FL/filename.H>

#include "VideoTrack.H"
#include "VideoClip.H"
#include "ImageClip.H"
#include "ErrorDialog/IErrorHandler.H"
#include "render_helper.H"
#include "VideoFileFactory.H"
#include "globals.H"
#include "DummyClip.H"
#include "TitleClip.H"

namespace nle
{

VideoTrack::VideoTrack( int num, string name )
	: TrackBase( num, name )
{
	m_vidCurrent = 0;
	m_fade_overs = 0;
	m_currentAudioFadeOver = 0;
	m_preparedFrame1.RGB = 0;
	m_preparedFrame2.RGB = 0;
}
VideoTrack::~VideoTrack()
{
	fade_over* node;
	while ( ( node = (fade_over*)sl_pop( &m_fade_overs ) ) ) {
		delete node;
	}
}
void VideoTrack::sort()
{
	Track::sort();
	m_current = m_clips;
	m_vidCurrent = m_clips;
	m_playPosition = 0;
	m_currentAudioFadeOver = m_fade_overs;
}
void VideoTrack::addFile( int64_t position, string filename, int64_t trimA, int64_t trimB, int mute, int id, int64_t length, ClipData* data )
{
	if ( filename == "TitleClip" ) {
		TitleClip* c = new TitleClip( this, position, length - trimA - trimB, id, data );
		addClip( c );
		return;
	}
	IVideoFile* vf = VideoFileFactory::get( filename );
	
	if ( vf ) {
		VideoClip* c = new VideoClip( this, position, vf, trimA, trimB, id, data );
		c->m_mute = mute;
		addClip( c );
		return;
	} else {
		ImageClip* ic = new ImageClip( this, position, filename, length - trimA - trimB, id, data );
		if ( !ic->ok() ) {
			delete ic;
			if ( length > 0 ) {
				Clip* c = new DummyClip( this, filename, position, length+trimA+trimB, trimA, trimB );
				addClip( c );
				return;
			}
			SHOW_ERROR( string( "Video file failed to load:\n" ) + fl_filename_name( filename.c_str() ) );
			return;
		}
		addClip( ic );
	}
}
frame_struct* VideoTrack::getFrame( int64_t position )
{
	frame_struct* res = NULL;
	if ( g_SEEKING ) {
		for ( clip_node *p = m_clips; p; p = p->next ) {
			IVideoReader* current;
			if ( p->clip->type() == CLIP_TYPE_VIDEO ) {
				current = dynamic_cast<VideoClip*>(p->clip);
			} else if ( p->clip->type() == CLIP_TYPE_IMAGE ) {
				current = dynamic_cast<ImageClip*>(p->clip);
			} else {
				continue;
			}
			res = current->getFrame( position );
			if ( res )
				return res;
		}
	} else {
		while ( m_vidCurrent && m_vidCurrent->clip->B() <= position  ) {
			m_vidCurrent = m_vidCurrent->next;
		}
		if ( m_vidCurrent ) {
			IVideoReader* current = 0;
			if ( m_vidCurrent->clip->type() == CLIP_TYPE_VIDEO ) {
				current = dynamic_cast<VideoClip*>(m_vidCurrent->clip);
			} else if ( m_vidCurrent->clip->type() == CLIP_TYPE_IMAGE ) {
				current = dynamic_cast<ImageClip*>(m_vidCurrent->clip);
			}
			if ( current ) {
				res = current->getFrame( position );
				return res;
			}
		}
	}
	return NULL;

}
void VideoTrack::removeClip( Clip* clip )
{
	Track::removeClip( clip );
	reconsiderFadeOver();
}
static int find_fade_over_helper( void* p, void* data )
{
	fade_over* node = (fade_over*)p;
	int64_t position = *((int64_t*)data);
	return ( fade_over_start( node ) <= position && fade_over_end( node ) > position );
}
frame_struct** VideoTrack::getFrameStack( int64_t position )
{
	static frame_struct* frameStack[3];
	frameStack[0] = 0;
	frameStack[1] = 0;
	frameStack[2] = 0;

	fade_over* o = (fade_over*)sl_map( m_fade_overs, find_fade_over_helper, &position );
	if ( o ) {
		IVideoReader* A;
		IVideoReader* B;
		A = dynamic_cast<IVideoReader*>(o->clipA);
		B = dynamic_cast<IVideoReader*>(o->clipB);
		assert( A );
		assert( B );
		frameStack[0] = A->getFrame( position );
		frameStack[1] = B->getFrame( position );
		if ( frameStack[0] && frameStack[1] ) {
			get_alpha_values( o, frameStack[0]->alpha, frameStack[1]->alpha, position );
			if ( !frameStack[0]->has_alpha_channel ) {
				frameStack[1]->alpha = 1.0;
			}
		}
		return frameStack;
	} else {
		for ( clip_node *p = m_clips; p; p = p->next ) {
			IVideoReader* current;
			current = dynamic_cast<IVideoReader*>(p->clip);
			if ( !current ) {
				continue;
			}
			frameStack[0] = current->getFrame( position );
			if ( frameStack[0] ) {
				frameStack[0]->alpha = 1.0;
				return frameStack;
			}
		}
	}
	return frameStack;
}
void VideoTrack::prepareFormat( int w, int h, int aspect_w, int aspect_h, float aspect, int analog_blank )
{
	m_preparedFrame1.RGB = new unsigned char[w * h * 4];
	m_preparedFrame2.RGB = new unsigned char[w * h * 4];
	//TODO: anders initialisieren?
	m_preparedFrame1.x = 0;
	m_preparedFrame1.y = 0;
	m_preparedFrame1.w = w;
	m_preparedFrame1.h = h;
	m_preparedFrame1.alpha = 1.0;
	m_preparedFrame1.has_alpha_channel = true;
	m_preparedFrame2.x = 0;
	m_preparedFrame2.y = 0;
	m_preparedFrame2.w = w;
	m_preparedFrame2.h = h;
	m_preparedFrame2.alpha = 1.0;
	m_preparedFrame2.has_alpha_channel = true;

	for ( clip_node* j = getClips(); j; j = j->next ) {
		VideoEffectClip* vec = dynamic_cast<VideoEffectClip*>(j->clip);
		if ( vec ) {
			vec->prepareFormat( w, h, aspect_w, aspect_h, aspect, analog_blank );
		}
	}

}
void VideoTrack::unPrepareFormat()
{
	if ( m_preparedFrame1.RGB ) {
		delete [] m_preparedFrame1.RGB;
		m_preparedFrame1.RGB = 0;
	}
	if ( m_preparedFrame2.RGB ) {
		delete [] m_preparedFrame2.RGB;
		m_preparedFrame2.RGB = 0;
	}
}
frame_struct** VideoTrack::getFormattedFrameStack( int64_t position )
{
	static frame_struct* frameStack[3];
	frameStack[0] = 0;
	frameStack[1] = 0;
	frameStack[2] = 0;

	fade_over* o = (fade_over*)sl_map( m_fade_overs, find_fade_over_helper, &position );
	if ( o ) {
		VideoEffectClip* A;
		VideoEffectClip* B;
		A = dynamic_cast<VideoEffectClip*>(o->clipA);
		B = dynamic_cast<VideoEffectClip*>(o->clipB);
		assert( A );
		assert( B );
		frameStack[0] = A->getFormattedFrame( &m_preparedFrame1, position );
		frameStack[1] = B->getFormattedFrame( &m_preparedFrame2, position );
		if ( frameStack[0] && frameStack[1] ) {
			get_alpha_values( o, frameStack[0]->alpha, frameStack[1]->alpha, position );
			if ( !frameStack[0]->has_alpha_channel ) {
				frameStack[1]->alpha = 1.0;
			}
		}
		return frameStack;
	} else {
		for ( clip_node *p = m_clips; p; p = p->next ) {
			VideoEffectClip* current;
			current = dynamic_cast<VideoEffectClip*>(p->clip);
			if ( !current ) {
				continue;
			}
			frameStack[0] = current->getFormattedFrame( &m_preparedFrame1, position );
			if ( frameStack[0] ) {
				frameStack[0]->alpha = 1.0;
				return frameStack;
			}
		}
	}
	return frameStack;

}

int fo_sort_helper( void* p, void* q )
{
	fade_over* pn = (fade_over*)p;
	fade_over* qn = (fade_over*)q;
	return fade_over_start( pn ) > fade_over_start( qn );

}
void VideoTrack::reconsiderFadeOver()
{
	fade_over* node;
	while ( ( node = (fade_over*)sl_pop( &m_fade_overs ) ) ) {
		delete node;
	}
	for ( clip_node* j = getClips(); j; j = j->next ) {
		Clip* clip = j->clip;
		for ( clip_node* k = j->next; k; k = k->next ) {
			Clip* cclip = k->clip;
			if ( clip->A() < cclip->A() && clip->B() > cclip->A() ) {
				m_fade_overs = (fade_over*)sl_push( m_fade_overs, create_fade_over( clip, cclip ) );
			} else if ( cclip->A() < clip->A() && cclip->B() > clip->A() ) {
				m_fade_overs = (fade_over*)sl_push( m_fade_overs, create_fade_over( cclip, clip ) );
			}
		}
	}

	m_fade_overs = (fade_over*)sl_mergesort( m_fade_overs, fo_sort_helper );
}
int64_t fade_over_start( fade_over* o )
{
	return o->clipB->A();
}
int64_t fade_over_end( fade_over* o )
{
	return o->clipA->B()+1;
}
fade_over* create_fade_over( Clip* a, Clip* b )
{
	fade_over* o = new fade_over;
	o->next = 0;
	o->clipA = a;
	o->clipB = b;
	o->inc = 1.0 / (float)( fade_over_end( o ) - fade_over_start( o ) );
	return o;
}
void get_alpha_values( fade_over* o, float& a, float& b, int64_t position )
{
	int64_t p = position - fade_over_start( o );
	a = 1.0 - ( o->inc * p );
	b = 1.0;// - a; //Fadeover mismatch
}
int VideoTrack::fillBuffer( float* output, unsigned long frames, int64_t position )
{
	unsigned long inc;
	unsigned long written = 0;
	unsigned long emptyItems = 0;
	float* incBuffer = output;
	float buf[23040*2];
	assert( frames <= 23040);
	while ( m_current && m_current->clip->type() != CLIP_TYPE_VIDEO && m_current->clip->type() != CLIP_TYPE_AUDIO ) {
		m_current = m_current->next;
	}
	//while ( m_currentAudioFadeOver && fade_over_end( m_currentAudioFadeOver ) * 1920 < position + frames  ) { // 48000 / 25 == 1920
	while ( m_currentAudioFadeOver && fade_over_end( m_currentAudioFadeOver ) * 1.6 < position + frames  ) { // 48000 / 30000 == 1.6
		m_currentAudioFadeOver = m_currentAudioFadeOver->next;
	}
	if ( m_currentAudioFadeOver && position > fade_over_start( m_currentAudioFadeOver ) * 1.6 ) {
		AudioClip* ac1 = dynamic_cast<AudioClip*>(m_currentAudioFadeOver->clipA);
		AudioClip* ac2 = dynamic_cast<AudioClip*>(m_currentAudioFadeOver->clipB);
		for ( int i = (frames * 2) - 1; i >= 0; i-- ) {
			incBuffer[i] = 0.0;
			buf[i] = 0.0;
		}
		if ( ac1 && ac2 ) {
			ac1->fillBuffer( incBuffer, frames, position );
			ac2->fillBuffer( buf, frames, position );
			mixChannels( incBuffer, buf, incBuffer, frames );
		} else if ( ac1 ) {
			ac1->fillBuffer( incBuffer, frames, position );
		} else if ( ac2 ) {
			ac2->fillBuffer( incBuffer, frames, position );
		}
		written = frames;
	} else {
		while( written < frames && m_current ) {
			inc = (dynamic_cast<AudioClip*>(m_current->clip))->fillBuffer( incBuffer,
					 frames - written, position + written
					);
			written += inc;
			incBuffer += inc;
			if ( written < frames ) { m_current = m_current->next; }
			while ( m_current && m_current->clip->type() != CLIP_TYPE_VIDEO && m_current->clip->type() != CLIP_TYPE_AUDIO ) {
				m_current = m_current->next;
			}
		}
		if ( m_current == 0 ) {
			while( written < frames ) {
				*incBuffer = 0.0;
				incBuffer++;
				*incBuffer = 0.0;
				written++;
				incBuffer++;
				emptyItems++;
			}
		}
	}
	return written - emptyItems;
}
} /* namespace nle */
