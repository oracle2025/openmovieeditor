/*  VideoViewGL.H
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

#ifndef _VIDEO_VIEW_GL_H_
#define _VIDEO_VIEW_GL_H_

#include <list>

#include <FL/Fl.H>
#include <FL/Fl_Gl_Window.H>

#include "NextFrameListener.H"
#include "frame_struct.h"

namespace nle
{
class Timeline;
class Sound;

class VideoViewGL : public Fl_Gl_Window, NextFrameListener
{
	public:
		VideoViewGL( int x, int y, int w, int h, const char *l = 0 );
		~VideoViewGL();
		void draw();
		void nextFrame( int64_t frame );
		void seek( int64_t position );
		void play();
		void stop();
	private:
		//Timeline *m_timeline; //g_timeline
		Sound *m_snd;
		bool m_playing;
		bool m_seek;
		int64_t m_seekPosition;
		//std::list< frame_struct* > m_frameList;
};

} /* namespace nle */


#endif /* _VIDEO_VIEW_GL_H_ */
