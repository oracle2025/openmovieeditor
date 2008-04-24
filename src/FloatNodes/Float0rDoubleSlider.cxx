		
#include "Float0rDoubleSlider.H"
#include "VideoViewGL.H"

void float0r_double_slider_callback( Float0rDoubleSlider* o, void* data )
{
	o->apply_value();
}
		
Float0rDoubleSlider::Float0rDoubleSlider( int x, int y, int w, int h, const char *l )
	: Fl_Value_Slider( x, y, w, h, l )
{
	m_instance = 0;
	m_param_index = 0;
	type(5);
	callback( (Fl_Callback*)float0r_double_slider_callback );
}
Float0rDoubleSlider::~Float0rDoubleSlider()
{
}
void Float0rDoubleSlider::apply_value()
{
	double dval = value();
	if ( m_instance ) {
		f0r_set_param_value( m_instance, &dval, m_param_index );
		nle::g_videoView->redraw();
	}
}
void Float0rDoubleSlider::set_instance( f0r_instance_t i, f0r_set_param_value_f s, f0r_get_param_value_f g, int param_index )
{
	m_instance = i;
	f0r_set_param_value = s;
	f0r_get_param_value = g;
	m_param_index = param_index;

	double dval;
	f0r_get_param_value( m_instance, &dval, param_index );
	value( dval );
}


