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
}
VideoTrack::~VideoTrack()
{
}
void VideoTrack::sort()
{
	Track::sort();
	m_current = m_clips;
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
	for ( clip_node *p = m_clips; p; p = p->next ) {
		IVideoReader* current;
		if ( p ->clip->type() == CLIP_TYPE_VIDEO ) {
			current = (VideoClip*)p->clip;
		} else if ( p->clip->type() == CLIP_TYPE_IMAGE ) {
			current = (ImageClip*)p->clip;
		} else {
			continue;
		}
		res = current->getFrame( position );
		if ( res )
			return res;
	}
	return NULL;

}
	
} /* namespace nle */
