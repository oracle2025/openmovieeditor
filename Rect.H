#ifndef _RECT_H_
#define _RECT_H_


namespace nle
{

class Rect
{
	public:
		Rect( int _x, int _y, int _w, int _h );
		Rect() {}
		int x, y, w, h;
		bool inside( int _x, int _y );
		
};

} /* namespace nle */

#endif /* _RECT_H_ */

