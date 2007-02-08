#include <cstdlib>
#include <math.h>
#include <FL/fl_draw.H>
#include <FL/Fl_Window.H>
#include "ColorWheel.H"


ColorWheel::ColorWheel( int x, int y, int w, int h, const char* label )
	: Fl_Widget( x, y, w, h, label )
{
	m_v = 1.0;
}

ColorWheel::~ColorWheel()
{
}

void HSVtoRGB( float *r, float *g, float *b, float h, float s, float v )
{
	int i;
	float f, p, q, t;

	if( s == 0 ) {
		// achromatic (grey)
		*r = *g = *b = v;
		return;
	}

	h /= 60;			// sector 0 to 5
	i = lrint(floor( h ));
	f = h - i;			// factorial part of h
	p = v * ( 1 - s );
	q = v * ( 1 - s * f );
	t = v * ( 1 - s * ( 1 - f ) );

	switch( i ) {
		case 0:
			*r = v;
			*g = t;
			*b = p;
			break;
		case 1:
			*r = q;
			*g = v;
			*b = p;
			break;
		case 2:
			*r = p;
			*g = v;
			*b = t;
			break;
		case 3:
			*r = p;
			*g = q;
			*b = v;
			break;
		case 4:
			*r = t;
			*g = p;
			*b = v;
			break;
		default:		// case 5:
			*r = v;
			*g = p;
			*b = q;
			break;
	}

}

void ColorWheel::draw()
{
	float r, g, b;
	fl_draw_box( FL_DOWN_BOX, x(), y(), w(), h(), FL_BACKGROUND_COLOR );
	//fl_push_clip( x() + 2, y() + 2,  w() - 4, h() - 4 );
	fl_color( FL_BLACK );
	fl_push_matrix();
	fl_translate( x() + 2, y() + 2 );
	fl_scale( ( w() - 4 ) / 256.0, ( h() - 4 ) / 256.0 );

	for ( int h = 0; h < 360; h++ ) {
		for ( float s = 1.0; s > 0.0; s -= 0.01 ) {
			HSVtoRGB( &r, &g, &b, (float)h, s, m_v );
			fl_color( (uchar)(r * 255), (uchar)(g * 255), (uchar)(b * 255) );
			fl_begin_polygon();
			//fl_vertex( 128.0, 128.0 );
			fl_arc( 128.0, 128.0, (s-0.05) * 120.0, (float)h+1, (float)h );
			fl_arc( 128.0, 128.0, s * 120.0, (float)h, (float)h+1 );
			fl_end_polygon();
		}
	}
	fl_color( FL_BLACK );
	fl_begin_polygon();
	fl_circle( 128.0, 128.0, 6.0);
	fl_end_polygon();
	fl_color( FL_WHITE );
	fl_begin_polygon();
	fl_circle( 128.0, 128.0, 3.0);
	fl_end_polygon();

	fl_pop_matrix();
	
	//fl_pop_clip();


}


