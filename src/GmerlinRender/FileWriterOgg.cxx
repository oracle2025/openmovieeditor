/*  FileWriterOgg.cxx
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

#include "FileWriterOgg.H"
#include "LazyFrame.H"
#include <cstring>
#include "fps_definitions.H"
#include <iostream>
#include <cassert>
#include "../ErrorDialog/IErrorHandler.H"
#include <gavl/metatags.h>

namespace nle
{

const char* name_of_parameter_type( bg_parameter_type_t type )
{
	switch ( type ) {
		case BG_PARAMETER_SECTION:
			return "BG_PARAMETER_SECTION";
		case BG_PARAMETER_CHECKBUTTON:
			return "BG_PARAMETER_CHECKBUTTON";
		case BG_PARAMETER_INT:
			return "BG_PARAMETER_INT";
		case BG_PARAMETER_FLOAT:
			return "BG_PARAMETER_FLOAT";
		case BG_PARAMETER_SLIDER_INT:
			return "BG_PARAMETER_SLIDER_INT";
		case BG_PARAMETER_SLIDER_FLOAT:
			return "BG_PARAMETER_SLIDER_FLOAT";
		case BG_PARAMETER_STRING:
			return "BG_PARAMETER_STRING";
		case BG_PARAMETER_STRING_HIDDEN:
			return "BG_PARAMETER_STRING_HIDDEN";
		case BG_PARAMETER_STRINGLIST:
			return "BG_PARAMETER_STRINGLIST";
		case BG_PARAMETER_COLOR_RGB:
			return "BG_PARAMETER_COLOR_RGB";
		case BG_PARAMETER_COLOR_RGBA:
			return "BG_PARAMETER_COLOR_RGBA";
		case BG_PARAMETER_FONT:
			return "BG_PARAMETER_FONT";
		case BG_PARAMETER_DEVICE:
			return "BG_PARAMETER_DEVICE";
		case BG_PARAMETER_FILE:
			return "BG_PARAMETER_FILE";
		case BG_PARAMETER_DIRECTORY:
			return "BG_PARAMETER_DIRECTORY";
		case BG_PARAMETER_MULTI_MENU:
			return "BG_PARAMETER_MULTI_MENU";
		case BG_PARAMETER_MULTI_LIST:
			return "BG_PARAMETER_MULTI_LIST";
		case BG_PARAMETER_MULTI_CHAIN:
			return "BG_PARAMETER_MULTI_CHAIN";
		case BG_PARAMETER_TIME:
			return "BG_PARAMETER_TIME";
		case BG_PARAMETER_POSITION:
			return "BG_PARAMETER_POSITION";
		case BG_PARAMETER_BUTTON:
			return "BG_PARAMETER_BUTTON";
	}
}

FileWriterOgg::FileWriterOgg( const char* filename )
{
	m_audio_frame = 0;
	m_audio_converter = 0;
	m_vorbis_audio_frame = 0;
	m_ok = false;
	m_cfg_registry = bg_cfg_registry_create();
	m_plugin_cfg = bg_cfg_registry_find_section( m_cfg_registry, "plugins" );
	m_plugin_registry = bg_plugin_registry_create( m_plugin_cfg );
	
	const bg_plugin_info_t* plugin_info = bg_plugin_find_by_name ( m_plugin_registry, "e_theora" );
	m_plugin_handle = bg_plugin_load( m_plugin_registry, plugin_info );

	m_encoder = (const bg_encoder_plugin_t*)m_plugin_handle->plugin;
	m_instance = m_encoder->common.create();

	gavl_metadata_t metadata;
	gavl_metadata_init (&metadata);
	gavl_metadata_set (&metadata, GAVL_META_ARTIST, "");
	gavl_metadata_set (&metadata, GAVL_META_TITLE, "");
	gavl_metadata_set (&metadata, GAVL_META_ALBUM, "");
	gavl_metadata_set (&metadata, GAVL_META_TRACKNUMBER, "");
	gavl_metadata_set (&metadata, GAVL_META_DATE, "");
	gavl_metadata_set (&metadata, GAVL_META_GENRE, "");
	gavl_metadata_set (&metadata, GAVL_META_COMMENT, "");
	gavl_metadata_set (&metadata, GAVL_META_SOFTWARE, "Open Movie Editor");
	gavl_metadata_set (&metadata, GAVL_META_COPYRIGHT, "");
	if ( !m_encoder->open( m_instance, filename, &metadata, 0 ) ) {
		ERROR_DETAIL( "Could not open Video file" );
		return;
	}
	
	m_audio_parameters = m_encoder->get_audio_parameters( m_instance );
	/* "Vorbis encoder" -> "VBR" -> Quality */
	m_video_parameters = m_encoder->get_video_parameters( m_instance );
	/* "Nominal quality" */


	m_video_format.frame_width = 640;
	m_video_format.frame_height = 480;
	m_video_format.image_width = 640;
	m_video_format.image_height = 480;
	m_video_format.pixel_width = 1;
	m_video_format.pixel_height = 1;
	m_video_format.pixelformat = GAVL_PIXELFORMAT_NONE;
	m_video_format.frame_duration = 1200;
	m_video_format.timescale = 30000;
	m_video_format.framerate_mode = GAVL_FRAMERATE_CONSTANT;
	m_video_format.chroma_placement = GAVL_CHROMA_PLACEMENT_DEFAULT;
	m_video_format.interlace_mode = GAVL_INTERLACE_NONE;

	m_audio_format.samples_per_frame = 19200;
	m_audio_format.samplerate = 48000;
	m_audio_format.num_channels = 2;
	m_audio_format.sample_format = GAVL_SAMPLE_FLOAT;
	m_audio_format.interleave_mode = GAVL_INTERLEAVE_ALL;
	m_audio_format.center_level = 1.0;
	m_audio_format.rear_level = 1.0;
	m_audio_format.channel_locations[0] = GAVL_CHID_FRONT_LEFT;
	m_audio_format.channel_locations[1] = GAVL_CHID_FRONT_RIGHT;

	m_audio_frame = gavl_audio_frame_create( 0 );

	m_encoder->add_video_stream( m_instance, 0, &m_video_format );

	m_encoder->add_audio_stream( m_instance, 0, &m_audio_format );

	for ( int i = 0; m_audio_parameters[i].name; i++ ) {
		std::cout << "Name: " << m_audio_parameters[i].name << " Type: " << name_of_parameter_type( m_audio_parameters[i].type ) << std::endl;
		if ( m_audio_parameters[i].type == BG_PARAMETER_MULTI_MENU ) {
			for ( int j = 0; m_audio_parameters[i].multi_names[j]; j++ ) {
				std::cout << "\tName: " << m_audio_parameters[i].multi_names[j] << std::endl;
				for ( int k = 0; m_audio_parameters[i].multi_parameters[j][k].name; k++ ) {
					std::cout << "\t\tName: " << m_audio_parameters[i].multi_parameters[j][k].name << " Type: " << name_of_parameter_type( m_audio_parameters[i].multi_parameters[j][k].type ) << std::endl;
					if ( BG_PARAMETER_STRINGLIST == m_audio_parameters[i].multi_parameters[j][k].type ) {
						for ( int l = 0; m_audio_parameters[i].multi_parameters[j][k].multi_names[l]; l++ ) {
							std::cout << "\t\t\tName: " << m_audio_parameters[i].multi_parameters[j][k].multi_names[l] << std::endl;
						}
					}
				}
			}
		}
		//m_encoder->set_audio_parameter( m_instance, 0, m_audio_parameters[i].name, &m_audio_parameters[i].val_default );
	}
	bg_parameter_value_t value;
	value.val_str = "vorbis";
	m_encoder->set_audio_parameter( m_instance, 0, "codec", &value );
	value.val_str = "vbr";
	m_encoder->set_audio_parameter( m_instance, 0, "bitrate_mode", &value );
	value.val_i = 128; // not used for vbr
	m_encoder->set_audio_parameter( m_instance, 0, "nominal_bitrate", &value );
	value.val_f = 3.0;
	m_encoder->set_audio_parameter( m_instance, 0, "quality", &value );
	value.val_i = 0;
	m_encoder->set_audio_parameter( m_instance, 0, "min_bitrate", &value );
	value.val_i = 0;
	m_encoder->set_audio_parameter( m_instance, 0, "max_bitrate", &value );
	//m_encoder->set_audio_parameter( m_instance, 0, 0, 0 );

	for ( int i = 0; m_video_parameters[i].name; i++ ) {
		std::cout << "Name: " << m_video_parameters[i].name << " Type: " << name_of_parameter_type( m_video_parameters[i].type ) << std::endl;
		m_encoder->set_video_parameter( m_instance, 0, m_video_parameters[i].name, &m_video_parameters[i].val_default );
	}
	value.val_i = 10; /* 0 bis 63 */
	m_encoder->set_video_parameter( m_instance, 0, "quality", &value );
	//m_encoder->set_video_parameter( m_instance, 0, 0, 0 );
	


	m_sample_tics = 0;

	m_ok = true;

	/* call get_audio_format, get_video_format after start() */

