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

//#include <FL/Fl_PNG_Image.H>
#include <FL/Fl_Shared_Image.H>

#include "ImageClip.H"
#include "ErrorDialog/IErrorHandler.H"
#include "ImageClipArtist.H"

namespace nle
{


ImageClip::ImageClip( Track* track, int64_t position, string filename )
	: Clip( track, position ), m_filename( filename )
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

	
//	m_image = new Fl_PNG_Image( filename.c_str() );
	cout << "DEPTH: " << m_image->d() << endl;
	m_length = 25 * 10;
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
	m_artist = new ImageClipArtist( this, m_image );
	m_ok = true;
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

frame_struct* ImageClip::getFrame( int64_t position )
{
	m_frame.alpha = 1.0;
	if ( position > m_position && position < m_position + m_length ) {
		return &m_frame;
	} else {
		return 0;
	}
}

void ImageClip::trimA( int64_t trim )
{
	if ( m_length - trim < 0 ) {
		return;
	}
	if ( m_position + trim < 0 ) {
		trim = (-1) * m_position;
	}
	m_length -= trim;
	m_position += trim;

}
void ImageClip::trimB( int64_t trim )
{
	if ( m_length - trim < 0 ) {
		return;
	}
	m_length -= trim;
}
int64_t ImageClip::fileLength()
{
	return -1;
}

} /* namespace nle */
