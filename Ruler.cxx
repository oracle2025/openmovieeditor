/*  Ruler.cxx
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


#include <FL/Fl.H>
#include <FL/fl_draw.H>

#include "Ruler.H"
#include "globals.H"
#include "TimelineView.H"
#include "helper.H"


namespace nle
{

Ruler* g_ruler;


Ruler::Ruler( int x, int y, int w, int h, const char *label )
	: Fl_Widget( x, y, w, h, label )
{
	m_stylus.x = LEFT_TRACK_SPACING - 12;
	m_stylus.y = 0;
	m_stylus.w = 25;
	m_stylus.h = 25;
	g_ruler = this;
}

void Ruler::draw()
{
	// SwitchBoard::i()->zoom();
	// g_scrollBar->value();
	// g_fps;
	
	fl_push_clip( x(), y(), w(), h());
	
	fl_draw_box( FL_UP_BOX, x(), y(), w(), h(), FL_BACKGROUND_COLOR );

	fl_color(FL_FOREGROUND_COLOR);
	fl_font( FL_HELVETICA, 11 );
	fl_draw( timestamp_to_string(g_timelineView->get_real_position(200)), x() + 200, y() + 14 );
	
	fl_draw_box( FL_DIAMOND_UP_BOX, x() + m_stylus.x, y() + m_stylus.y, m_stylus.w, m_stylus.h, FL_BACKGROUND_COLOR );
		
	fl_pop_clip();
}

int Ruler::handle( int event )
{
	static int __x = 0;
	int _x = Fl::event_x() - x();
	int _y = Fl::event_y() - y();
	switch ( event ) {
		case FL_PUSH:
			if ( m_stylus.inside( _x, _y ) ) {
				__x = _x - m_stylus.x;
				return 1;
			}
			break;
		case FL_DRAG:
			if ( !__x )
				break;
			{
				long new_x = _x - __x;
				g_timelineView->stylus( x() + new_x + ( m_stylus.w / 2 ) );
				return 1;
			}
		case FL_RELEASE:
			if ( __x ) {
				__x = 0;
				return 1;
			}
			break;
	}
	return Fl_Widget::handle( event );
}
void Ruler::stylus( long stylus_pos )
{
	m_stylus.x = stylus_pos - ( m_stylus.w / 2 );
	redraw(); //FIXME: OpenGL Window is redrawn
	// Maybe it should be somehow draw from within VideoViewGL
}

} /* namespace nle */

