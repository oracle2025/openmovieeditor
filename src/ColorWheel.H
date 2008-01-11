#ifndef _COLOR_WHEEL_H_
#define _COLOR_WHEEL_H_

#include <FL/Fl.H>
#include <FL/Fl_Widget.H>
#include <FL/x.H>
class Fl_Image;
class ColorWheel : public Fl_Widget
{
	public:
		ColorWheel( int x, int y, int w, int h, const char* label = 0 );
		~ColorWheel();
		void draw();
		void v( float v ) { m_v = v; redraw(); }
		int handle( int event );
		float len() { return m_len; }
		float angle() { return m_angle; }
		Fl_Color color_value() { return m_color; }
		unsigned char m_r;
		unsigned char m_g;
		unsigned char m_b;
	private:
		Fl_Color m_color;
		float m_angle;
		float m_len;
		float m_v;
		float m_x;
		float m_y;
		Fl_Offscreen m_offscreen;
		unsigned char* m_pixels;
		Fl_Image* m_image;
};

#endif /* _COLOR_WHEEL_H_ */
