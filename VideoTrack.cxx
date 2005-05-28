/*  VideoTrack.cxx
 *
 *  Copyright (C) 2003 Richard Spindler <richard.spindler AT gmail.com>
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

#include "VideoTrack.H"
#include "VideoClip.H"
#include "VideoFileQT.H"

namespace nle
{

VideoTrack::VideoTrack( int num )
	: Track( num )
{
}
VideoTrack::~VideoTrack()
{
}
void VideoTrack::reset()
{
	Track::reset();
	m_playPosition = 0;
}
void VideoTrack::add( const char* filename, int64_t position ) //FIXME: use IVideoFile here??
{
	VideoFileQT *vf = new VideoFileQT( filename );
	
	if ( !vf->ok() ) {
		delete vf;
		return;
	}
	m_clips.push_back( new VideoClip( this, position, vf ) );
}
frame_struct* VideoTrack::getFrame( int64_t position )
{
	frame_struct* res = NULL;
	for ( std::list< Clip* >::iterator j = m_clips.begin(); j != m_clips.end(); j++ ) {
		if ( res = ((VideoClip*)(*j))->getFrame( position ) )
			return res;
	}
	return NULL;

}
/*frame_struct* VideoTrack::nextFrame()
{
	frame_struct* res = NULL;
	m_playPosition++;
	for ( std::list< Clip* >::iterator j = m_clips.begin(); j != m_clips.end(); j++ ) {
		res = ((VideoClip*)(*j))->getFrame( m_playPosition - 1 );
		if ( res ) {
			return res;
		}
	}
	return NULL;
}*/
	
} /* namespace nle */
