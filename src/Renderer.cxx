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

#include <cstring>

#include <colormodels.h>

#include "strlcpy.h"
#include "sl/sl.h"

#include "Renderer.H"
#include "globals.H"
#include "Timeline.H"
#include "ProgressDialog/IProgressListener.H"
#include "Codecs.H"
#include "render_helper.H"

render_frame_size fs720x576 = { 720, 576 };
render_frame_size fs360x288 = { 360, 288 };
render_frame_size fs640x480 = { 640, 480 };
render_frame_size fs320x240 = { 320, 240 };

namespace nle
{

static quicktime_t *qt;



Renderer::Renderer( string filename, int w, int h, int framerate, int samplerate, CodecParameters* params )
{
	char buffer[1024];
	m_w = w;
	m_h = h;
	m_framerate = framerate;
	m_samplerate = samplerate;
	m_filename = filename;
	strlcpy( buffer, m_filename.c_str(), sizeof(buffer) );
	qt = quicktime_open( buffer, false, true );
	if ( !qt ) {
		fl_alert( "Could not open file.\n%s", filename.c_str() );
		return;
	}

	params->set( qt, m_w, m_h );

	lqt_set_cmodel( qt, 0, BC_RGB888 );
	return;

	
	lqt_codec_info_t **codecs = lqt_query_registry( 1, 0, 1, 0 );
	lqt_codec_info_t *codec = codecs[7];
	lqt_add_audio_track( qt, 2, 48000, 16, codec );
	lqt_destroy_codec_info( codecs );
	codecs = lqt_query_registry( 0, 1, 1, 0 );
	codec = codecs[22];
	lqt_add_video_track( qt, m_w, m_h, 1200, 30000, codec ); // Was bedeuted 1001 zum Teufel? => 30000 / 1001 == 29.97
	                                                         // 30000 / 1200 == 25
	lqt_destroy_codec_info( codecs );
	lqt_set_cmodel( qt, 0, BC_RGB888 );
}
bool Renderer::ok() {
	return qt;
}
Renderer::~Renderer()
{
	if (qt)
		quicktime_close( qt );
}

//#define AUDIO_BUFFER_SIZE 480
#define AUDIO_BUFFER_SIZE 23040
void Renderer::go( IProgressListener* l )
{
	if ( !qt ) {
		return;
	}
	if ( l ) {
		l->start();
	}
	g_timeline->seek( 0 );
	g_timeline->sort();
	
	int res;
	float buffer[AUDIO_BUFFER_SIZE*2];
	float left_buffer[AUDIO_BUFFER_SIZE];
	float right_buffer[AUDIO_BUFFER_SIZE];
	float *buffer_p[2] = { left_buffer, right_buffer };

	int64_t length = g_timeline->length();
	int64_t current_frame = 0;

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


/*
 * 12 * 1920 = 23040
 * This is the chunk of audio that will be written between 12 Frames
 */
	bool run = true;
	do {
		res = g_timeline->fillBuffer( buffer, AUDIO_BUFFER_SIZE );
		g_timeline->sampleseek( 0, AUDIO_BUFFER_SIZE );
		for ( int i = 0; i < res; i++ ) {
			left_buffer[i] = buffer[i*2];
			right_buffer[i] = buffer[i*2+1];
		}
		lqt_encode_audio_track( qt, 0, buffer_p, res, 0 );

		for ( int i = 0; i < 12; i++ ) {
			g_timeline->getBlendedFrame( &enc_frame );
			quicktime_encode_video( qt, enc_frame.rows, 0 );
			current_frame++;
			if ( l ) {
				if ( l->progress( (double)current_frame / length ) ) {
					run = false;
					break;
				}
			}
		}
		
	} while ( res == AUDIO_BUFFER_SIZE && run );
	
	delete [] enc_frame.RGB;
	delete [] enc_frame.rows;
	if ( l ) {
		l->end();
	}
}


} /* namespace nle */
