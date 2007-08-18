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

#include <FL/gl.h>
#include <FL/Fl_PNG_Image.H>

#include "VideoViewGL.H"
#include "globals.H"
#include "SwitchBoard.H"
#include "IPlaybackCore.H"
#include "Timeline.H"
#include "events.H"
#include "render_helper.H"
#include "int_float_scale.H"

extern bool g_16_9;
namespace nle
{

struct texture_frame_cache {
	void* p;
	int dirty;
};

static struct texture_frame_cache tcache[10];
	
VideoViewGL* g_videoView = 0;

void reset_cache()
{
	for ( int i = 0; i < 10; i++ ) {
		tcache[i].p = 0;
		tcache[i].dirty = 0;
	}
}

VideoViewGL::VideoViewGL( int x, int y, int w, int h, const char *l )
	: Fl_Gl_Window( x, y, w, h, l )
{
	g_videoView = this;
	m_zoom = 0.0;
	m_seekPosition = 0;
	reset_cache();
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
//static unsigned char pulldown_frame[3 * 1024 * 1024];

void VideoViewGL::drawVideoBorder()
{
	//int w_ = 1024;
	int w_ = g_16_9 ? 1024 : 768;
	int h_ = 576;
	float gl_x, gl_y, gl_w, gl_h;
	{
		float f_v = ( (float)w_ / (float)h_ );
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

	glDisable (GL_TEXTURE_2D);
	if ( g_black_borders && !g_16_9 ) {
		int w_ = 768;
		//int w_ = 1024;
		int h_ = 576;
		float gl_x, gl_y, gl_w, gl_h;
		{
			float f_v = ( (float)w_ / (float)h_ );
			float f_w = ( (float)w() / (float)h() );
			float f_g = f_v / f_w;
			if ( f_g > 1.0 ) {
				gl_h = 10.0 / f_g;
				gl_w = 10.0;
			} else {
				gl_h = 10.0;
				gl_w = f_g * 10.0;
			}
			//gl_h = gl_h / ( ( 16.0 / 9.0 ) / ( 4.0 / 3.0 ) );
			gl_h = gl_h / ( ( 2.35 / 1.0 ) / ( 4.0 / 3.0 ) );
			gl_x = ( 10.0 - gl_w ) / 2;
			gl_y = ( 10.0 - gl_h ) / 2;

		}
		
		glBegin (GL_QUADS);

		glColor4f( 0.0f, 0.0f, 0.0f, 1.0f );

		glVertex3f( 0.0f, 0.0f, 0.0f );
		glVertex3f( gl_x + gl_w, 0.0f, 0.0f );
		glVertex3f( gl_x + gl_w, gl_y, 0.0f );
		glVertex3f( 0.0f, gl_y, 0.0f );
		
		glVertex3f( 0.0f, gl_y + gl_h, 0.0f );
		glVertex3f( gl_x + gl_w, gl_y + gl_h, 0.0f );
		glVertex3f( gl_x + gl_w, 10.0f, 0.0f );
		glVertex3f( 0.0f, 10.0f, 0.0f );


		glEnd();
	}

	glLineWidth(3);
	glBegin (GL_LINE_LOOP);
	glColor4f( 0.0f, 0.0f, 0.0f, 1.0f );
	
	glVertex3f   (  gl_x,      gl_y, 0.0 );
	glVertex3f   ( gl_x + gl_w,      gl_y, 0.0 );
	glVertex3f   ( gl_x + gl_w,     gl_y + gl_h, 0.0 );
	glVertex3f   (  gl_x,     gl_y + gl_h, 0.0 );

	glEnd();
	glLineWidth(1);
	glBegin (GL_LINE_LOOP);
	glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );

	glVertex3f   (  gl_x,      gl_y, 0.0 );
	glVertex3f   ( gl_x + gl_w,      gl_y, 0.0 );
	glVertex3f   ( gl_x + gl_w,     gl_y + gl_h, 0.0 );
	glVertex3f   (  gl_x,     gl_y + gl_h, 0.0 );

	glEnd();

	
	glEnable (GL_TEXTURE_2D);
	
/*
	float _x, _y, _w, _h;
	_x = gl_x + (gl_w / 8.0);
	_y = gl_y;
	_w = gl_w / 1.33333333333333333;
	_h = gl_h;
	glDisable (GL_TEXTURE_2D);
	glLineWidth(3);
	glBegin (GL_LINE_LOOP);
	glColor4f( 0.0f, 0.0f, 0.0f, 1.0f );
	
	glVertex3f   (  _x,      _y, 0.0 );
	glVertex3f   ( _x + _w,      _y, 0.0 );
	glVertex3f   ( _x + _w,     _y + _h, 0.0 );
	glVertex3f   (  _x,     _y + _h, 0.0 );

	glEnd();
	glLineWidth(1);
	glBegin (GL_LINE_LOOP);
	glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );

	glVertex3f   (  _x,      _y, 0.0 );
	glVertex3f   ( _x + _w,      _y, 0.0 );
	glVertex3f   ( _x + _w,     _y + _h, 0.0 );
	glVertex3f   (  _x,     _y + _h, 0.0 );

	glEnd();
	glEnable (GL_TEXTURE_2D);
	*/
}

void VideoViewGL::pushFrameStack( frame_struct** fs, bool move_cursor )
{
	if ( move_cursor ) {
		SwitchBoard::i()->move_cursor();
	}
	make_current();
	if ( !valid() ) {
		glLoadIdentity(); glViewport( 0, 0, w(), h() ); // glViewport( _x, _y, _w, _h );
		glOrtho( 0, 10, 10, 0, -20000, 10000 ); glEnable( GL_BLEND );
		glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
		glEnable (GL_TEXTURE_2D);
	}

	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glBindTexture( GL_TEXTURE_2D, video_canvas[0] );

	if ( !fs[0] ) {
		static unsigned char p[3 * T_W * T_H] = { 0 };
		glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, T_W, T_H, GL_RGB, GL_UNSIGNED_BYTE, p );
		swap_buffers();
		return;
	}
	
	int count = 0;	
	for ( int i = 1; fs[i]; i++ ) {
		count++;
	}
	//TODO: i < 10
	for ( int i = count; i>=0; i-- ) {
		glBindTexture( GL_TEXTURE_2D, video_canvas[i] );
		if ( fs[i]->cacheable ) {
			if ( tcache[i].p == fs[i] ) {
				continue;
			} else {
				tcache[i].p = fs[i];
			}
		}
		if ( fs[i]->has_alpha_channel ) {
			glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, fs[i]->w, fs[i]->h, GL_RGBA, GL_UNSIGNED_BYTE, fs[i]->RGB );
		} else {
			// Pulldown large Image
/*			if ( fs[i]->w > T_W || fs[i]->h > T_H  ) {
				halve_image( pulldown_frame, fs[i]->RGB, fs[i]->w, fs[i]->h );
				glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, fs[i]->w >> 1, fs[i]->h >> 1, GL_RGB, GL_UNSIGNED_BYTE, pulldown_frame );
			} else {*/
				glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, fs[i]->w, fs[i]->h, GL_RGB, GL_UNSIGNED_BYTE, fs[i]->RGB );
/*			}*/
		}
	}
	for ( int i = count; i>=0; i-- ) {
		glBindTexture( GL_TEXTURE_2D, video_canvas[i] );
		float gl_x, gl_y, gl_w, gl_h;
		{
			if ( fs[i]->render_strategy == RENDER_FIT ) {
				float f_v = fs[i]->aspect;
				float _4_3 = g_16_9 ? ( 16.0 / 9.0 ) : ( 4.0 / 3.0 );
				float _4_3_w, _4_3_h, _4_3_x, _4_3_y;
				float f_w = ( (float)w() / (float)h() );
				{
					float f_g = _4_3 / f_w;
					if ( f_g > 1.0 ) {
						_4_3_h = 10.0 / f_g;
						_4_3_w = 10.0;
					} else {
						_4_3_h = 10.0;
						_4_3_w = f_g * 10.0;
					}
					_4_3_x = ( 10.0 - _4_3_w ) / 2;
					_4_3_y = ( 10.0 - _4_3_h ) / 2;

				}
				if ( f_v > _4_3 ) { //Banner Format
					gl_w = _4_3_w;
					gl_h = _4_3_w / (f_v / f_w);
					gl_x = _4_3_x;
					gl_y = ( 10.0 - gl_h ) / 2;
				} else { //Skyscraper Format
					gl_h = _4_3_h;
					gl_w = _4_3_h * (f_v / f_w);
					gl_y = _4_3_y;
					gl_x = ( 10.0 - gl_w ) / 2;
				}
			} else if ( fs[i]->render_strategy == RENDER_CROP ) {
				float f_v = fs[i]->aspect;
				float _4_3 = g_16_9 ? ( 16.0 / 9.0 ) : ( 4.0 / 3.0 );
				float _4_3_w, _4_3_h, _4_3_x, _4_3_y;
				float f_w = ( (float)w() / (float)h() );
				{
					float f_g = _4_3 / f_w;
					if ( f_g > 1.0 ) {
						_4_3_h = 10.0 / f_g;
						_4_3_w = 10.0;
					} else {
						_4_3_h = 10.0;
						_4_3_w = f_g * 10.0;
					}
					_4_3_x = ( 10.0 - _4_3_w ) / 2;
					_4_3_y = ( 10.0 - _4_3_h ) / 2;

				}
				if ( f_v > _4_3 ) { //Banner Format
					gl_h = _4_3_h;
					gl_w = _4_3_h * (f_v / f_w);
					gl_y = _4_3_y;
					gl_x = ( 10.0 - gl_w ) / 2;
				} else { //Skyscraper Format
					gl_w = _4_3_w;
					gl_h = _4_3_w / (f_v / f_w);
					gl_x = _4_3_x;
					gl_y = ( 10.0 - gl_h ) / 2;
				}
			} else {
				float f_v;// = ( (float)fs[i]->w / (float)fs[i]->h );
				if ( g_16_9 ) {
					f_v = ( 16.0 / 9.0 );
				} else {
					f_v = ( 4.0 / 3.0 );
				}
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

		}
		
		if ( fs[i]->render_strategy == RENDER_FIT && !fs[i]->has_alpha_channel ) {
			int w_ = g_16_9 ? 1024 : 768;
			int h_ = 576;
			float gl_x, gl_y, gl_w, gl_h;
			{
				float f_v = ( (float)w_ / (float)h_ );
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
			glDisable (GL_TEXTURE_2D);
			glBegin (GL_QUADS);
			glColor4f( 0.0f, 0.0f, 0.0f, fs[i]->alpha );

			glVertex3f   (  gl_x,      gl_y, 0.0 );
			glVertex3f   ( gl_x + gl_w,      gl_y, 0.0 );
			glVertex3f   ( gl_x + gl_w,     gl_y + gl_h, 0.0 );
			glVertex3f   (  gl_x,     gl_y + gl_h, 0.0 );

			glEnd();
			glEnable (GL_TEXTURE_2D);
		}

		float ww = ( fs[i]->w - 2 * fs[i]->analog_blank ) / TEXTURE_WIDTH;
		float xx = ( fs[i]->analog_blank ) / TEXTURE_WIDTH;
		float hh = fs[i]->h / TEXTURE_HEIGHT;
		glColor4f( 1.0f, 1.0f, 1.0f, fs[i]->alpha ); //Control Transparency
		glBegin (GL_QUADS);
			glTexCoord2f (  xx,      0.0 );
			glVertex3f   (  gl_x,      gl_y, 0.0 );
			glTexCoord2f (  xx + ww,  0.0 );  // (fs->w / 512.0)
			glVertex3f   ( gl_x + gl_w,      gl_y, 0.0 );
			glTexCoord2f (  xx + ww,  hh ); // (368.0 / 512.0) (240.0 / 512.0)
			glVertex3f   ( gl_x + gl_w,     gl_y + gl_h, 0.0 );
			glTexCoord2f (  xx,      hh ); // (fs->h / 512.0)
			glVertex3f   (  gl_x,     gl_y + gl_h, 0.0 );
		glEnd ();
	}
	drawVideoBorder();
/*	gl_font(FL_TIMES|FL_BOLD, 80);
	glRasterPos3f( 2, 2, 0 );
	gl_draw( "HELLO", strlen( "HELLO" ) );*/

	swap_buffers();

}

void VideoViewGL::pushFrame( frame_struct* fs, bool move_cursor )
{
	frame_struct* fstack[2] = {0};
	fstack[0] = fs;
	pushFrameStack( fstack, move_cursor );
}
/*
 *  GL_NVX_ycrcb
 */
void VideoViewGL::draw()
{
	if (g_backseek) { reset_cache(); g_backseek=false; }
	if ( !valid() ) {
		float glo1, glo2;
		if ( m_zoom < 0.0 ) {
			glo1 = 10.0 * m_zoom;
			glo2 = 10.0 - 10.0 * m_zoom;
			/*
			0.0:
				0.0;
				10.0;
			-1.0:
				-10.0
				20.0;
			*/
		} else {
			glo1 = 0.0 + m_zoom * 5.0; // -> 5.0
			glo2 = 10.0 - m_zoom * 5.0; // -> 5.0
		}
		glLoadIdentity(); glViewport( 0, 0, w(), h() ); // glViewport( _x, _y, _w, _h );
		glOrtho( glo1, glo2, glo2, glo1, -20000, 10000 ); glEnable( GL_BLEND );
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
			glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA, T_W, T_H, 0, GL_RGB, GL_UNSIGNED_BYTE, p);
		}
		once = false;
	}
	if ( g_playbackCore->active() ) { return; }
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glBindTexture( GL_TEXTURE_2D, video_canvas[0] );
	if ( m_seekPosition >= 0 ) {
		g_SEEKING = true;
		frame_struct** fs = g_timeline->getFrameStack( m_seekPosition );
		g_SEEKING = false;
		if ( !fs[0] ) {
			g_PREVENT_OFFSCREEN_CRASH = false;
			return;
		}
		int count = 0;	
		for ( int i = 1; fs[i]; i++ ) {
			count++;
		}

		for ( int i = count; i>=0; i-- ) {
			if ( fs[i]->has_alpha_channel ) {
				glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, fs[i]->w, fs[i]->h, GL_RGBA, GL_UNSIGNED_BYTE, fs[i]->RGB );
			} else {
/*				if ( fs[i]->w > 512 || fs[i]->h > 512  ) {
					halve_image( pulldown_frame, fs[i]->RGB, fs[i]->w, fs[i]->h );
					//glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, fs[i]->w, fs[i]->h, GL_RGB, GL_UNSIGNED_BYTE, pulldown_frame );
					glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, fs[i]->w >> 1, fs[i]->h >> 1, GL_RGB, GL_UNSIGNED_BYTE, pulldown_frame );
				} else {*/
					glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, fs[i]->w, fs[i]->h, GL_RGB, GL_UNSIGNED_BYTE, fs[i]->RGB );
				//}

				//glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, fs[i]->w, fs[i]->h, GL_RGB, GL_UNSIGNED_BYTE, fs[i]->RGB );
			}

			float glw = 10.0;
			float glh = 10.0;
			float ww = w();
			float wh = h();
			float aspect_frame = ( 4.0 / 3.0 );
			float aspect_window = ww / wh;
			float aspect_image = fs[i]->aspect;

			float tw = fs[i]->w / TEXTURE_WIDTH;
			float th = fs[i]->h / TEXTURE_HEIGHT;

			float fx, fy, fw, fh;

			if ( (aspect_frame/aspect_window) > 1.0 ) {
				fw = glw;
				fh = glh / (aspect_frame/aspect_window);
				fx = 0.0;
				fy = (glh - fh) / 2.0;
			} else {
				fw = glw * (aspect_frame/aspect_window);
				fh = glh;
				fx = (glw - fw) / 2.0;
				fy = 0.0;
			}
			
			
			float ix = 0.0;
			float iy = 0.0;
			float iw = 1.0;
			float ih = 1.0;
			float ct = (float)fs[i]->crop_top / (float)INT_FLOAT_SCALE;
			float cb = (float)fs[i]->crop_bottom / (float)INT_FLOAT_SCALE;
			float cl = (float)fs[i]->crop_left / (float)INT_FLOAT_SCALE;
			float cr = (float)fs[i]->crop_right / (float)INT_FLOAT_SCALE;

			ix += cl;
			iy += ct;
			iw -= (cl + cr);
			ih -= (ct + cb);

			float tiltx = (float)fs[i]->tilt_x / (float)INT_FLOAT_SCALE;
			float tilty = (float)fs[i]->tilt_y / (float)INT_FLOAT_SCALE;

			float scalex = (float)fs[i]->scale_x / (float)INT_FLOAT_SCALE;
			float scaley = (float)fs[i]->scale_y / (float)INT_FLOAT_SCALE;

			float tiltx_modifier = 1.0;
			float tilty_modifier = 1.0;
			if ( scalex < 0.0 ) {
				scalex += 1.0; // 0.0 -> 1.0
			} else {
				scalex = 1.0 + scalex;
				tiltx_modifier *= scalex;
			}
			if ( scaley < 0.0 ) {
				scaley += 1.0; // 0.0 -> 1.0
			} else {
				scaley = 1.0 + scaley;
				tilty_modifier *= scaley;
			}

			
			if ( fs[i]->render_strategy == RENDER_FIT ) {
				if ( aspect_image > aspect_frame ) { //Banner
					fy += ( fh - (fw / ( aspect_image / aspect_window )) ) / 2.0;
					tilty_modifier *= fh / (fw / ( aspect_image / aspect_window ));
					fh = fw / ( aspect_image / aspect_window );
				} else { //Skyscraper
					fx += ( fw - (fh * ( aspect_image / aspect_window )) ) / 2.0;
					tiltx_modifier *= fw / (fh * ( aspect_image / aspect_window ));
					fw = fh * ( aspect_image / aspect_window );
				}
			}
			float vertex_x = fx + ix * fw * scalex + tiltx * fw * tiltx_modifier;
			float vertex_y = fy + iy * fh * scaley + tilty * fh * tilty_modifier;
			float vertex_w = fw * iw * scalex;
			float vertex_h = fh * ih * scaley;

			float border_x = fx + tiltx * fw * tiltx_modifier;
			float border_y = fy + tilty * fh * tilty_modifier;
			float border_w = fw * scalex;
			float border_h = fh * scaley;

			float tex_x = cl * tw;
			float tex_y = ct * th;
			float tex_w = iw * tw;
			float tex_h = ih * th;

			glColor4f( 1.0f, 1.0f, 1.0f, fs[i]->alpha );
			
			glBegin (GL_QUADS);
				glTexCoord2f ( tex_x, tex_y );
				glVertex3f   ( vertex_x, vertex_y, 0.0 );
				glTexCoord2f ( tex_x + tex_w, tex_y );
				glVertex3f   ( vertex_x + vertex_w, vertex_y, 0.0 );
				glTexCoord2f ( tex_x + tex_w, tex_y + tex_h );
				glVertex3f   ( vertex_x + vertex_w, vertex_y + vertex_h, 0.0 );
				glTexCoord2f ( tex_x, tex_y + tex_h );
				glVertex3f   ( vertex_x, vertex_y + vertex_h, 0.0 );
			glEnd ();

			glDisable (GL_TEXTURE_2D);
			glLineWidth(3);
			glBegin (GL_LINE_LOOP);
			glColor4f( 1.0f, 0.0f, 0.0f, 1.0f );

				glVertex3f ( border_x, border_y, 0.0 );
				glVertex3f ( border_x + border_w, border_y, 0.0 );
				glVertex3f ( border_x + border_w, border_y + border_h, 0.0 );
				glVertex3f ( border_x, border_y + border_h, 0.0 );

			glEnd();
			glEnable (GL_TEXTURE_2D);
