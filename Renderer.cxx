/*  Renderer.cxx
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
#include <cstring>

#include <lqt.h>
#include <colormodels.h>

#include "Renderer.H"
#include "globals.H"
#include "Timeline.H"

using namespace std;

namespace nle
{

static quicktime_t *qt;

Renderer::Renderer( const char* filename )
{
	int w = 368;
	int h = 240;
	m_filename = strdup( filename );
	qt = quicktime_open( m_filename, false, true );
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
	lqt_add_video_track( qt, w, h, 1001, 30000, codec );
	lqt_destroy_codec_info( codecs );
	lqt_set_cmodel( qt, 0, BC_RGB888 );
	//[10]jpeg
	//[3]ulaw
	//[6]lame
}
Renderer::~Renderer()
{
	if ( m_filename )
		delete [] m_filename;
	if (qt)
		quicktime_close( qt );
}
void Renderer::go()
{
	g_timeline->seek( 0 );
	g_timeline->sort();
	
	
	int res;
	float buffer[256];
	float left_buffer[128];
	float right_buffer[128];
	float *buffer_p[2] = { left_buffer, right_buffer };

/*
gavl_video_frame_t * f
lqt_decode_video(e->file, f->planes,  e->video_streams[stream].quicktime_index);
 */
	
#if 0
	while ( frame_struct *fs = g_timeline->nextFrame() ) {
		quicktime_encode_video( qt, fs->rows, 0 );
	}
	do {
		res = g_timeline->fillBuffer( buffer, 128 );
		for ( int i = 0; i < res; i++ ) {
			left_buffer[i] = buffer[i*2];
			right_buffer[i] = buffer[i*2+1];
		}
		lqt_encode_audio_track( qt, 0, buffer_p, res, 0 );
	} while ( res == 128 );

#else

	int64_t fcnt = 0;
	do {
		if ( fcnt > 1601 ) {
			if ( frame_struct *fs = g_timeline->nextFrame() ) {
				// Scale to fit output size
				quicktime_encode_video( qt, fs->rows, 0 );
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
#endif
}


} /* namespace nle */
