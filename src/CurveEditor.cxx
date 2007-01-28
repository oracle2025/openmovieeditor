/* CurveEditor.cxx
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

#include "CurveEditor.H"

#include <FL/fl_draw.H>

#include "global_includes.H"

namespace nle
{


CurveEditor::CurveEditor( int x, int y, int w, int h, unsigned char* values, const char* label )
	: Fl_Widget( x, y, w, h, label )
{
	m_values = values;
}

CurveEditor::~CurveEditor()
{
}

void CurveEditor::draw()
{
	fl_draw_box( FL_DOWN_BOX, x(), y(), w(), h(), FL_BACKGROUND_COLOR );
	fl_push_clip( x() + 2, y() + 2,  w() - 4, h() - 4 );
	fl_color( FL_BLACK );
	fl_push_matrix();
	fl_translate( x(), y() );
	fl_scale( w() / 256.0, h() / 256.0 );

	fl_begin_line();
	for ( int i = 0; i < 256; i++ ) {
		fl_vertex( i, 256 - m_values[i] );
	}
	fl_end_line();
	fl_pop_matrix();
	
	fl_pop_clip();

}

int CurveEditor::handle( int event )
{
	int x_rel = Fl::event_x() - x();
	int y_rel = Fl::event_y() - y();
	int x_point1;
	int y_point1;
	int x_point2;
	int y_point2;
	static int x_last;
	static int y_last;
	switch ( event ) {
		case FL_PUSH:
			x_last = x_rel;
			y_last = y_rel;
		case FL_RELEASE:
			return 1;
		case FL_DRAG:
			{
				convert_from_display( x_last, y_last, x_point1, y_point1 );
				convert_from_display( x_rel, y_rel, x_point2, y_point2 );
				if ( x_point1 > x_point2 ) {
					int tmp = x_point1;
					x_point1 = x_point2;
					x_point2 = tmp;
					tmp = y_point1;
					y_point1 = y_point2;
					y_point2 = tmp;
				}
				if ( x_point1 == x_point2 ) {
					if ( x_point1 >= 256 ) {
						x_point1 = 255;
					}
					if ( x_point1 < 0 ) {
						x_point1 = 0;
					}
					m_values[x_point1] = y_point1;
					
				} else {
					for ( int x = x_point1; x <= x_point2; x++ ) {
						int y = y_point1 + ( x - x_point1 ) * ( ( y_point2 - y_point1 ) / ( x_point2 - x_point1 ) );
						int x_p = x;
						if ( x_p >= 256 ) {
							x_p = 255;
						}
						if ( x_p < 0 ) {
							x_p = 0;
						}
						m_values[x_p] = y;
					}
				}
				redraw();
				x_last = x_rel;
				y_last = y_rel;
				return 1;
			}
			break;
	}
	return Fl_Widget::handle( event );
}

void CurveEditor::convert_from_display( int x, int y, int& out_x, int& out_y )
{
	out_x = x * 256 / w();
	out_y = 256 - y * 256 / h();
}
void CurveEditor::convert_to_display( int x, int y, int& out_x, int& out_y )
{
	out_x = x * w() / 256;
	out_y = y * h() / 256;
}


} /* namespace nle */
