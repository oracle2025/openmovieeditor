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
//#include <gavl.h>

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
	lqt_codec_info_t *codec = codecs[3];
	lqt_add_audio_track( qt, 2, 48000, 16, codec );
	lqt_destroy_codec_info( codecs );
	codecs = lqt_query_registry( 0, 1, 1, 0 );
	for ( int i = 0; codecs[i]; i++ ) {
		cout << "[" << i << "]" << codecs[i]->name << endl;
		cout << codecs[i]->long_name << endl;
		cout << "---------------------" << endl;
	}
	codec = codecs[10];
	lqt_add_video_track( qt, m_w, m_h, 1001, 30000, codec );
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
/*	static int last_src_w = 0;
	static int last_src_h = 0;

	gavl_rectangle_t src_rect;
	gavl_rectangle_t dst_rect;
	gavl_video_format_t format_src;
	gavl_video_format_t format_dst;

	gavl_video_frame_t * frame_src, * frame_dst;


	gavl_video_scaler_t *scaler;
	scaler = gavl_video_scaler_create();
	

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
	
	if ( gavl_video_scaler_init( scaler, GAVL_RGB_24, &src_rect, &dst_rect, &format_src, &format_dst ) < 0 ) {
		cerr << "Video Scaler Init failed" << endl;
	}


	frame_src = gavl_video_frame_create( 0 );
	frame_dst = gavl_video_frame_create( 0 );*/

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
	do {
		if ( fcnt > 1601 ) {
			if ( frame_struct *fs = g_timeline->nextFrame() ) {
				if ( fs->w != m_w || fs->h != m_h ) {

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
}


} /* namespace nle */
