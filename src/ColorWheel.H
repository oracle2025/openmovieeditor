#ifndef _COLOR_WHEEL_H_
#define _COLOR_WHEEL_H_

#include <FL/Fl.H>
#include <FL/Fl_Widget.H>


class ColorWheel : public Fl_Widget
{
	public:
		ColorWheel( int x, int y, int w, int h, const char* label = 0 );
		~ColorWheel();
		void draw();
		void v( float v ) { m_v = v; redraw(); }
	private:
		float m_v;
};

#endif /* _COLOR_WHEEL_H_ */
