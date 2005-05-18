#ifndef _RULER_H_
#define _RULER_H_

#include <FL/Fl_Widget.H>

namespace nle
{

class Ruler : public Fl_Widget
{
	public:
		Ruler( int x, int y, int w, int h, const char *label = 0 );
	protected:
		void draw();
	private:
};


} /* namespace nle */
#endif /* _RULER_H_ */



