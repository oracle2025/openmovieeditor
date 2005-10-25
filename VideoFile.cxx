/*  VideoFile.cxx
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

//FIXME or DROPME ;)

/*
g++ -o VideoFileTest VideoFileTest.cxx VideoFile.cxx -lavformat -lavcodec -lz -L../ffmpeg/libavcodec -L../ffmpeg/libavformat -I../ffmpeg/libavcodec -I../ffmpeg/libavformat
 */
#include <avcodec.h>
#include <avformat.h>
#include <string>
#include <iostream>
#include <stdint.h>
#include <stdio.h>
#include "VideoFile.H"
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/fl_draw.H>

namespace nle
{

VideoFile::VideoFile( const std::string &filename, int mode )
{
	bool ok = 0;
	if ( av_open_input_file( &m_FormatCtx, filename.c_str(), NULL, 0, NULL ) != 0 ) {
		return;
	}

	if ( av_find_stream_info( m_FormatCtx ) < 0 ) {
		return;
	}

	dump_format( m_FormatCtx, 0, filename.c_str(), false );

	m_videoStream = -1;

	for ( int i = 0; i < m_FormatCtx->nb_streams; i++ ) {
		if ( m_FormatCtx->streams[i]->codec.codec_type == CODEC_TYPE_VIDEO ) {
			m_videoStream = i;
			break;
		}
	}
	if ( m_videoStream == -1 ) {
		return;
	}
	m_CodecCtx=&m_FormatCtx->streams[m_videoStream]->codec;
	
	m_Codec = avcodec_find_decoder( m_CodecCtx->codec_id );
	if ( m_Codec == NULL ) {
		return;
	}

	if ( avcodec_open( m_CodecCtx, m_Codec ) < 0 ) {
		return;
	}
	
	if ( m_CodecCtx->frame_rate > 1000 && m_CodecCtx->frame_rate_base == 1 ) {
		m_CodecCtx->frame_rate_base = 1000;
	}

	m_Frame = avcodec_alloc_frame();

	m_FrameRGB = avcodec_alloc_frame();

	if ( m_FrameRGB == NULL ) {
		return;
	}

	m_numBytes = avpicture_get_size( PIX_FMT_RGB24, m_CodecCtx->width, m_CodecCtx->height );
	m_buffer = new uint8_t[m_numBytes];

	avpicture_fill( (AVPicture*)m_FrameRGB, m_buffer, PIX_FMT_RGB24, m_CodecCtx->width, m_CodecCtx->height );

	m_frameCnt = calc_frame_cnt();
	m_duration = calc_duration();

	std::cout << "VideoFile() m_frameCnt: " << m_frameCnt << std:: endl;
	std::cout << "VideoFile() m_duration: " << m_duration << std:: endl;
}

VideoFile::~VideoFile()
{
	delete [] m_buffer;
	av_free( m_FrameRGB );
	av_free( m_Frame );
	avcodec_close( m_CodecCtx );
	av_close_input_file( m_FormatCtx );
}
int64_t VideoFile::calc_frame_cnt()
{
	std::cout << "hh" << std::endl;
	printf("hh123\n");
	int64_t fps = m_CodecCtx->frame_rate / m_CodecCtx->frame_rate_base;
	return fps * ( m_FormatCtx->duration - m_FormatCtx->start_time );
}
int64_t VideoFile::calc_duration()
{
	return m_FormatCtx->duration - m_FormatCtx->start_time;
}
//(int64_t)( cur_stream->ic->start_time + frac * cur_stream->ic->duration )
int64_t VideoFile::frame_to_timestamp( int64_t frame )
{
	return (int64_t)( ( float(frame) / float(m_frameCnt) ) * float(m_duration) );
}
void VideoFile::SeekFloat( float frac )
{
	av_seek_frame( m_FormatCtx, -1, (int64_t)( m_FormatCtx->start_time + frac * m_FormatCtx->duration ), 0 );
}
void VideoFile::SeekFrame( int64_t frame )
{
	std::cout << "frame_to_timestamp: " << frame_to_timestamp( frame ) << std::endl;
	av_seek_frame( m_FormatCtx, -1, frame_to_timestamp( frame ), 0 );
}
void VideoFile::readFrame()
{
	if(av_read_frame(m_FormatCtx, &m_packet)>=0) {
		if(m_packet.stream_index==m_videoStream) {
			avcodec_decode_video(m_CodecCtx, m_Frame, &m_frameFinished, 
			m_packet.data, m_packet.size);
			if(m_frameFinished) {
				// Convert the image from its native format to RGB
				img_convert((AVPicture *)m_FrameRGB, PIX_FMT_RGB24, 
				(AVPicture*)m_Frame, m_CodecCtx->pix_fmt, m_CodecCtx->width, 
				m_CodecCtx->height);
				fl_draw_image((uchar*)m_FrameRGB->data[0], 0, 0, m_CodecCtx->width, m_CodecCtx->height);
			}
		}
		av_free_packet(&m_packet);
	}
	//std::cout << "ReadFrame frame_number: " << m_CodecCtx->frame_number << std::endl;
}

void VideoFile::WriteFrame()
{
}
	
}
