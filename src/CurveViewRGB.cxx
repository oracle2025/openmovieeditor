#include <cstdlib>
#include <cmath>
#include <FL/fl_draw.H>
#include <FL/Fl_Window.H>
#include "CurveViewRGB.H"

unsigned char CLAMP0255(int32_t a)
{
  return (unsigned char)
    ( (((-a) >> 31) & a)  // 0 if the number was negative
      | (255 - a) >> 31); // -1 if the number was greater than 255
}

CurveViewRGB::CurveViewRGB( int x, int y, int w, int h, const char* label )
	: Fl_Widget( x, y, w, h, label )
{
}

CurveViewRGB::~CurveViewRGB()
{
}

void CurveViewRGB::draw()
{
	fl_draw_box( FL_DOWN_BOX, x(), y(), w(), h(), FL_BACKGROUND_COLOR );
	fl_push_clip( x() + 2, y() + 2,  w() - 4, h() - 4 );
	fl_push_matrix();
	fl_translate( x() + 2, y() + 2 );
	fl_scale( ( w() - 4 ) / 256.0, ( h() - 4 ) / 256.0 );
	
	fl_line_style( FL_SOLID, 2 );

	fl_color( FL_RED );
	fl_begin_line();
	for ( int i = 0; i < 256; i++ ) {
		fl_vertex( i, 256 - m_values_r[i] );
	}
	fl_end_line();
	fl_color( FL_GREEN );
	fl_begin_line();
	for ( int i = 0; i < 256; i++ ) {
		fl_vertex( i, 256 - m_values_g[i] );
	}
	fl_end_line();
	fl_color( FL_BLUE );
	fl_begin_line();
	for ( int i = 0; i < 256; i++ ) {
		fl_vertex( i, 256 - m_values_b[i] );
	}
	fl_end_line();
	fl_line_style( FL_SOLID, 1 );

	
	fl_pop_matrix();
	
	fl_pop_clip();


}


void CurveViewRGB::convert_from_display( int x, int y, int& out_x, int& out_y )
{
	out_x = x * 256 / w();
	out_y = y * 256 / h();
}
void CurveViewRGB::convert_to_display( int x, int y, int& out_x, int& out_y )
{
	out_x = x * w() / 256;
	out_y = y * h() / 256;
}

void CurveViewRGB::contrast( float r, float g, float b )
{
	int i;
	unsigned char *lut = m_values_r;
	int contrast = (int)r;
	for (i=0; i<128; ++i)
		lut[i] = CLAMP0255(i - (((128 - i)*contrast)>>8));
	for (i=128; i<256; ++i)
		lut[i] = CLAMP0255(i + (((i - 128)*contrast)>>8));
	lut = m_values_g;
	contrast = (int)g;
	for (i=0; i<128; ++i)
		lut[i] = CLAMP0255(i - (((128 - i)*contrast)>>8));
	for (i=128; i<256; ++i)
		lut[i] = CLAMP0255(i + (((i - 128)*contrast)>>8));
	lut = m_values_b;
	contrast = (int)b;
	for (i=0; i<128; ++i)
		lut[i] = CLAMP0255(i - (((128 - i)*contrast)>>8));
	for (i=128; i<256; ++i)
		lut[i] = CLAMP0255(i + (((i - 128)*contrast)>>8));
	redraw();
}
void CurveViewRGB::brightness( float r, float g, float b )
{
	int i;
	unsigned char *lut = m_values_r;
	int brightness = (int)r;
	if (brightness < 0) {
		for (i=0; i<256; ++i)
			lut[i] = CLAMP0255((i * (256 + brightness))>>8);
	} else {
		for (i=0; i<256; ++i)
			lut[i] = CLAMP0255(i + (((256 - i) * brightness)>>8));
	}
	lut = m_values_g;
	brightness = (int)g;
	if (brightness < 0) {
		for (i=0; i<256; ++i)
			lut[i] = CLAMP0255((i * (256 + brightness))>>8);
	} else {
		for (i=0; i<256; ++i)
			lut[i] = CLAMP0255(i + (((256 - i) * brightness)>>8));
	}
	lut = m_values_b;
	brightness = (int)b;
	if (brightness < 0) {
		for (i=0; i<256; ++i)
			lut[i] = CLAMP0255((i * (256 + brightness))>>8);
	} else {
		for (i=0; i<256; ++i)
			lut[i] = CLAMP0255(i + (((256 - i) * brightness)>>8));
	}

	redraw();
}
#define MAX_GAMMA 4.0
void CurveViewRGB::gamma( float r, float g, float b )
{
	int i;
	unsigned char *lut = m_values_r;
	double inv_gamma = 1.0 / (r * MAX_GAMMA);
	lut[0] = 0;
	for (i=1; i<256; ++i)
		lut[i] = CLAMP0255( ROUND(255.0 * pow( (double)i / 255.0, inv_gamma ) ) );
	lut = m_values_g;
	inv_gamma = 1.0 / (g * MAX_GAMMA);
	lut[0] = 0;
	for (i=1; i<256; ++i)
		lut[i] = CLAMP0255( ROUND(255.0 * pow( (double)i / 255.0, inv_gamma ) ) );
	lut = m_values_b;
	inv_gamma = 1.0 / (b * MAX_GAMMA);
	lut[0] = 0;
	for (i=1; i<256; ++i)
		lut[i] = CLAMP0255( ROUND(255.0 * pow( (double)i / 255.0, inv_gamma ) ) );
	redraw();
}	
