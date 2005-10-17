/*  VideoViewGL.cxx
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

#include <string> // declares for_each
#include <iostream>

#include <FL/gl.h>

#include "VideoViewGL.H"
#include "VideoTrack.H"
#include "globals.H"
#include "Timeline.H"
#include "Sound.H"
#include "SwitchBoard.H"
#include "Texter.H"

#include <unistd.h>

#define LEN_TIMEOUT 0.05

using namespace std;
using namespace Magick;
namespace nle
{
static bool vv_play;
static Fl_Window *vv_cb_window;
static void vv_callback( void* )
{
	if ( !vv_play )
		return;
	if ( !g_sound->stillPlaying() )
		return;
	SwitchBoard::i()->move_cursor();
	Fl::repeat_timeout( LEN_TIMEOUT, vv_callback );
}
VideoViewGL* g_videoView;
VideoViewGL::VideoViewGL( int x, int y, int w, int h, const char *l )
	: Fl_Gl_Window( x, y, w, h, l )
{
	m_snd = new Sound( this );
	vv_cb_window = this->window();
	m_playing = false;
	m_seek = false;
	g_videoView = this;
	m_playingPosition = 0;
	m_A = 0;
	m_B = 0;
	m_C = 0;
}
VideoViewGL::~VideoViewGL()
{
	delete m_snd;
	m_snd = NULL;
}
GLuint video_canvas[10];
//#define T_W 368
//#define T_H 240
#define T_W_F 1024.0 
#define T_H_F 1024.0
#define T_W 1024 
#define T_H 1024
void VideoViewGL::draw()
{
	if ( !valid() ) {
/*		int _w, _h, _x, _y;
		float a_b = ( 368.0 / 240.0 );
		{
			if ( w() < h() * a_b ) {
				_h = int( w() / a_b );
				_w = w();
			} else {
				_h = h();
				_w = int( h() * a_b );
			}
			_x = ( w() - _w ) / 2;
			_y = ( h() - _h ) / 2;
		}*/
		glLoadIdentity(); glViewport( 0, 0, w(), h() ); // glViewport( _x, _y, _w, _h );
		glOrtho( 0, 10, 10, 0, -20000, 10000 ); glEnable( GL_BLEND );
		glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
		glEnable (GL_TEXTURE_2D);
    	//	loadTGA ("texture.tga", 13);
	}
	static bool once = true;
	static unsigned char p[3 * T_W * T_H] = { 0 };
	if (once) {
		glGenTextures( 10, video_canvas );
		for ( int i = 0; i < 10; i++ ) {
			glBindTexture (GL_TEXTURE_2D, video_canvas[i] );
			glPixelStorei (GL_UNPACK_ALIGNMENT, 1);
			glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
			if ( i != 1 ) {glTexImage2D (GL_TEXTURE_2D, 0, GL_RGB, T_W, T_H, 0, GL_RGB, GL_UNSIGNED_BYTE, p);}
		}
		{
			Texter te;
			glBindTexture( GL_TEXTURE_2D, video_canvas[1] );
			glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, 512, 512, 0, GL_RGBA, GL_UNSIGNED_BYTE, te.generateText(512, 512,"SUNFUN") );
		}
		once = false;
	}
	static frame_struct *fs = 0;
	if (m_seek) {
		m_seek = false;
		fs = g_timeline->getFrame( m_seekPosition );
		if ( fs ) {
			glBindTexture( GL_TEXTURE_2D, video_canvas[0] );
			glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, fs->w, fs->h, GL_RGB, GL_UNSIGNED_BYTE, fs->RGB );
		} else {
			glBindTexture( GL_TEXTURE_2D, video_canvas[0] );
			glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, T_W, T_H, GL_RGB, GL_UNSIGNED_BYTE, p );
		}

	}
	static int64_t last_frame = m_playingPosition; //FIXME Vorsicht, falls mehrere Instanzen existieren
	if (m_playing) {
		if ( last_frame == m_playingPosition ) {
			m_A++;
			return;
		} else {
			m_B++;
			last_frame = m_playingPosition;
		}
		//for_each( g_timeline->getVideoTracks()->begin(), g_timeline->getVideoTracks()->end(), draw_track_helper );
		fs = g_timeline->nextFrame();
		if ( fs ) {
			glBindTexture( GL_TEXTURE_2D, video_canvas[0] );
			glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, fs->w, fs->h, GL_RGB, GL_UNSIGNED_BYTE, fs->RGB );
		} else {
			glBindTexture( GL_TEXTURE_2D, video_canvas[0] );
			glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, T_W, T_H, GL_RGB, GL_UNSIGNED_BYTE, p );
		}
	} else {
	}
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	static float gl_x, gl_y, gl_w, gl_h;
	if (fs) { //TODO: Optimize ?
		static int w_buf = fs->w; 
		static int h_buf = fs->h;
		static int ww_buf = w();
		static int wh_buf = h();
		static bool a = true;
		if ( a || !( w_buf == fs->w && h_buf == fs->h && ww_buf == w() && wh_buf == h() ) ) {
			a = false;
			float f_v = ( (float)fs->w / (float)fs->h );
			float f_w = ( (float)w() / (float)h() );
			float f_g = f_v / f_w;
			if ( f_g > 1.0 ) {
				gl_h = 10.0 / f_g;
				gl_w = 10.0;
			} else {
				gl_h = 10.0;
				gl_w = f_g * 10.0;
			}
			gl_x = ( 10.0 - gl_w ) / 2;
			gl_y = ( 10.0 - gl_h ) / 2;
			w_buf = fs->w;
			h_buf = fs->h;
			ww_buf = w();
			wh_buf = h();
		}
	}
	
	if (fs) {
		glBindTexture (GL_TEXTURE_2D, video_canvas[0] );
		glBegin (GL_QUADS);
		float ww = fs->w / T_W_F;
		float hh = fs->h / T_H_F;
		glTexCoord2f (  0.0,      0.0 );
		glVertex3f   (  gl_x,      gl_y, 0.0 );
		glTexCoord2f (  ww,  0.0 );  // (fs->w / 512.0)
		glVertex3f   ( gl_x + gl_w,      gl_y, 0.0 );
		glTexCoord2f (  ww,  hh ); // (368.0 / 512.0) (240.0 / 512.0)
		glVertex3f   ( gl_x + gl_w,     gl_y + gl_h, 0.0 );
		glTexCoord2f (  0.0,      hh ); // (fs->h / 512.0)
		glVertex3f   (  gl_x,     gl_y + gl_h, 0.0 );
		glEnd ();
	}
