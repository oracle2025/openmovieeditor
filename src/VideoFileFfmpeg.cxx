/*  VideoFileFfmpeg.cxx
 *
 *  Copyright (C) 2006 Richard Spindler <richard.spindler AT gmail.com>
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

#ifdef AVCODEC

#include "VideoFileFfmpeg.H"
#include "render_helper.H"
#include "ErrorDialog/IErrorHandler.H"

namespace nle
{

VideoFileFfmpeg::VideoFileFfmpeg( string filename )
{
	cout << "VideoFileFfmpeg" << endl;
	m_ok = false;
	m_frame = NULL;
	m_rows = NULL;
	m_avFrame = NULL;
	m_avFrameRGB = NULL;
	m_formatContext = NULL;
	
	AVInputFormat *file_iformat = av_find_input_format( filename.c_str() );
	
	if ( av_open_input_file( &m_formatContext, filename.c_str(), file_iformat, 0, NULL ) < 0 ) {
		ERROR_DETAIL( "This file cannot be read by ffmpeg" );
		return;
	}
	if ( av_find_stream_info( m_formatContext ) < 0 ) {
		ERROR_DETAIL( "av_find_stream_info failed" );
		return;
	}
	m_videoStream = -1;
	for ( int i = 0; i < m_formatContext->nb_streams; i++ ) {
		if ( m_formatContext->streams[i]->codec->codec_type == CODEC_TYPE_VIDEO ) {
			m_videoStream = i;
			break;
		}
	}
	if ( m_videoStream == -1 ) {
		ERROR_DETAIL( "No Video Stream found in file" );
		return;
	}
	m_codecContext = m_formatContext->streams[m_videoStream]->codec;
	m_codec = avcodec_find_decoder( m_codecContext->codec_id );
	if ( m_codec == NULL ) {
		ERROR_DETAIL( "Codec not supported" );
		return;
	}
	if ( avcodec_open( m_codecContext, m_codec ) < 0 ) {
		ERROR_DETAIL( "avcodec_open failed" );
		return;
	}
	m_avFrame = avcodec_alloc_frame();
	m_avFrameRGB = avcodec_alloc_frame();
	if ( m_avFrame == NULL || m_avFrameRGB == NULL ) {
		ERROR_DETAIL( "could not allocate frames" );
		return;
	}
	m_width = m_codecContext->width;
	m_height = m_codecContext->height;
	int byteLen = avpicture_get_size( PIX_FMT_RGB24, m_width, m_height );
	m_frame = new unsigned char[byteLen];
	avpicture_fill( (AVPicture *)m_avFrameRGB, m_frame, PIX_FMT_RGB24, m_width, m_height );
	m_rows = new unsigned char*[m_height];
	for ( int i = 0; i < m_height; i++ ) {
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
	m_filename = filename;

	//m_formatContext->streams[m_videoStream]->r_frame_rate;
	//time_base
	m_framerate = av_q2d( m_formatContext->streams[m_videoStream]->r_frame_rate );
	if ( m_framerate < 24.9 || m_framerate > 25.1 ) {
		CLEAR_ERRORS();
		cout << "Wrong Framerate: " << m_framerate << endl;
		ERROR_DETAIL( "Video framerates other than 25 are not supported" );
		return;
	}

	cout << "FFMPEG Framerate: " << m_framerate << endl;
	
	//m_framerate = (double)(m_codecContext->frame_rate) / (double)(m_codecContext->frame_rate_base);
	int64_t len = m_formatContext->duration - m_formatContext->start_time;
	m_length = (int64_t)( len * m_framerate / AV_TIME_BASE );
	
	m_ok = true;
}
VideoFileFfmpeg::~VideoFileFfmpeg()
{
	if ( m_avFrame )
		av_free( m_avFrame );
	if ( m_avFrameRGB )
		av_free( m_avFrameRGB );
	if ( m_frame )
		delete [] m_frame;
	if ( m_rows )
		delete [] m_rows;
	if ( m_formatContext )
		av_close_input_file( m_formatContext );
}
bool VideoFileFfmpeg::ok() { return m_ok; }
int64_t VideoFileFfmpeg::length() { return m_length; }
double VideoFileFfmpeg::fps() { return m_framerate; }

frame_struct* VideoFileFfmpeg::read()
{
	int frameFinished;
	while ( 1 ) {
		if ( av_read_frame( m_formatContext, &m_packet ) >= 0 ) {
			if ( m_packet.stream_index == m_videoStream ) {
				avcodec_decode_video( m_codecContext, m_avFrame, &frameFinished, m_packet.data, m_packet.size );
				if ( frameFinished ) {
					img_convert( (AVPicture*)m_avFrameRGB, PIX_FMT_RGB24,
						  (AVPicture*)m_avFrame, m_codecContext->pix_fmt, m_width, m_height );
					av_free_packet( &m_packet );
					return &m_framestruct;
				}
			} 
			av_free_packet( &m_packet );
		} else {
			return 0;
		}
	}
	return 0;
}
void VideoFileFfmpeg::read( unsigned char** rows, int w, int h )
{
	frame_struct fs;
	fs.w = w;
	fs.h = h;
	fs.RGB = rows[0];
	read();
	scale_it( &m_framestruct, &fs );
}
void VideoFileFfmpeg::seek( int64_t frame )
{
	avcodec_flush_buffers( m_codecContext );
	const int64_t time_base = 1;
	int64_t timestamp = ( frame * AV_TIME_BASE * time_base ) / (int64_t)m_framerate;
	av_seek_frame( m_formatContext, -1, timestamp, 0 );
}

} /* namespace nle */

#endif /* AVCODEC */

