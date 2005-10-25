/*  VideoViewFL.cxx
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

#include <FL/Fl.H>
#include <FL/Fl_Widget.H>
#include <FL/Fl_Window.H>
#include <FL/fl_draw.H>

#include "VideoView.H"
#include "SwitchBoard.H"
#include "Timeline.H"
#include "Sound.H"

using namespace std;

/*Use glTexSubImage2D to update Movie Texture in OpenGL*/

Fl_Window *vv_cb_window;
bool vv_cv_play;
void vv_callback(void*)
{
	if (!vv_cv_play)
		return;
	nle::SwitchBoard::i()->move_cursor();
	Fl::repeat_timeout(0.01, vv_callback);
}

namespace nle
{

VideoView::VideoView( int x, int y, int w, int h, const char *label )
	: Fl_Widget( x, y, w, h, label )
{
	m_timeline = SwitchBoard::i()->timeline();
	vv_cv_play = false;
	vv_cb_window = this->window();
	m_snd = new Sound( this );
}
VideoView::~VideoView()
{
	delete m_snd;
	m_snd = NULL;
}
void VideoView::draw()
{
	fl_draw_box( FL_FLAT_BOX, x(), y(), w(), h(), FL_BACKGROUND_COLOR );
	int width = 368;
	int height = 240;
	if (!m_timeline)
		return;
	unsigned char* image = m_timeline->nextFrame();
	if (image && vv_cv_play) {
		fl_draw_image( (uchar*)image, x(), y(), width, height );
	} else {
		fl_draw_box( FL_FLAT_BOX, x(), y(), width, height, FL_GREEN );
	}
}
void VideoView::nextFrame( int64_t frame )
{
	Fl::lock();
	vv_cb_window->redraw();
	Fl::awake();
	Fl::unlock();
	//cout << "Hello" << endl;
}
void VideoView::play()
{
	m_timeline = SwitchBoard::i()->timeline();
	m_timeline->reset();
	vv_cv_play = true;
	m_snd->Play();
	Fl::add_timeout(0.1, vv_callback, NULL);
	//Fl::add_fd( fd, pipe_cb, data )
}
void VideoView::stop()
{
	m_snd->Stop();
	vv_cv_play = false;
}

} /* namespace nle */