/*	{       // Titel Overlay
		glBindTexture (GL_TEXTURE_2D, video_canvas[1] );
		
		glEnable(GL_BLEND); // Transparenz
		glColor4f(1.0f,1.0f,1.0f,0.5f);
		
		glBegin (GL_QUADS);
		glTexCoord2f (  0.0,      0.0 );
		glVertex3f   (  gl_x,      gl_y, 0.0 );
		glTexCoord2f (  0.71875,  0.0 ); 
		glVertex3f   ( gl_x + gl_w,      gl_y, 0.0 );
		glTexCoord2f (  0.71875,  0.46875 ); // (368.0 / 512.0) (240.0 / 512.0)
		glVertex3f   ( gl_x + gl_w,     gl_y + gl_h, 0.0 );
		glTexCoord2f (  0.0,      0.46875 );
		glVertex3f   (  gl_x,     gl_y + gl_h, 0.0 );
		glEnd ();

		glDisable(GL_BLEND);
	}*/

}

void VideoViewGL::nextFrame( int64_t frame )
{
/*	static int64_t last_frame = frame;
	if (frame == last_frame) {
		cout << "XXX" << endl;
	}
	last_frame = frame;
	m_C++;*/
	Fl::lock();
	m_playingPosition = frame;
	redraw();
	Fl::awake();
	Fl::unlock();
}
void VideoViewGL::seek( int64_t position )
{
	m_seekPosition = position;
	m_seek = true;
	redraw();
}
void VideoViewGL::play()
{
	if ( m_playing )
		return;
	m_playing = true;
	m_playingPosition = 0;
	g_timeline->sort();
	m_snd->Play();
	vv_play = true;
	Fl::add_timeout( LEN_TIMEOUT, vv_callback, NULL );
}
void VideoViewGL::stop()
{
	if ( !m_playing )
		return;
	cout << "A: " << m_A << " B: " << m_B << " C: " << m_C << endl;
	m_playing = false;
	m_snd->Stop();
	vv_play = false;
}

} /* namespace nle */
