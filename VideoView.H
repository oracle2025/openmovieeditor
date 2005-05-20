/*  VideoView.H
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

#ifndef _VIDEO_VIEW_H_
#define _VIDEO_VIEW_H_

#include "NextFrameListener.H"

namespace nle
{

class Timeline;
class Sound;

class VideoView : public Fl_Widget, NextFrameListener
{
	public:
		VideoView( int x, int y, int w, int h, const char *label = 0 );
		~VideoView();
		void play();
		void stop();
		void draw();
		void nextFrame( int64_t frame );
	private:
		Timeline *m_timeline;
		Sound *m_snd;
};
	
} /* namespace nle */
#endif /* _VIDEO_VIEW_H_ */

