/*  VideoWriterMP4.cxx
 *
 *  Copyright (C) 2009 Richard Spindler <richard.spindler AT gmail.com>
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

#include "VideoWriterMP4.H"
#include "LazyFrame.H"
#include "lqtgavl.h"
#include "fps_definitions.H"
#include <cstring>
#include <cassert>

namespace nle
{

VideoWriterMP4::VideoWriterMP4( const char* filename )
{
	m_rows = 0;
	m_ok = false;
	m_timestamp = 0;

	m_encoder = lqt_open_write ( filename, LQT_FILE_MP4 );

	lqt_codec_info_t** video_codec = lqt_find_video_codec_by_name( "x264" );
	lqt_codec_info_t** audio_codec = lqt_find_audio_codec_by_name( "faac" );

	if ( !video_codec || !audio_codec || !video_codec[0] || !audio_codec[0] ) {
		goto cleanup;
	}

	initFormats();

	lqt_gavl_add_video_track( m_encoder, &m_video_format, video_codec[0] );
	lqt_gavl_add_audio_track( m_encoder, &m_audio_format, audio_codec[0] );

	lqt_gavl_get_video_format( m_encoder, 0, &m_video_format, 1 );
	//lqt_gavl_get_audio_format( m_encoder, 0, &m_audio_format, 1 ); //Not
	//necessary for encoding Audio?

	m_rows = lqt_gavl_rows_create( m_encoder, 0 );

	m_ok = true;
cleanup:
	lqt_destroy_codec_info( video_codec );
	lqt_destroy_codec_info( audio_codec );
}
VideoWriterMP4::~VideoWriterMP4()
{
	if ( m_encoder ) {
		quicktime_close( m_encoder );
		m_encoder = 0;
	}
	if ( m_rows ) {
		lqt_gavl_rows_destroy( m_rows );
	}
}
void VideoWriterMP4::initFormats()
{
	m_video_format.frame_width = 480;
	m_video_format.frame_height = 360;
	m_video_format.image_width = 480;
	m_video_format.image_height = 360;
	m_video_format.pixel_width = 1;
	m_video_format.pixel_height = 1;
	m_video_format.pixelformat = GAVL_PIXELFORMAT_NONE;
	m_video_format.frame_duration = 1200;
	m_video_format.timescale = 30000;
	m_video_format.framerate_mode = GAVL_FRAMERATE_CONSTANT;
	m_video_format.chroma_placement = GAVL_CHROMA_PLACEMENT_DEFAULT;
	m_video_format.interlace_mode = GAVL_INTERLACE_NONE;

	m_audio_format.samples_per_frame = 1024;
	m_audio_format.samplerate = 48000;
	m_audio_format.num_channels = 2;
	m_audio_format.sample_format = GAVL_SAMPLE_NONE;
	m_audio_format.interleave_mode = GAVL_INTERLEAVE_ALL;
	m_audio_format.center_level = 1.0;
	m_audio_format.rear_level = 1.0;
	m_audio_format.channel_locations[0] = GAVL_CHID_FRONT_LEFT;
	m_audio_format.channel_locations[1] = GAVL_CHID_FRONT_RIGHT;
}
void VideoWriterMP4::encodeVideoFrame( LazyFrame* frame )
{
	frame->set_target( &m_video_format );
	gavl_video_frame_t* gavl_frame = frame->target();
	gavl_frame->timestamp = m_timestamp;
	m_timestamp += m_video_format.frame_duration;
	lqt_gavl_encode_video ( m_encoder, 0, gavl_frame, m_rows );
}
void VideoWriterMP4::encodeAudioFrame( float* buffer, int frames )
{
	float *buffer_p[2] = { m_left_buffer, m_right_buffer };
	assert( frames <= 32000 );
	for ( int i = 0; i < frames; i++ ) {
		m_left_buffer[i] = buffer[i*2];
		m_right_buffer[i] = buffer[i*2+1];
	}
	lqt_encode_audio_track( m_encoder, 0, buffer_p, frames, 0 );
}
video_format* VideoWriterMP4::format()
{
	strcpy( m_ome_format.name, "youtube mp4" );
	strcpy( m_ome_format.video_codec, "x264" );
	strcpy( m_ome_format.audio_codec, "faac" );
	m_ome_format.w = m_video_format.frame_width;
	m_ome_format.h = m_video_format.frame_height;
	m_ome_format.pixel_w = m_video_format.pixel_width;
	m_ome_format.pixel_h = m_video_format.pixel_height;
	m_ome_format.framerate.frame_duration = m_video_format.frame_duration;
	m_ome_format.framerate.timescale = m_video_format.timescale;
	m_ome_format.framerate.audio_frames_per_chunk = 19200;
	m_ome_format.framerate.video_frames_per_chunk = 10;
	m_ome_format.pixel_aspect_ratio = 1.0;
	m_ome_format.interlacing = INTERLACE_PROGRESSIVE;
	m_ome_format.samplerate = m_audio_format.samplerate;
	return &m_ome_format;
}
bool VideoWriterMP4::ok()
{
	return m_ok;
}

} /* namespace nle */

