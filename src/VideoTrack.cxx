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

#include <FL/filename.H>

#include "VideoTrack.H"
#include "VideoClip.H"
#include "ImageClip.H"
#include "VideoFileQT.H"
#include "ErrorDialog/IErrorHandler.H"

namespace nle
{

VideoTrack::VideoTrack( int num )
	: TrackBase( num )
{
	m_vidCurrent = 0;
	m_fade_overs = 0;
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
}
void VideoTrack::addFile( int64_t position, string filename, int64_t trimA, int64_t trimB )
{
	VideoFileQT *vf = new VideoFileQT( filename );
	
	if ( vf->ok() ) {
		Clip* c = new VideoClip( this, position, vf );
		c->trimA( trimA );
		c->trimB( trimB );
		addClip( c );
		return;
	} else {
		delete vf;
		ImageClip* ic = new ImageClip( this, position, filename );
		if ( !ic->ok() ) {
			delete ic;
			SHOW_ERROR( string( "Video file failed to load:\n" ) + fl_filename_name( filename.c_str() ) );
			return;
		}
		ic->trimA( trimA );
		ic->trimB( trimB );
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

static int find_fade_over_helper( void* p, void* data )
{
	fade_over* node = (fade_over*)p;
	int64_t position = *((int64_t*)data);
	return ( fade_over_start( node ) <= position && fade_over_end( node ) >= position );
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
		A = dynamic_cast<VideoClip*>(o->clipA);
		if ( !A ) {
			A = dynamic_cast<ImageClip*>(o->clipA);
		}
		B = dynamic_cast<VideoClip*>(o->clipB);
		if ( !B ) {
			B = dynamic_cast<ImageClip*>(o->clipB);
		}
		frameStack[0] = A->getFrame( position );
		frameStack[1] = B->getFrame( position );
		if ( frameStack[0] && frameStack[1] ) {
			get_alpha_values( o, frameStack[0]->alpha, frameStack[1]->alpha, position );
		}
		return frameStack;
	} else {
		for ( clip_node *p = m_clips; p; p = p->next ) {
			IVideoReader* current;
			if ( p->clip->type() == CLIP_TYPE_VIDEO ) {
				current = dynamic_cast<VideoClip*>(p->clip);
			} else if ( p->clip->type() == CLIP_TYPE_IMAGE ) {
				current = dynamic_cast<ImageClip*>(p->clip);
			} else {
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
				cout << "ADDING FADE OVER" << endl;
				m_fade_overs = (fade_over*)sl_push( m_fade_overs, create_fade_over( clip, cclip ) );
			} else if ( cclip->A() < clip->A() && cclip->B() > clip->A() ) {
				cout << "ADDING FADE OVER" << endl;
				m_fade_overs = (fade_over*)sl_push( m_fade_overs, create_fade_over( cclip, clip ) );
			}
		}
	}
}
int64_t fade_over_start( fade_over* o )
{
	return o->clipB->A();
}
int64_t fade_over_end( fade_over* o )
{
	return o->clipA->B();
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
	b = 1.0;
	a = 1.0 - ( o->inc * p );
}

} /* namespace nle */
