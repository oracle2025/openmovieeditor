#include <cstdlib>
#include <math.h>
#include <FL/fl_draw.H>
#include <FL/Fl_Window.H>
#include <iostream>
#include <FL/Fl_Image.H>

#include "ColorWheel.H"
void HSVtoRGB( float *r, float *g, float *b, float h, float s, float v );

ColorWheel::ColorWheel( int x, int y, int w, int h, const char* label )
	: Fl_Widget( x, y, w, h, label )
{
	m_v = 1.0;
	m_x = m_y = 128.0;
	m_image = 0;
	m_len = 0;
	m_angle = 0;
	m_color = FL_WHITE;
}

ColorWheel::~ColorWheel()
{
	if ( m_image ) {
		delete m_image;
	}
}
int ColorWheel::handle( int event )
{
	
	switch ( event ) {
		case FL_PUSH:
		return 1;
		case FL_DRAG:
		case FL_RELEASE:
		m_x = ( (float)Fl::event_x() - (float)x() ) / (float)w();
		m_y = ( (float)Fl::event_y() - (float)y() ) / (float)h();
		m_x -= 0.5;
		m_y -= 0.5;
		m_x *= 2.0;
		m_y *= 2.0;
		m_len = sqrt( pow( m_x, 2 ) + pow( m_y, 2) );
		m_angle = (M_PI/2) + asin( m_y / m_len );
		if ( m_len > 1.0 ) {
			m_x = m_x / m_len;
			m_y = m_y / m_len;
			m_len = 1.0;
		}
		if ( m_x < 0.0 ) {
			m_angle = (2*M_PI) - m_angle;
		}
		m_angle /= (2*M_PI);

		float r, g, b;
		HSVtoRGB( &r, &g, &b, ( 450 - (int)( 360.0 * m_angle ) ) % 360, m_len, m_v );

		m_color = fl_rgb_color( (uchar)(r * 255), (uchar)(g * 255), (uchar)(b * 255) );
		m_r = (uchar)(r*255);
		m_g = (uchar)(g*255);
		m_b = (uchar)(b*255);


		m_x /= 2.0;
		m_y /= 2.0;
		m_x += 0.5;
		m_y += 0.5;
		m_x *= 256.0;
		m_y *= 256.0;
		redraw();
		do_callback();
		return 1;
	}
	Fl_Widget::handle( event );
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
	if ( !m_image ) {
		m_offscreen = fl_create_offscreen( w(), h() );
		fl_begin_offscreen(m_offscreen);
		fl_draw_box(FL_FLAT_BOX, 0, 0, w(), h(), FL_BACKGROUND_COLOR);


		fl_push_matrix();
		fl_translate( 2, 2 );
		fl_scale( ( w() - 4 ) / 256.0, ( h() - 4 ) / 256.0 );
		for ( int h_ = 0; h_ < 360; h_++ ) {
			for ( float s = 1.0; s > 0.0; s -= 0.01 ) {
				HSVtoRGB( &r, &g, &b, (float)h_, s, m_v );
				fl_color( (uchar)(r * 255), (uchar)(g * 255), (uchar)(b * 255) );
				fl_begin_polygon();
				//fl_vertex( 128.0, 128.0 );
				fl_arc( 128.0, 128.0, (s-0.05) * 120.0, (float)h_+1, (float)h_ );
				fl_arc( 128.0, 128.0, s * 120.0, (float)h_, (float)h_+1 );
				fl_end_polygon();
			}
		}
		fl_pop_matrix();


		m_pixels = new unsigned char[ w()*h()*4 ];
		fl_read_image(m_pixels, 0, 0, w(), h(), 255 );
		fl_end_offscreen();
		m_image = new Fl_RGB_Image( m_pixels, w(), h(), 4 );
	}
	char** d = (char**)m_image->data();
	fl_draw_image( (unsigned char *)d[0], x(), y(), w(), h(), 4 );
	fl_draw_box( FL_DOWN_FRAME, x(), y(), w(), h(), FL_BACKGROUND_COLOR );
	//fl_push_clip( x() + 2, y() + 2,  w() - 4, h() - 4 );
	fl_color( FL_BLACK );


	fl_push_matrix();
	fl_translate( x() + 2, y() + 2 );
	fl_scale( ( w() - 4 ) / 256.0, ( h() - 4 ) / 256.0 );


	fl_color( FL_BLACK );
	fl_begin_line();
	fl_vertex( 128.0, 128.0 );
	fl_vertex( m_x, m_y );
	fl_end_line();

	fl_begin_polygon();
	fl_circle( m_x, m_y, 6.0);
	fl_end_polygon();
	fl_color( FL_WHITE );
	fl_begin_polygon();
	fl_circle( m_x, m_y, 3.0);
	fl_end_polygon();


	fl_pop_matrix();
	
	//fl_pop_clip();


}


