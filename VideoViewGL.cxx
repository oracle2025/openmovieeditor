/*  VideoViewGL.cxx
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

#include <string> // declares for_each
#include <iostream>

#include <FL/gl.h>

#include "VideoViewGL.H"
#include "globals.H"
#include "SwitchBoard.H"
#include "PlaybackCore.H"
#include "Timeline.H"
#include "events.H"

using namespace std;

namespace nle
{
	
VideoViewGL* g_videoView = 0;

VideoViewGL::VideoViewGL( int x, int y, int w, int h, const char *l )
	: Fl_Gl_Window( x, y, w, h, l )
{
	g_videoView = this;
}

VideoViewGL::~VideoViewGL()
{
}

static GLuint video_canvas[10];
#define T_W_F 1024.0 
#define T_H_F 1024.0
#define T_W 1024 //368
#define T_H 1024 //240

#define TEXTURE_WIDTH 1024.0 
#define TEXTURE_HEIGHT 1024.0

void VideoViewGL::pushFrame( frame_struct* fs )
{
	make_overlay_current();
	fl_draw_box( FL_DIAMOND_UP_BOX, 0, 0, 25, 25, FL_BACKGROUND_COLOR );
	//SwitchBoard::i()->move_cursor();
	make_current();
	if ( !valid() ) {
		glLoadIdentity(); glViewport( 0, 0, w(), h() ); // glViewport( _x, _y, _w, _h );
		glOrtho( 0, 10, 10, 0, -20000, 10000 ); glEnable( GL_BLEND );
		glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
		glEnable (GL_TEXTURE_2D);
	}

	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glBindTexture( GL_TEXTURE_2D, video_canvas[0] );
	if ( fs ) {
		glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, fs->w, fs->h, GL_RGB, GL_UNSIGNED_BYTE, fs->RGB );
	} else {
		static unsigned char p[3 * T_W * T_H] = { 0 };
		glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, T_W, T_H, GL_RGB, GL_UNSIGNED_BYTE, p );
		swap_buffers();
		return;
	}

	float gl_x, gl_y, gl_w, gl_h;
	{
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

	}

	

	float ww = fs->w / TEXTURE_WIDTH;
	float hh = fs->h / TEXTURE_HEIGHT;
	glBegin (GL_QUADS);
		glTexCoord2f (  0.0,      0.0 );
		glVertex3f   (  gl_x,      gl_y, 0.0 );
		glTexCoord2f (  ww,  0.0 );  // (fs->w / 512.0)
		glVertex3f   ( gl_x + gl_w,      gl_y, 0.0 );
		glTexCoord2f (  ww,  hh ); // (368.0 / 512.0) (240.0 / 512.0)
		glVertex3f   ( gl_x + gl_w,     gl_y + gl_h, 0.0 );
		glTexCoord2f (  0.0,      hh ); // (fs->h / 512.0)
		glVertex3f   (  gl_x,     gl_y + gl_h, 0.0 );
	glEnd ();

	
	swap_buffers();
}
void VideoViewGL::draw()
{
	if ( !valid() ) {
		glLoadIdentity(); glViewport( 0, 0, w(), h() ); // glViewport( _x, _y, _w, _h );
		glOrtho( 0, 10, 10, 0, -20000, 10000 ); glEnable( GL_BLEND );
		glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
		glEnable (GL_TEXTURE_2D);
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
		once = false;
	}
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glBindTexture( GL_TEXTURE_2D, video_canvas[0] );
#if 0 
	float gl_x, gl_y, gl_w, gl_h;
	{
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

	

	float ww = fs->w / TEXTURE_WIDTH;
	float hh = fs->h / TEXTURE_HEIGHT;
	glBegin (GL_QUADS);
		glTexCoord2f (  0.0,      0.0 );
		glVertex3f   (  gl_x,      gl_y, 0.0 );
		glTexCoord2f (  ww,  0.0 );  // (fs->w / 512.0)
		glVertex3f   ( gl_x + gl_w,      gl_y, 0.0 );
		glTexCoord2f (  ww,  hh ); // (368.0 / 512.0) (240.0 / 512.0)
		glVertex3f   ( gl_x + gl_w,     gl_y + gl_h, 0.0 );
		glTexCoord2f (  0.0,      hh ); // (fs->h / 512.0)
		glVertex3f   (  gl_x,     gl_y + gl_h, 0.0 );
	glEnd ();
#endif
}

void VideoViewGL::seek( int64_t position )
{
	redraw();
}

void VideoViewGL::play()
{
	if ( g_playbackCore->active() ) {
		return;
	}
	g_timeline->sort();
	g_playbackCore->play();
}

void VideoViewGL::stop()
{
	g_playbackCore->stop();
}

} /* namespace nle */
