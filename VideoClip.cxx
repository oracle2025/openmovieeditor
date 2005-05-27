/*  VideoClip.cxx
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

#include <string>
#include <iostream>

#include "VideoClip.H"
#include "IVideoFile.H"
#include "FilmStrip.H"

using namespace std;

namespace nle
{
	
VideoClip::VideoClip( Track *track, int64_t position, IVideoFile *vf )
	: Clip( track, position )
{
	m_videoFile = vf;
	m_filmStrip = new FilmStrip( vf );
	m_lastFramePosition = -1;
}
VideoClip::~VideoClip(){}
int64_t VideoClip::length()
{
	return m_videoFile->length() - ( m_trimA + m_trimB );
}
void VideoClip::reset()
{
	m_videoFile->seek( m_trimA );//FIXME noch nötig??
	/*evtl. damit nicht bei jedem Abspiel vorgang für jeden Clip geseekt werden muss*/
}
frame_struct* VideoClip::getFrame( int64_t position )
{
//	m_lastFramePosition = position;
	if ( position < m_position || position > m_position + length() )
		return NULL;
	if ( m_lastFramePosition + 1 != position ) {
		int64_t s_pos = position - m_position + m_trimA;
		m_videoFile->seek( s_pos );
	} 
	m_lastFramePosition = position;
	return m_videoFile->read();
}
/*frame_struct* VideoClip::get_preview( int index )
{
	pic_struct *p = m_filmStrip->get_pic( index ); //data w h
	m_previewFrame.x = 0;
	m_previewFrame.y = 0;
	m_previewFrame.w = p->w;
	m_previewFrame.h = p->h;
	m_previewFrame.RGB = p->data;
	return &m_previewFrame;
}*/
	
} /* namespace nle */
