#include <FL/Fl.h>
#include <FL/fl_draw.H>


#include "Ruler.H"
#include "globals.H"
#include "TimelineView.H"


namespace nle
{

Ruler::Ruler( int x, int y, int w, int h, const char *label )
	: Fl_Widget( x, y, w, h, label )
{
	m_stylus.x = 10;
	m_stylus.y = 0;
	m_stylus.w = 25;
	m_stylus.h = 25;
}

void Ruler::draw()
{
	fl_draw_box( FL_UP_BOX, x(), y(), w(), h(), FL_BACKGROUND_COLOR );
	fl_draw_box( FL_DIAMOND_UP_BOX, x() + m_stylus.x, y() + m_stylus.y, m_stylus.w, m_stylus.h, FL_BACKGROUND_COLOR );
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
			m_stylus.x = _x - __x;
			{
				m_stylus.x = m_stylus.x < 0 ? 0 : m_stylus.x;
				m_stylus.x = m_stylus.x + m_stylus.w > w() - x() ? w() - x() - m_stylus.w : m_stylus.x;
			}
//			g_timelineView->move_cursor( m_stylus.x + ( m_stylus.w / 2 ) - TRACK_SPACING );
			g_timelineView->stylus( x() + m_stylus.x + ( m_stylus.w / 2 ) ); 
			redraw();
			return 1;
		case FL_RELEASE:
			if ( __x ) {
				__x = 0;
				return 1;
			}
			break;
	}
	return Fl_Widget::handle( event );
}

} /* namespace nle */

