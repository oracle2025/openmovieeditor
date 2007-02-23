/* TestFilter.cxx
 */

#include <FL/fl_draw.H>
#include "global_includes.H"
#include "TestFilter.H"


namespace nle
{

TestFilter::TestFilter( Clip* /*clip*/ )
{
}

void TestFilter::onDraw( Rect& rect )
{
	fl_draw_box( FL_UP_BOX, rect.x, rect.y, 20, 20, FL_RED );
}
DragHandler* TestFilter::onMouseDown( Rect&, int, int, bool )
{
	cout << "on Mouse Down" << endl;
	return 0;
}


} /* namespace nle */
