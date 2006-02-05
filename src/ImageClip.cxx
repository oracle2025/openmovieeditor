/*  ImageClip.cxx
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

#include <FL/Fl_PNG_Image.H>

#include "ImageClip.H"

namespace nle
{


ImageClip::ImageClip( Track* track, int64_t position, string filename )
	: Clip( track, position ), m_filename( filename )
{
	m_ok = false;
	m_image = new Fl_PNG_Image( filename.c_str() );
	cout << "DEPTH: " << m_image->d() << endl;
	m_length = 25 * 10;
	m_frame.x = m_frame.y = 0;
	m_frame.w = m_image->w();
	m_frame.h = m_image->h();
	m_frame.alpha = 1.0;
	m_frame.has_alpha_channel = true;
	//m_frame.RGB = new (unsigned char)[m_frame.w * m_frame.h * 4];
	char** d = (char**)m_image->data();
	int c = m_image->count();
	m_frame.RGB = (unsigned char *)d[0];
	m_ok = true;
}

ImageClip::~ImageClip()
{
	if ( m_image ) {
		delete m_image;
		m_image = 0;
	}
}
int64_t ImageClip::length()
{
	return m_length;
}

frame_struct* ImageClip::getFrame( int64_t position )
{
	m_frame.alpha = 1.0;
	if ( position > m_position && position < m_position + m_length ) {
		return &m_frame;
	} else {
		return 0;
	}
}


} /* namespace nle */
