/*  VideoFileQT.cxx
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

#include <cstring>

#include <lqt.h>
#include <colormodels.h>

#include "VideoFileQT.H"
#include "ErrorDialog/IErrorHandler.H"

namespace nle
{

VideoFileQT::VideoFileQT( string filename )
{
	m_ok = false;
	m_qt = NULL;
	m_frame = NULL;
	m_rows = NULL;
	char *lqt_sucks_filename = const_cast<char *>( filename.c_str() );
	if ( !quicktime_check_sig( lqt_sucks_filename ) ) {
		ERROR_DETAIL( "This is not a Quicktime video file" );
		return;
	}
	m_qt = quicktime_open( lqt_sucks_filename, true, false );
	if ( !m_qt ) {
		ERROR_DETAIL( "This is not a Quicktime video file" );
		return;
	}
	if ( quicktime_video_tracks( m_qt ) == 0 ) {
		ERROR_DETAIL( "This Quicktime file does not have a video track" );
		return;
	}
	if ( !quicktime_supported_video( m_qt, 0 ) ) {
		ERROR_DETAIL( "This Video Codec is not supported" );
		return;
	}
	// check frame rate
//	if ( quicktime_frame_rate( m_qt, 0 ) != 25.0 ) { // This is a double (DANGEROUS)
//		ERROR_DETAIL( "Video framerates other than 25 are not supported" );
//		return;
//	}
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
	m_framestruct.alpha = 1.0;
	m_framestruct.has_alpha_channel = false;
/*	cout << "Video Duration: " << lqt_video_duration( m_qt, 0 ) << endl;
	cout << "Width: " << quicktime_video_width( m_qt, 0 ) << endl;
	cout << "Height: " << quicktime_video_height( m_qt, 0 ) << endl;
	cout << "Video FPS: " << quicktime_frame_rate( m_qt, 0 ) << endl;
	cout << "Video Frame Duration: " << lqt_frame_duration( m_qt, 0, 0 ) << endl;
	cout << "Video Length: " << quicktime_video_length( m_qt, 0 ) << endl;
	cout << "Video Timescale: " << lqt_video_time_scale( m_qt, 0 ) << endl;
	cout << "Audio Length: " << quicktime_audio_length( m_qt, 0 ) << endl;
	cout << "Audio Samplerate: " << quicktime_sample_rate( m_qt, 0 ) << endl;*/
	m_filename = filename;
	m_ok = true;
}
VideoFileQT::~VideoFileQT()
{
	if ( m_frame )
		delete [] m_frame;
	if ( m_rows )
		delete [] m_rows;
	if ( m_qt )
		quicktime_close( m_qt );
}
void VideoFileQT::initScaler( unsigned int w, unsigned int h )
{
	m_scaled_w = w;
	m_scaled_h = h;
}
void VideoFileQT::readScaled( unsigned char** rows )
{
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
	m_framestruct.alpha = 1.0;
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