/*
	
	m_lazy_frame = new LazyFrame( clip->w(), clip->h() );
	m_gavl_frame = gavl_video_frame_create( m_lazy_frame->format() );
	m_lazy_frame->put_data( m_gavl_frame );*/

	
	/* Open The Gmerlin Plugin for encoding */
}

FileWriterOgg::~FileWriterOgg()
{
	m_encoder->close( m_instance, 0 );
	m_encoder->common.destroy( m_instance );
	bg_plugin_unref( m_plugin_handle );
	bg_plugin_registry_destroy( m_plugin_registry );
	bg_cfg_registry_destroy( m_cfg_registry );

	if ( m_audio_frame ) {
		gavl_audio_frame_null( m_audio_frame );
		gavl_audio_frame_destroy( m_audio_frame );
	}
	if ( m_vorbis_audio_frame ) {
		gavl_audio_frame_destroy( m_vorbis_audio_frame );
	}
	if ( m_audio_converter ) {
		gavl_audio_converter_destroy( m_audio_converter );
	}
}

void FileWriterOgg::videoQuality( int q )
{
	bg_parameter_value_t value;
	value.val_i = q; /* 0 bis 63 */
	m_encoder->set_video_parameter( m_instance, 0, "quality", &value );
}

void FileWriterOgg::audioQuality( float q )
{
	bg_parameter_value_t value;
	value.val_f = q;
	m_encoder->set_audio_parameter( m_instance, 0, "quality", &value );
}

