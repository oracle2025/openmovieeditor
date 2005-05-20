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

#define LEN_TIMEOUT 0.01

using namespace std;

namespace nle
{
static bool vv_play;
static Fl_Window *vv_cb_window;
static void vv_callback( void* )
{
	if ( !vv_play )
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
}
VideoViewGL::~VideoViewGL()
{
	delete m_snd;
	m_snd = NULL;
}
static int texture_counter;
GLuint video_canvas[10];
//#define T_W 368
//#define T_H 240
#define T_W 512 
#define T_H 512
#if 0
static void draw_track_helper( VideoTrack* track )
{
	if ( texture_counter >= 1 )
		return;
	frame_struct *fs = track->nextFrame();
	if (!fs)
		return;
	glBindTexture ( GL_TEXTURE_2D, video_canvas[texture_counter] );
	texture_counter++;
	glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, fs->w, fs->h, GL_RGB, GL_UNSIGNED_BYTE, fs->RGB );
	GLenum e = glGetError();
	while ( e != GL_NO_ERROR ) {
		switch (e) {
			case GL_INVALID_ENUM:
				cerr << "GL_INVALID_ENUM" << endl;
				break;
			case GL_INVALID_VALUE:
				cerr << "GL_INVALID_VALUE" << endl;
				break;
			case GL_INVALID_OPERATION:
				cerr << "GL_INVALID_OPERATION" << endl;
				break;
			case GL_STACK_OVERFLOW:
				cerr << "GL_STACK_OVERFLOW" << endl;
				break;
			case GL_STACK_UNDERFLOW:
				cerr << "GL_STACK_UNDERFLOW" << endl;
				break;
			case GL_OUT_OF_MEMORY:
				cerr << "GL_OUT_OF_MEMORY" << endl;
				break;
			case GL_TABLE_TOO_LARGE:
				cerr << "GL_TABLE_TOO_LARGE" << endl;
				break;
		}
		e = glGetError();
	}
	// TODO geht das so überhaupt? sollte man nicht mehrere Texturen initialisieren
}
#endif
void VideoViewGL::draw()
{
	if ( !valid() ) {
		glLoadIdentity(); glViewport( 0, 0, w(), h() );
		glOrtho( 0, 10, 10, 0, -20000, 10000 ); glEnable( GL_BLEND );
		glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
		glEnable (GL_TEXTURE_2D);
    	//	loadTGA ("texture.tga", 13);
	}
	static bool once = true;
	if (once) {
		glGenTextures( 10, video_canvas );
		unsigned char p[3 * T_W * T_H] = { 0 };
		for ( int i = 0; i < 10; i++ ) {
/*			if ( loadTGA ("texture.tga", video_canvas[i]) != 1)
				cerr << "TGA Error" << endl;*/
			glBindTexture (GL_TEXTURE_2D, video_canvas[i] );
			glPixelStorei (GL_UNPACK_ALIGNMENT, 1);
			glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
			glTexImage2D (GL_TEXTURE_2D, 0, GL_RGB, T_W, T_H, 0, GL_RGB, GL_UNSIGNED_BYTE, p);
		}
		once = false;
	}
	texture_counter = 0;
	if (m_seek) {
		m_seek = false;
		frame_struct *fs = g_timeline->frame( m_seekPosition );
		if ( fs ) {
			glBindTexture( GL_TEXTURE_2D, video_canvas[texture_counter] );
			glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, fs->w, fs->h, GL_RGB, GL_UNSIGNED_BYTE, fs->RGB );
		}
	}
	if (m_playing) {
		//for_each( g_timeline->getVideoTracks()->begin(), g_timeline->getVideoTracks()->end(), draw_track_helper );
		frame_struct *fs = g_timeline->nextFrame();
		if ( fs ) {
			glBindTexture( GL_TEXTURE_2D, video_canvas[texture_counter] );
			glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, fs->w, fs->h, GL_RGB, GL_UNSIGNED_BYTE, fs->RGB );
		}
	} else {
	}
	texture_counter++;
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	for ( int i = 0; i < texture_counter; i++ ) {
		glBindTexture (GL_TEXTURE_2D, video_canvas[i] );
		glBegin (GL_QUADS);
		glTexCoord2f (0.0, 0.0);
		glVertex3f (0.0, 0.0, 0.0);
		glTexCoord2f (0.71875, 0.0);
		glVertex3f (10.0, 0.0, 0.0);
		glTexCoord2f (0.71875, 0.46875);
		glVertex3f (10.0, 10.0, 0.0);
		glTexCoord2f (0.0, 0.46875);
		glVertex3f (0.0, 10.0, 0.0);
		glEnd ();
	}
	/*
	glTexSubImage2D
	http://www.gamedev.net/reference/articles/article947.asp
	http://developer.apple.com/samplecode/OpenGL_Movie/listing4.html
	 */
}

void VideoViewGL::nextFrame( int64_t frame )
{
	Fl::lock();
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
	m_playing = true;
	g_timeline->reset();
	m_snd->Play();
	vv_play = true;
	Fl::add_timeout( LEN_TIMEOUT, vv_callback, NULL );
}
void VideoViewGL::stop()
{
	m_playing = false;
	m_snd->Stop();
	vv_play = false;
}

} /* namespace nle */
