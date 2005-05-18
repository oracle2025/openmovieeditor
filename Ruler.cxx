#include <FL/fl_draw.H>


#include "Ruler.H"


namespace nle
{

Ruler::Ruler( int x, int y, int w, int h, const char *label )
	: Fl_Widget( x, y, w, h, label )
{
}

void Ruler::draw()
{
	fl_draw_box( FL_UP_BOX, x(), y(), w(), h(), FL_BACKGROUND_COLOR );
}


} /* namespace nle */

