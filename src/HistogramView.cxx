/*  HistogramView.cxx
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

#include <FL/fl_draw.H>

#include "HistogramView.H"
#include "VideoEffectClip.H"

namespace nle
{

HistogramView* g_histogram;

HistogramView::HistogramView( int x, int y, int w, int h, const char* label )
	: Fl_Widget( x, y, w, h, label )
{
	g_histogram = this;
	for ( int i = 0; i < 256; i++ ) {
		m_r[i] = m_g[i] = m_b[i] = 0;
	}
	m_max = 0;
}	
HistogramView::~HistogramView()
{
	g_histogram = 0;
}
void HistogramView::setVideoClip( VideoEffectClip* clip, int64_t position )
{
	m_clip = clip;
	m_position = position;
	//make all m_r, m_g, m_b = 0;
	if ( m_clip ) {
		for ( int i = 0; i < 256; i++ ) {
			m_r[i] = m_g[i] = m_b[i] = 0;
		}
		frame_struct* fs = m_clip->getFrame( m_position );
		int mul;
		if ( fs->has_alpha_channel ) {
			mul = 4;
		} else {
			mul = 3;
		}
		if ( fs ) {
			for ( int x = 0; x < fs->w; x++ ) {
				for ( int y = 0; y < fs->h; y++ ) {
					m_r[fs->RGB[x * mul + y * fs->w * mul]]++;
					m_g[fs->RGB[x * mul + y * fs->w * mul + 1]]++;
					m_b[fs->RGB[x * mul + y * fs->w * mul + 2]]++;
				}
			}
		}
		m_max = 0;
		for ( int i = 0; i < 255; i++ ) {
			m_max = m_r[i] > m_max ? m_r[i] : m_max;
			m_max = m_g[i] > m_max ? m_g[i] : m_max;
			m_max = m_b[i] > m_max ? m_b[i] : m_max;
		}
		redraw();
	}
}
void HistogramView::draw()
{
	float scale = h() / float(m_max);
	fl_draw_box( FL_DOWN_BOX, x(), y(), w(), h(), FL_BACKGROUND_COLOR );
	if ( !m_clip ) {
		return;
	}
	fl_push_clip( x() + 2, y() + 2,  w() - 4, h() - 4 );
	fl_color( FL_RED );
	for ( int i = 0; i < 255; i++ ) {
		fl_line( x() + i, y() + h(), x() + i, y() + h() - (int)(m_r[i] * scale) );
	}
	fl_color( FL_GREEN );
	for ( int i = 0; i < 255; i++ ) {
		fl_line( x() + i, y() + h(), x() + i, y() + h() - (int)(m_g[i] * scale) );
	}
	fl_color( FL_BLUE );
	for ( int i = 0; i < 255; i++ ) {
		fl_line( x() + i, y() + h(), x() + i, y() + h() - (int)(m_b[i] * scale) );
	}
	fl_pop_clip();
}


} /* namespace nle */



