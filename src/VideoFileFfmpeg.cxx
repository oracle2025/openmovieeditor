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
#include "globals.H"
#include "helper.H"
#include "LazyFrame.H"

namespace nle
{

VideoFileFfmpeg::VideoFileFfmpeg( string filename )
	: IVideoFile()
{
	m_ok = false;
	m_lazy_frame = 0;
	m_gavl_frame = 0;
	m_avFrame = NULL;
	m_avFrameRGB = NULL;
	m_formatContext = NULL;
	m_frame = 0;
#ifdef SWSCALE
	pSWSCtx = 0;
#endif
	
	
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
	for ( unsigned int i = 0; i < m_formatContext->nb_streams; i++ ) {
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
	gavl_video_format_t format;
	format.frame_width  = m_width;
	format.frame_height = m_height;
	format.image_width  = m_width;
	format.image_height = m_height;
	format.pixel_width = 1;
	format.pixel_height = 1;
	format.pixelformat = GAVL_RGB_24;
	format.interlace_mode = GAVL_INTERLACE_NONE;
	m_lazy_frame = new LazyFrame( &format );
	m_gavl_frame = gavl_video_frame_create( 0 );
	m_lazy_frame->put_data( m_gavl_frame );
	m_gavl_frame->planes[0] = m_frame;
	m_gavl_frame->strides[0] = m_width * 3;

	m_filename = filename;

	int64_t num = m_formatContext->streams[m_videoStream]->r_frame_rate.num;
	int64_t den = m_formatContext->streams[m_videoStream]->r_frame_rate.den;

	m_ticksPerFrame = ( den * NLE_TIME_BASE ) / num;
	
	int64_t len = m_formatContext->duration;
	m_length = (int64_t)( len * NLE_TIME_BASE / AV_TIME_BASE ) - m_ticksPerFrame;
#ifdef SWSCALE
	pSWSCtx = sws_getContext(m_width, m_height, m_codecContext->pix_fmt, m_width, m_height, PIX_FMT_RGB24, SWS_BICUBIC, NULL, NULL, NULL);
#endif
	
	m_ok = true;
}
VideoFileFfmpeg::~VideoFileFfmpeg()
{
	if ( m_avFrame )
		av_free( m_avFrame );
	if ( m_avFrameRGB )
		av_free( m_avFrameRGB );
	if ( m_lazy_frame ) {
		delete m_lazy_frame;
	}
	if ( m_gavl_frame ) {
		gavl_video_frame_null( m_gavl_frame );
		gavl_video_frame_destroy( m_gavl_frame );
	}
	if ( m_frame )
		delete [] m_frame;
	if ( m_formatContext )
		av_close_input_file( m_formatContext );
#ifdef SWSCALE
	if ( pSWSCtx ) {
		sws_freeContext(pSWSCtx);
		pSWSCtx = 0;
	}
#endif
}
bool VideoFileFfmpeg::ok() { return m_ok; }
int64_t VideoFileFfmpeg::length() { return m_length; }
//double VideoFileFfmpeg::fps() { return m_framerate; }

LazyFrame* VideoFileFfmpeg::read()
{
	int frameFinished;
	while ( 1 ) {
		if ( av_read_frame( m_formatContext, &m_packet ) >= 0 ) {
			if ( m_packet.stream_index == m_videoStream ) {
				avcodec_decode_video( m_codecContext, m_avFrame, &frameFinished, m_packet.data, m_packet.size );
				if ( frameFinished ) {
#if 0
					cout << "m_avFrame->interlaced_frame: " << m_avFrame->interlaced_frame << endl;
					cout << "m_avFrame->top_field_first: " << m_avFrame->top_field_first << endl;
#endif
#ifdef SWSCALE
					sws_scale(pSWSCtx,
						((AVPicture*)m_avFrame)->data,
						((AVPicture*)m_avFrame)->linesize, 0,
						m_codecContext->height,
						((AVPicture*)m_avFrameRGB)->data,
						((AVPicture*)m_avFrameRGB)->linesize);
#else
					img_convert( (AVPicture*)m_avFrameRGB, PIX_FMT_RGB24,
						  (AVPicture*)m_avFrame, m_codecContext->pix_fmt, m_width, m_height );
#endif

					av_free_packet( &m_packet );
					return m_lazy_frame;
				}
			} 
			av_free_packet( &m_packet );
		} else {
			return 0;
		}
	}
	return 0;
}
/*
http://lists.mplayerhq.hu/pipermail/ffmpeg-devel/2005-July/002320.html
On Tuesday 2005 July 05 06:24, Steve Willis wrote:

> I would also very much like some more information on this. I asked a
> similar question last week, and am very grateful for the response. I am
> a newcomer to avcodec/avformat, and could really use some tips or
> example code on the best way to seek to a particular frame given its
> time, even if that frame is not a keyframe. Could someone explain the
> HURRY_UP mode that was suggested for quickly skipping from the nearest
> keyframe to the actual frame I am seeking?
>
> Like Ian, I need some help understanding how to map a timestamp to a
> framenumber as well.

I don't know for sure is my answer.  However, I am having success treating the 
units to av_seek_frame() as being in AV_TIME_BASE units (at present this is a 
64 bit integer representing time in microseconds passed 
AVFormatContext.start_time)

As to seeking to a particular (non-key) frame, I believe the suggestion from 
Rich Felker was that you seek to the nearest key frame using av_seek_frame() 
then enable hurry_up mode until you hit the frame you want. e.g. (error 
checking and initialisation ommitted)

av_seek_frame( fmtCtx, -1, TARGET_PTS, AVSEEK_FLAG_BACKWARD );
CodecCtx->hurry_up = 1;
do {
    av_read_frame( fmtCtx, &Packet );
    // should really be checking that this is a video packet
    MyPts = Packet.pts / Packet.duration *
	AV_TIME_BASE / av_q2d( Stream->r_frame_rate);
    if( MyPts > TARGET_PTS )
        break;
    avcodec_decode_video( CodecCtx, pFrame, &gotFrame, Packet.data,
        Packet.size );
    av_free_packet( &Packet );
} while(1);
CodecCtx->hurry_up = 0;
// Now near TARGET_PTS with Packet ready for decode (and free)

The calculation of MyPts is probably the answer to your question about frame 
numbers and timestamp.  I am sure that I am not doing this in the correct 
way, but it's working for me :-)

Hope that's helped.



*/
void VideoFileFfmpeg::seek( int64_t position )
{
	avcodec_flush_buffers( m_codecContext );
	int64_t timestamp = ( position * AV_TIME_BASE ) / (int64_t)NLE_TIME_BASE;
	av_seek_frame( m_formatContext, -1, m_formatContext->start_time + timestamp, AVSEEK_FLAG_BACKWARD );
}
void VideoFileFfmpeg::seekToFrame( int64_t frame )
{
	seek( frame * m_ticksPerFrame );
/*	avcodec_flush_buffers( m_codecContext );
	int64_t timestamp = ( frame * AV_TIME_BASE ) / (int64_t)m_framerate;
	av_seek_frame( m_formatContext, -1, timestamp, 0 );*/
}
int64_t VideoFileFfmpeg::ticksPerFrame()
{
	return m_ticksPerFrame;
}

} /* namespace nle */

#endif /* AVCODEC */

