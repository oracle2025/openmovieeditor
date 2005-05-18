#include <FL/Fl.H>
#include <FL/fl_draw.H>
#include "Draw.H"

namespace nle
{

void Draw::box( float x, float y, float w, float h, Fl_Color c )
{
//	fl_draw_box( FL_UP_BOX, g_xOrigin + x, g_yOrigin + y, w, h, c );
	fl_color(c);
	fl_begin_polygon();
	fl_vertex( x, y );
	fl_vertex( x, y + h);
	fl_vertex( x + w, y + h );
	fl_vertex( x + w, y );
	fl_end_polygon();
	fl_color(FL_BLACK);
	fl_begin_loop();
	fl_vertex( x, y );
	fl_vertex( x, y + h);
	fl_vertex( x + w, y + h );
	fl_vertex( x + w, y );
	fl_end_loop();

}
extern bool USING_AUDIO;
void Draw::triangle( float x, float y, bool direction )
{
	float _x;
	if (USING_AUDIO) {
		_x = direction ? 8 * ( 48000 / 29.97 ) : - 8 * ( 48000 / 29.97 );
	} else {
		_x = direction ? 8 : -8;
	}
	fl_color( FL_BLACK );
	fl_begin_polygon();
	fl_vertex( x, y );
	fl_vertex( x + _x, y - 5 );
	fl_vertex( x + _x, y + 5 );
	fl_end_polygon();
	
}


} /* namespace nle */
