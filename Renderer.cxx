/*  Renderer.cxx
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

#include <iostream>
#include <cstring>

#include <lqt.h>
#include <colormodels.h>
#include <gavl/gavl.h>

#include "strlcpy.h"

#include "Renderer.H"
#include "globals.H"
#include "Timeline.H"

using namespace std;

namespace nle
{

static quicktime_t *qt;

Renderer::Renderer( string filename, int w, int h, int framerate, int samplerate )
{
	char buffer[1024];
	m_w = w;
	m_h = h;
	m_framerate = framerate;
	m_samplerate = samplerate;
/*	int w = 368;
	int h = 240;*/
	m_filename = filename;
	strlcpy( buffer, m_filename.c_str(), sizeof(buffer) );
	qt = quicktime_open( buffer, false, true );
	lqt_codec_info_t **codecs = lqt_query_registry( 1, 0, 1, 0 );
	for ( int i = 0; codecs[i]; i++ ) {
		cout << "[" << i << "]" << codecs[i]->name << endl;
		cout << codecs[i]->long_name << endl;
		cout << "---------------------" << endl;
	}
	lqt_codec_info_t *codec = codecs[7];
	lqt_add_audio_track( qt, 2, 48000, 16, codec );
	lqt_destroy_codec_info( codecs );
	codecs = lqt_query_registry( 0, 1, 1, 0 );
	for ( int i = 0; codecs[i]; i++ ) {
		cout << "[" << i << "]" << codecs[i]->name << endl;
		cout << codecs[i]->long_name << endl;
		cout << "---------------------" << endl;
	}
	codec = codecs[22];
	lqt_add_video_track( qt, m_w, m_h, 1200, 30000, codec ); // Was bedeuted 1001 zum Teufel? => 30000 / 1001 == 29.97
	                                                         // 30000 / 1200 == 25
	lqt_destroy_codec_info( codecs );
	lqt_set_cmodel( qt, 0, BC_RGB888 );
	//[10]jpeg
	//[3]ulaw
	//[6]lame
}
Renderer::~Renderer()
{
	if (qt)
		quicktime_close( qt );
}
void scale_it( frame_struct* src, frame_struct* dst )
{

	gavl_rectangle_t src_rect;
	gavl_rectangle_t dst_rect;
	gavl_video_format_t format_src;
	gavl_video_format_t format_dst;

	gavl_video_frame_t * frame_src, * frame_dst;

	frame_src = gavl_video_frame_create( 0 );
	frame_dst = gavl_video_frame_create( 0 );

	frame_src->strides[0] = src->w * 3;
	frame_src->planes[0] = src->RGB;
	
	frame_dst->strides[0] = dst->w * 3;
	frame_dst->planes[0] = dst->RGB;


	gavl_video_scaler_t *scaler;
	scaler = gavl_video_scaler_create();
	gavl_video_options_set_scale_mode( gavl_video_scaler_get_options( scaler ), GAVL_SCALE_AUTO );
	

	format_dst.frame_width  = dst->w;
	format_dst.frame_height = dst->h;
	format_dst.image_width  = dst->w;
	format_dst.image_height = dst->h;;
	format_dst.pixel_width = 1;
	format_dst.pixel_height = 1;
	format_dst.colorspace = GAVL_RGB_24;
	
	format_src.frame_width  = src->w;
	format_src.frame_height = src->h;
	format_src.image_width  = src->w;
	format_src.image_height = src->h;;
	format_src.pixel_width = 1;
	format_src.pixel_height = 1;
	format_src.colorspace = GAVL_RGB_24;

	src_rect.x = 0;
	src_rect.y = 0;
	src_rect.w = src->w;
	src_rect.h = src->h;

	dst_rect.x = 0;
	dst_rect.y = 0;
	dst_rect.w = dst->w;
	dst_rect.h = dst->h;

	
	if ( gavl_video_scaler_init( scaler, GAVL_RGB_24, &src_rect, &dst_rect, &format_src, &format_dst ) == -1 ) {
		cerr << "Video Scaler Init failed" << endl;
		return;
	}
	gavl_video_scaler_scale( scaler, frame_src, frame_dst );

	frame_src->planes[0] = 0;
	frame_dst->planes[0] = 0;
	gavl_video_frame_destroy( frame_src );
	gavl_video_frame_destroy( frame_dst );

	gavl_video_scaler_destroy( scaler );

//	frame_src->planes
//	frame_src->strides

}
void Renderer::go()
{
	

	
	// Ein eigener Scaler für jeden nicht passenden Clip??
	g_timeline->seek( 0 );
	g_timeline->sort();
	
	
	int res;
	float buffer[256];
	float left_buffer[128];
	float right_buffer[128];
	float *buffer_p[2] = { left_buffer, right_buffer };

	int64_t fcnt = 0;

	frame_struct enc_frame;
	enc_frame.x = enc_frame.y = 0;
	enc_frame.w = m_w;
	enc_frame.h = m_h;
	enc_frame.nr = 0;
	enc_frame.YUV = 0;
	enc_frame.RGB = new unsigned char[m_w * m_h * 3];
	enc_frame.rows = new unsigned char*[m_h];
	for (int i = 0; i < m_h; i++) {
                enc_frame.rows[i] = enc_frame.RGB + m_w * 3 * i;
	}


	
	do {
		if ( fcnt > 1601 ) {
			if ( frame_struct *fs = g_timeline->nextFrame() ) {
				if ( fs->w != m_w || fs->h != m_h ) {
					scale_it( fs, &enc_frame  );
					quicktime_encode_video( qt, enc_frame.rows, 0 );
				
				} else {
					quicktime_encode_video( qt, fs->rows, 0 );
				}
				fcnt = 0;
			}
		}

		
		res = g_timeline->fillBuffer( buffer, 128 ); //Das sollte etwas mehr als 128 sein
		for ( int i = 0; i < res; i++ ) {
			left_buffer[i] = buffer[i*2];
			right_buffer[i] = buffer[i*2+1];
		}
		lqt_encode_audio_track( qt, 0, buffer_p, res, 0 );
		fcnt += 128;

	} while ( res == 128 );
	delete [] enc_frame.RGB;
	delete [] enc_frame.rows;
}


} /* namespace nle */
