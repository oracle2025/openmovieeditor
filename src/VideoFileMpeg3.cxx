/*  VideoFileMpeg3.cxx
 *
 *  Copyright (C) 2007 Richard Spindler <richard.spindler AT gmail.com>
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

#ifdef LIBMPEG3

#include "VideoFileMpeg3.H"
#include "ErrorDialog/IErrorHandler.H"
#include "helper.H"

#include <math.h>

namespace nle
{

VideoFileMpeg3::VideoFileMpeg3( string filename )
	: IVideoFile()
{
	m_ok = false;
	m_frame = NULL;
	m_rows = NULL;
	m_file = NULL;
	if ( !mpeg3_check_sig( (char*)filename.c_str() ) ) {
		ERROR_DETAIL( "This is not an LibMPEG3 readable Video or TOC File" );
		return;
	}
	m_file = mpeg3_open( (char*)filename.c_str() );
	if ( !m_file ) {
		ERROR_DETAIL( "Could not open MPEG file" );
		return;
	}
	if ( !mpeg3_has_video( m_file ) || mpeg3_total_vstreams( m_file ) <= 0 ) {
		ERROR_DETAIL( "This MPEG file does not have a video track" );
		return;
	}
	m_width = mpeg3_video_width( m_file, 0 );
	m_height = mpeg3_video_height( m_file, 0 );
	float frame_rate = mpeg3_frame_rate( m_file, 0 );

	m_ticksPerFrame = llrint( NLE_TIME_BASE / frame_rate );


	m_frame = new unsigned char[m_width * m_height * 3];
	m_rows = new unsigned char*[m_height];
	for (int i = 0; i < m_height; i++) {
                m_rows[i] = m_frame + m_width * 3 * i;
	}
	m_framestruct.x = 0;
	m_framestruct.y = 0;
	m_framestruct.w = m_width;
	m_framestruct.h = m_height;
	m_framestruct.RGB = m_frame;
	m_framestruct.YUV = 0;
	m_framestruct.rows = m_rows;
	m_framestruct.alpha = 1.0;
	m_framestruct.has_alpha_channel = false;
	m_framestruct.cacheable = false;
	m_framestruct.interlace_mode = 0;
	m_framestruct.first_field = true;
	m_framestruct.scale_x = 0;
	m_framestruct.scale_y = 0;
	m_framestruct.crop_left = 0;
	m_framestruct.crop_right = 0;
	m_framestruct.crop_top = 0;
	m_framestruct.crop_bottom = 0;
	m_framestruct.tilt_x = 0;
	m_framestruct.tilt_y = 0;
	m_framestruct.interlace_mode = INTERLACE_PROGRESSIVE;
	guess_aspect( m_framestruct.w, m_framestruct.h, &m_framestruct );
	m_filename = filename;
	m_ok = true;
}
VideoFileMpeg3::~VideoFileMpeg3()
{
	if ( m_frame )
		delete [] m_frame;
	if ( m_rows )
		delete [] m_rows;
	if ( m_file )
		mpeg3_close( m_file );
}
bool VideoFileMpeg3::ok() { return m_ok; }
int64_t VideoFileMpeg3::length()
{
	return (int64_t)mpeg3_video_frames( m_file, 0 ) * m_ticksPerFrame;
}
frame_struct* VideoFileMpeg3::read()
{
	mpeg3_read_frame( m_file, m_rows, 0, 0, m_width, m_height, m_width, m_height, MPEG3_RGB888, 0 );
	m_framestruct.alpha = 1.0;
	m_framestruct.first_field = m_first_field;
	return &m_framestruct;
}
void VideoFileMpeg3::read( unsigned char** rows, int w, int h )
{
	mpeg3_read_frame( m_file, rows, 0, 0, m_width, m_height, w, h, MPEG3_RGB888, 0 );
}
void VideoFileMpeg3::seek( int64_t position )
{
	int64_t p = position / m_ticksPerFrame;
	if ( p >= mpeg3_video_frames( m_file, 0 ) ) {
		p = mpeg3_video_frames( m_file, 0 ) - 1;
	}
	mpeg3_set_frame( m_file, p, 0 );
}
int64_t VideoFileMpeg3::ticksPerFrame()
{
	return m_ticksPerFrame;
}
void VideoFileMpeg3::seekToFrame( int64_t frame )
{
	mpeg3_set_frame( m_file, frame, 0 );
}



} /* namespace nle */


#endif /* LIBMPEG3 */