#if 0
			
			float gl_x, gl_y, gl_w, gl_h;
			{
				if ( fs[i]->render_strategy == RENDER_FIT ) {
					
					float f_v = fs[i]->aspect;
					float _4_3 = g_16_9 ? ( 16.0 / 9.0 ) : ( 4.0 / 3.0 );
					float _4_3_w, _4_3_h, _4_3_x, _4_3_y;
					float f_w = ( (float)w() / (float)h() );
					{
						float f_g = _4_3 / f_w;
						if ( f_g > 1.0 ) {
							_4_3_h = 10.0 / f_g;
							_4_3_w = 10.0;
						} else {
							_4_3_h = 10.0;
							_4_3_w = f_g * 10.0;
						}
						_4_3_x = ( 10.0 - _4_3_w ) / 2;
						_4_3_y = ( 10.0 - _4_3_h ) / 2;

					}
					if ( f_v > _4_3 ) { //Banner Format
						gl_w = _4_3_w;
						gl_h = _4_3_w / (f_v / f_w);
						gl_x = _4_3_x;
						gl_y = ( 10.0 - gl_h ) / 2;
					} else { //Skyscraper Format
						gl_h = _4_3_h;
						gl_w = _4_3_h * (f_v / f_w);
						gl_y = _4_3_y;
						gl_x = ( 10.0 - gl_w ) / 2;
					}
				} else if ( fs[i]->render_strategy == RENDER_CROP ) {
					float f_v = fs[i]->aspect;
					float _4_3 = g_16_9 ? ( 16.0 / 9.0 ) : ( 4.0 / 3.0 );
					float _4_3_w, _4_3_h, _4_3_x, _4_3_y;
					float f_w = ( (float)w() / (float)h() );
					{
						float f_g = _4_3 / f_w;
						if ( f_g > 1.0 ) {
							_4_3_h = 10.0 / f_g;
							_4_3_w = 10.0;
						} else {
							_4_3_h = 10.0;
							_4_3_w = f_g * 10.0;
						}
						_4_3_x = ( 10.0 - _4_3_w ) / 2;
						_4_3_y = ( 10.0 - _4_3_h ) / 2;

					}
					if ( f_v > _4_3 ) { //Banner Format
						gl_h = _4_3_h;
						gl_w = _4_3_h * (f_v / f_w);
						gl_y = _4_3_y;
						gl_x = ( 10.0 - gl_w ) / 2;
					} else { //Skyscraper Format
						gl_w = _4_3_w;
						gl_h = _4_3_w / (f_v / f_w);
						gl_x = _4_3_x;
						gl_y = ( 10.0 - gl_h ) / 2;
					}
				} else {
					float f_v;// = ( (float)fs[i]->w / (float)fs[i]->h );
					if ( g_16_9 ) {
						f_v = ( 16.0 / 9.0 );
					} else {
						f_v = ( 4.0 / 3.0 );
					}
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
			}

			if ( fs[i]->render_strategy == RENDER_FIT && !fs[i]->has_alpha_channel ) {
				int w_ = g_16_9 ? 1024 : 768;
				int h_ = 576;
				float gl_x, gl_y, gl_w, gl_h;
				{
					float f_v = ( (float)w_ / (float)h_ );
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
				glDisable (GL_TEXTURE_2D);
				glBegin (GL_QUADS);
				glColor4f( 0.0f, 0.0f, 0.0f, fs[i]->alpha );

				glVertex3f   (  gl_x,      gl_y, 0.0 );
				glVertex3f   ( gl_x + gl_w,      gl_y, 0.0 );
				glVertex3f   ( gl_x + gl_w,     gl_y + gl_h, 0.0 );
				glVertex3f   (  gl_x,     gl_y + gl_h, 0.0 );

				glEnd();
				glEnable (GL_TEXTURE_2D);
			}
			
			glColor4f( 1.0f, 1.0f, 1.0f, fs[i]->alpha ); //Control Transparency
			float ww = ( fs[i]->w - 2 * fs[i]->analog_blank ) / TEXTURE_WIDTH;
			float xx = ( fs[i]->analog_blank ) / TEXTURE_WIDTH;
			float hh = fs[i]->h / TEXTURE_HEIGHT;
					glBegin (GL_QUADS);
				glTexCoord2f (  xx,      0.0 );
				glVertex3f   (  gl_x + fs[i]->tilt_x,      gl_y + fs[i]->tilt_y, 0.0 );
				glTexCoord2f (  xx + ww,  0.0 );  // (fs->w / 512.0)
				glVertex3f   ( gl_x + gl_w + fs[i]->tilt_x,      gl_y + fs[i]->tilt_y, 0.0 );
				glTexCoord2f (  xx + ww,  hh ); // (368.0 / 512.0) (240.0 / 512.0)
				glVertex3f   ( gl_x + gl_w + fs[i]->tilt_x,     gl_y + gl_h + fs[i]->tilt_y, 0.0 );
				glTexCoord2f (  xx,      hh ); // (fs->h / 512.0)
				glVertex3f   (  gl_x + fs[i]->tilt_x,     gl_y + gl_h + fs[i]->tilt_y, 0.0 );
			glEnd ();

#endif
		}
	}
	g_PREVENT_OFFSCREEN_CRASH = false;
/*	drawVideoBorder( 720, 576  ); //Pixel sind nicht quadratisch?
	drawVideoBorder( 768, 576  );
	drawVideoBorder( 1024, 576  );*/
	drawVideoBorder();
}

void VideoViewGL::seek( int64_t position )
{
	m_seekPosition = position;
	redraw();
}

void VideoViewGL::play()
{
	if ( g_playbackCore->active() ) {
		return;
	}
	reset_cache();
	m_seekPosition = -1;
	g_timeline->sort();
	g_playbackCore->play();
}

void VideoViewGL::stop()
{
	g_playbackCore->stop();
}

void VideoViewGL::pause()
{
	g_playbackCore->pause();
}

} /* namespace nle */
