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

#include <FL/Fl_Shared_Image.H>

#include "ImageClip.H"
#include "ErrorDialog/IErrorHandler.H"
#include "ImageClipArtist.H"
#include "helper.H"
#include "globals.H"

namespace nle
{


ImageClip::ImageClip( Track* track, int64_t position, string filename, int64_t length, int id, ClipData* data )
	: Clip( track, position, id ), m_filename( filename )
{
	m_ok = false;
	m_artist = 0;
	m_image = Fl_Shared_Image::get( filename.c_str() );
	if ( !m_image ) {
		ERROR_DETAIL( "This is not an image file" );
		return;
	}
	if ( m_image->w() > 1024 || m_image->h() > 1024 ) {
		ERROR_DETAIL( "This image is to big, maximum is 1024x1024" );
		return;
	}


	if ( length > 0 ) {
		m_length = length;
	} else {
		m_length = NLE_TIME_BASE * 10;
	}
	m_frame.x = m_frame.y = 0;
	m_frame.w = m_image->w();
	m_frame.h = m_image->h();
	m_frame.alpha = 1.0;
	m_frame.cacheable = true;
	if ( m_image->d() == 4 ) {
		m_frame.has_alpha_channel = true;
	} else if ( m_image->d() == 3 ) {
		m_frame.has_alpha_channel = false;
	} else {
		ERROR_DETAIL( "This image file has a wrong color depth,\nonly RGB and RGBA images are supported" );
		return;
	}
	//m_frame.RGB = new (unsigned char)[m_frame.w * m_frame.h * 4];
	char** d = (char**)m_image->data();
	m_frame.RGB = (unsigned char *)d[0];
	m_artist = new ImageClipArtist( m_image );
	
	unsigned long gcd_wh = gcd( m_frame.w, m_frame.h );
	m_aspectHeight = m_frame.h / gcd_wh; 
	m_aspectWidth = m_frame.w /gcd_wh;
	m_aspectRatio = (float)m_aspectWidth / (float)m_aspectHeight;
	m_ok = true;
	setEffects( data );
}

ImageClip::~ImageClip()
{
	if ( m_image ) {
		m_image->release();
		m_image = 0;
	}
	if ( m_artist ) {
		delete m_artist;
	}
}
int64_t ImageClip::length()
{
	return m_length;
}

frame_struct* ImageClip::getRawFrame( int64_t position, int64_t &position_in_file )
{
	m_frame.alpha = 1.0;
	position_in_file = position - m_position;
	if ( position >= m_position && position <= m_position + m_length ) {
		return &m_frame;
	} else {
		return 0;
	}
}
frame_struct* ImageClip::getFirstFrame()
{
	return &m_frame;
}

int ImageClip::w()
{
	return m_image->w();
}
int ImageClip::h()
{
	return m_image->h();
}
int64_t ImageClip::trimA( int64_t trim )
{
	if ( m_length - trim < 0 ) {
		return 0;
	}
	if ( m_position + trim < 0 ) {
		trim = (-1) * m_position;
	}
	m_length -= trim;
	m_position += trim;
	return trim;
}
int64_t ImageClip::trimB( int64_t trim )
{
	if ( m_length - trim < 0 ) {
		return 0;
	}
	m_length -= trim;
	return trim;
}
int64_t ImageClip::fileLength()
{
	return -1;
}

} /* namespace nle */
