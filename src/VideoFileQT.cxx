/*  VideoFileQT.cxx
 *
 *  Copyright (C) 2005-2008 Richard Spindler <richard.spindler AT gmail.com>
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
#include <lqt_version.h>
#include <colormodels.h>

#include "VideoFileQT.H"
#include "ErrorDialog/IErrorHandler.H"
#include "helper.H"
#include "lqtgavl.h"
#include "LazyFrame.H"

namespace nle
{

VideoFileQT::VideoFileQT( string filename )
	: IVideoFile()
{
	m_ok = false;
	m_qt = NULL;
	m_frame = NULL;
	m_rows = NULL;
	m_lazy_frame = 0;
	char *lqt_filename = const_cast<char *>( filename.c_str() );
	if ( !quicktime_check_sig( lqt_filename ) ) {
		ERROR_DETAIL( "This is not a Quicktime video file" );
		return;
	}
	m_qt = quicktime_open( lqt_filename, true, false );
	if ( !m_qt ) {
		ERROR_DETAIL( "Could not open Quicktime file" );
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
	int64_t frame_duration = lqt_frame_duration( m_qt, 0, 0 );
	int64_t time_scale = lqt_video_time_scale( m_qt, 0 );
	m_ticksPerFrame = ( frame_duration * NLE_TIME_BASE ) / time_scale;
	
	m_width = quicktime_video_width( m_qt, 0 );
	m_height = quicktime_video_height( m_qt, 0 );

	lqt_gavl_get_video_format ( m_qt, 0, &m_video_format, 0 );

	m_frame = gavl_video_frame_create( &m_video_format );
	m_rows = lqt_gavl_rows_create( m_qt, 0 );

	m_lazy_frame = new LazyFrame( &m_video_format );

	//guess_aspect( m_framestruct.w, m_framestruct.h, &m_framestruct );

	m_filename = filename;
	m_ok = true;
}
VideoFileQT::~VideoFileQT()
{
	if ( m_frame )
		gavl_video_frame_destroy( m_frame );
	if ( m_rows )
		lqt_gavl_rows_destroy( m_rows );
	if ( m_qt )
		quicktime_close( m_qt );
	if ( m_lazy_frame )
		delete m_lazy_frame;
}
bool VideoFileQT::ok() { return m_ok; }
int64_t VideoFileQT::length()
{
	return quicktime_video_length( m_qt, 0 ) * m_ticksPerFrame;
}
/*double VideoFileQT::fps()
{
	return quicktime_frame_rate( m_qt, 0 ); 
}*/
LazyFrame* VideoFileQT::read()
{
	lqt_gavl_decode_video( m_qt, 0, m_frame, m_rows );
	m_lazy_frame->put_data( m_frame );
	m_lazy_frame->alpha( 1.0 );
	return m_lazy_frame;
}

void VideoFileQT::seek( int64_t position )
{
	int64_t p = position / m_ticksPerFrame;
	if ( p >= quicktime_video_length( m_qt, 0 ) ) {
		p = quicktime_video_length( m_qt, 0 ) - 1;
	}
	quicktime_set_video_position( m_qt, p, 0 );
}
int64_t VideoFileQT::ticksPerFrame()
{
	return m_ticksPerFrame;
}
void VideoFileQT::seekToFrame( int64_t frame )
{
	quicktime_set_video_position( m_qt, frame, 0 );
}

}; /* namespace nle */
