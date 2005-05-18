#ifndef _RULER_H_
#define _RULER_H_

#include <FL/Fl_Widget.H>

#include "Rect.H"

namespace nle
{

class Ruler : public Fl_Widget
{
	public:
		Ruler( int x, int y, int w, int h, const char *label = 0 );
		int handle( int event );
	protected:
		void draw();
	private:
		Rect m_stylus;
};


} /* namespace nle */
#endif /* _RULER_H_ */



