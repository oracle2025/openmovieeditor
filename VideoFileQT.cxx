/*  VideoFileQT.cxx
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

#include <iostream>
#include <string.h>

#include <lqt.h>
#include <colormodels.h>
//#include <quicktime/lqt.h>
//#include <quicktime/colormodels.h>

#include "VideoFileQT.H"

using namespace std;

namespace nle
{

VideoFileQT::VideoFileQT( const char* filename )
{
	//m_scaler = NULL;
	m_ok = false;
	m_qt = NULL;
	m_frame = NULL;
	m_rows = NULL;
	char *lqt_sucks_filename = strdup( filename );
	if ( !quicktime_check_sig( lqt_sucks_filename ) ) {
		cerr << "Check Sig failed" << endl;
		return;
	}
	m_qt = quicktime_open( lqt_sucks_filename, true, false );
	free(lqt_sucks_filename);
	if ( quicktime_video_tracks( m_qt ) == 0 ) {
		cerr << "No Video Tracks" << endl;
		return;
	}
	if ( !quicktime_supported_video( m_qt, 0 ) ) {
		cerr << "Video Codec not supported" << endl;
		return;
	}
	lqt_set_cmodel( m_qt, 0, BC_RGB888);
	m_width = quicktime_video_width( m_qt, 0 );
	m_height = quicktime_video_height( m_qt, 0 );
	m_frame = new unsigned char[m_width * m_height * 3];
	m_rows = new unsigned char*[m_height];
	for (int i = 0; i < m_height; i++) {
                m_rows[i] = m_frame + m_width * 3 * i;
	}
	m_framestruct.x = 0;
	m_framestruct.y = 0;
	m_framestruct.w = quicktime_video_width( m_qt, 0 );
	m_framestruct.h = quicktime_video_height( m_qt, 0 );
	m_framestruct.RGB = m_frame;
	m_framestruct.YUV = 0;
	m_framestruct.rows = m_rows;
	cout << "Video Duration: " << lqt_video_duration( m_qt, 0 ) << endl;
	cout << "Width: " << quicktime_video_width( m_qt, 0 ) << endl;
	cout << "Height: " << quicktime_video_height( m_qt, 0 ) << endl;
	cout << "Video FPS: " << quicktime_frame_rate( m_qt, 0 ) << endl;
	cout << "Video Frame Duration: " << lqt_frame_duration( m_qt, 0, 0 ) << endl;
	cout << "Video Length: " << quicktime_video_length( m_qt, 0 ) << endl;
	cout << "Video Timescale: " << lqt_video_time_scale( m_qt, 0 ) << endl;
	cout << "Audio Length: " << quicktime_audio_length( m_qt, 0 ) << endl;
	cout << "Audio Samplerate: " << quicktime_sample_rate( m_qt, 0 ) << endl;
	strncpy(m_filename, filename, STR_LEN);
	m_ok = true;
/*	unsigned char p[25*25*3];
	unsigned char* ro[25];
	for ( int i = 0; i < 25; i++ ) {
		ro[i] = p + i * 25 * 3;
	}
	quicktime_decode_scaled( m_qt, 0, 0, 25, 25, 25, 25, BC_RGB888, ro, 0 );*/
}
VideoFileQT::~VideoFileQT()
{
	if ( m_frame )
		delete [] m_frame;
	if ( m_rows )
		delete [] m_rows;
	if ( m_qt )
		quicktime_close( m_qt );
/*	if ( m_scaler )
		gavl_video_scaler_destroy( m_scaler ); */
}
void VideoFileQT::initScaler( unsigned int w, unsigned int h )
{
	m_scaled_w = w;
	m_scaled_h = h;
/*	if ( !m_scaler ) {
		m_scaler = gavl_video_scaler_create();
	} else {
		gavl_video_scaler_destroy( m_scaler );
		m_scaler = gavl_video_scaler_create();
	}
	gavl_video_format_t src_format;
	gavl_video_format_t dst_format;
	gavl_rectangle_t src_rect;
	gavl_rectangle_t dst_rect;
	src_rect.w = src_format.frame_width = w; //TODO: no no
	src_rect.h = src_format.frame_height = h;
	src_rect.x = src_rect.y = dst_rect.x = dst_rect.y = 0;
	dst_rect.w = dst_format.frame_width = w;
	dst_rect.h = dst_format.frame_height = h;
	src_format.colorspace = dst_format.colorspace = GAVL_RGB_24;
	gavl_video_scaler_init( m_scaler, GAVL_RGB_24, &src_rect, &dst_rect, &src_format, &dst_format);*/
}
void VideoFileQT::readScaled( unsigned char** rows )
{
/*	gavl_video_format_t dst_format;
	gavl_video_frame_t* src = gavl_video_frame_create( 0 );
	gavl_video_frame_t* dst = gavl_video_frame_create( 0 );
	return (frame_struct*)0;*/
	quicktime_decode_scaled( m_qt, 0, 0, m_width, m_height, m_scaled_w, m_scaled_h, BC_RGB888, rows, 0 );
}
bool VideoFileQT::ok() { return m_ok; }
int64_t VideoFileQT::length()
{
	return quicktime_video_length( m_qt, 0 );
}
double VideoFileQT::fps()
{
	return quicktime_frame_rate( m_qt, 0 ); 
}
frame_struct* VideoFileQT::read()
{
	quicktime_decode_video( m_qt, m_rows, 0);
	return &m_framestruct;
}
void VideoFileQT::read( unsigned char** rows, int w, int h )
{
	quicktime_decode_scaled( m_qt, 0, 0, m_width, m_height, w, h, BC_RGB888, rows, 0 );
}
void VideoFileQT::seek( int64_t frame )
{
	quicktime_set_video_position( m_qt, frame, 0 );
}

}; /* namespace nle */
