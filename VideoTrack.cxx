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

#include <iostream>

#include "VideoTrack.H"
#include "VideoClip.H"
#include "VideoFileQT.H"

using namespace std;

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
void VideoTrack::addFile( int64_t position, string filename )
{
	VideoFileQT *vf = new VideoFileQT( filename );
	
	if ( !vf->ok() ) {
		cerr << "VideoFile not OK" << endl;
		delete vf;
		return;
	}
	addClip( new VideoClip( this, position, vf ) );
}
frame_struct* VideoTrack::getFrame( int64_t position )
{
	frame_struct* res = NULL;
	for ( clip_node *p = m_clips; p; p = p->next ) {
		VideoClip* current = (VideoClip*)p->clip;
		res = current->getFrame( position );
		if ( res )
			return res;
	}
	return NULL;

}
	
} /* namespace nle */
