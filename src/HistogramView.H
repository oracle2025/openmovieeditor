/*  HistogramView.H
 *
 *  Copyright (C) 2007 Richard Spindler <richard.spindler AT gmail.com>
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

#ifndef _HISTOGRAM_VIEW_H_
#define _HISTOGRAM_VIEW_H_

#include <stdint.h>

#include <FL/Fl.H>
#include <FL/Fl_Widget.H>


namespace nle
{

class HistogramView;
extern HistogramView* g_histogram;

class VideoEffectClip;

class HistogramView : public Fl_Widget
{
	public:
		HistogramView( int x, int y, int w, int h, const char* label = 0 );
		~HistogramView();
		void setVideoClip( VideoEffectClip* clip, int64_t position );
		void draw();
	private:
		VideoEffectClip* m_clip;
		int64_t m_position;
		int m_r[256];
		int m_g[256];
		int m_b[256];
		int m_max;
};

} /* namespace nle */

#endif /* _HISTOGRAM_VIEW_H_ */
