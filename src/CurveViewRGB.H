#ifndef _CURVE_VIEW_RGB_H_
#define _CURVE_VIEW_RGB_H_

#include <FL/Fl.H>
#include <FL/Fl_Widget.H>
#include <stdint.h>

#define ROUND(x) ((int32_t)((x)+0.5))

class CurveViewRGB : public Fl_Widget
{
	public:
		CurveViewRGB( int x, int y, int w, int h, const char* label = 0 );
		~CurveViewRGB();
		void draw();
		unsigned char m_values_r[256];
		unsigned char m_values_g[256];
		unsigned char m_values_b[256];
		void contrast( float r, float g, float b ); /* -256 -> 256 */
		void brightness( float r, float g, float b ); /* -256 -> 256 */
		void gamma( float r, float g, float b ); /* 0 -> 1 */
	private:
		void convert_from_display( int x, int y, int& out_x, int& out_y );
		void convert_to_display( int x, int y, int& out_x, int& out_y );
};

#endif /* _CURVE_VIEW_RGB_H_ */


