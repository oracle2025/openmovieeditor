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

render_frame_size fs720x576 = { 720, 576, 4, 3, ( 4.0 / 3.0 ), 10 };
render_frame_size fs768x576 = { 768, 576, 4, 3, ( 4.0 / 3.0 ), 0 };
render_frame_size fs384x288 = { 384, 288, 4, 3, ( 4.0 / 3.0 ), 0 };
render_frame_size fs640x480 = { 640, 480, 4, 3, ( 4.0 / 3.0 ), 0 };
render_frame_size fs320x240 = { 320, 240, 4, 3, ( 4.0 / 3.0 ), 0 };

render_fps_chunks fps25x48000 =    { 1200, 30000, 19200, 10 };
render_fps_chunks fps29_97x48000 = { 1001, 30000, 16016, 10 };
render_fps_chunks fps24x48000 =    { 1250, 30000, 20000, 10 };
render_fps_chunks fps15x48000 =    { 2000, 30000, 32000, 10 };
render_fps_chunks fps50x48000 =    { 600,  30000, 19200, 20 };
render_fps_chunks fps60x48000 =    { 500,  30000, 16000, 20 };

/*
	int frame_duration;
	int timescale;
	int audio_frames_per_chunk;
	int video_frames_per_chunk;
*/
namespace nle
{

static quicktime_t *qt;


Renderer::Renderer( string filename, render_frame_size* format, render_fps_chunks* framerate, int samplerate, CodecParameters* params )
{
	p_timeline = 0;
	char buffer[1024];
	m_w = format->w;
	m_h = format->h;
	m_fps = *framerate;
	m_samplerate = samplerate;
	m_filename = filename;
	strlcpy( buffer, m_filename.c_str(), sizeof(buffer) );
	qt = quicktime_open( buffer, false, true );
	if ( !qt ) {
		fl_alert( "Could not open file.\n%s", filename.c_str() );
		return;
	}

	params->set( qt, m_w, m_h, &m_fps );

	lqt_set_cmodel( qt, 0, BC_RGB888 );
	
	Timeline* x = g_timeline;
	p_timeline = new Timeline();
	g_timeline = x;

	p_timeline->render_mode( true );
	string temp_filename;
	strcpy( buffer,"OME_RENDER_XXXXXX" );
	temp_filename = string(g_homefolder);
	
	temp_filename += ("/.openme/temp" PREF_FILE_ADD);
	temp_filename += buffer;
	g_timeline->write( temp_filename, "" );

	p_timeline->read( temp_filename );
	
	p_timeline->prepareFormat( m_w, m_h, format->aspect_w, format->aspect_h, format->aspect, format->analog_blank );

	return;
}
bool Renderer::ok() {
	return qt;
}
Renderer::~Renderer()
{
	if (qt)
		quicktime_close( qt );
	p_timeline->unPrepareFormat();
	if ( p_timeline ) {
		Timeline* x = g_timeline;
		delete p_timeline;
		g_timeline = x;
	}
}

//#define AUDIO_BUFFER_SIZE 480
//#define AUDIO_BUFFER_SIZE 23040
#define AUDIO_BUFFER_SIZE 32000
void Renderer::go( IProgressListener* l )
{
	
	if ( !qt ) {
		return;
	}
	if ( l ) {
		l->start();
	}
	//TODO:  Make a copy of the timeline?
	//Use Write project to tmp file
	

	
	p_timeline->seek( 0 );
	p_timeline->sort();
	
	int res;
	float buffer[AUDIO_BUFFER_SIZE*2];
	float left_buffer[AUDIO_BUFFER_SIZE];
	float right_buffer[AUDIO_BUFFER_SIZE];
	float *buffer_p[2] = { left_buffer, right_buffer };

	int64_t length = (int64_t)( p_timeline->length() * ( (float)m_fps.timescale / (float)m_fps.frame_duration  ) );
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
	int64_t position = 0;
	int64_t frame_length = 35280000 / m_fps.timescale * m_fps.frame_duration;
	bool run = true;
	int frames_to_write;
	do {
		res = p_timeline->fillBuffer( buffer, m_fps.audio_frames_per_chunk );
		p_timeline->sampleseek( 0, m_fps.audio_frames_per_chunk );
		for ( int i = 0; i < res; i++ ) {
			left_buffer[i] = buffer[i*2];
			right_buffer[i] = buffer[i*2+1];
		}
		lqt_encode_audio_track( qt, 0, buffer_p, res, 0 );
		for ( int i = 0; i < m_fps.video_frames_per_chunk; i++ ) {
			p_timeline->getBlendedFrame( position, &enc_frame );
			position += frame_length;
			quicktime_encode_video( qt, enc_frame.rows, 0 );
			current_frame++;
			if ( l ) {
				if ( l->progress( (double)current_frame / length ) ) {
					run = false;
					break;
				}
			}
		}
	} while ( res == m_fps.audio_frames_per_chunk && run );
	
	delete [] enc_frame.RGB;
	delete [] enc_frame.rows;
	if ( l ) {
		l->end();
	}
}


} /* namespace nle */