void FileWriterOgg::resolution( int w, int h )
{
}

void FileWriterOgg::done()
{
	m_encoder->start( m_instance );

	m_encoder->get_audio_format( m_instance, 0, &m_vorbis_audio_format );
	m_encoder->get_video_format( m_instance, 0, &m_video_format );

	m_vorbis_audio_frame = gavl_audio_frame_create( &m_vorbis_audio_format );

	m_audio_converter = gavl_audio_converter_create();
	gavl_audio_converter_init( m_audio_converter, &m_audio_format, &m_vorbis_audio_format );

}

void FileWriterOgg::encodeVideoFrame( LazyFrame* frame )
{
	frame->set_target( &m_video_format );
	
	gavl_video_frame_t* gavl_frame = frame->target();
	
	gavl_frame->timestamp = m_timestamp;
	m_timestamp += m_video_format.frame_duration;
	
	m_encoder->write_video_frame( m_instance, gavl_frame, 0 );
}

void FileWriterOgg::encodeAudioFrame( float* buffer, int frames )
{
	assert( frames <= 19200 );

	//memcpy( m_audio_frame->samples.f, buffer, frames*2*sizeof(float) );
	m_audio_frame->samples.f = buffer;
	m_audio_frame->valid_samples = frames;

	m_audio_frame->timestamp = m_sample_tics;
	m_sample_tics += frames;

	//m_audio_frame->channel_stride = 0;

	gavl_audio_convert( m_audio_converter, m_audio_frame, m_vorbis_audio_frame );

	/* Für den Vorbis Encoder gibts kein INTERLEAVE Also muss hier eh umgewandelt werden*/

	/*
	float * buffer;
	int num_samples;

	gavl_audio_frame_t * frame = gavl_audio_frame_create(NULL);
	frame->samples.f = buffer;
	frame->valid_samples = num_samples;

	Interleave mode is GAVL_INTERLEAVE_ALL then.
	*/


	m_encoder->write_audio_frame( m_instance, m_vorbis_audio_frame, 0 );
}

bool FileWriterOgg::ok()
{
	return m_ok;
}
video_format* FileWriterOgg::format()
{
	strcpy( m_ome_format.name, "" );
	strcpy( m_ome_format.video_codec, "" );
	strcpy( m_ome_format.audio_codec, "" );
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


} /* namespace nle */
