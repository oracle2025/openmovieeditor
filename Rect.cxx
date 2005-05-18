#include "Rect.H"

namespace nle
{
	
Rect::Rect( int _x, int _y, int _w, int _h )
	: x(_x), y(_y), w(_w), h(_h)
{
}
bool Rect::inside( int _x, int _y )
{
	return _x >= x && _x <= x + w && _y >= y && _y <= y + h;
}

} /* namespace nle */
