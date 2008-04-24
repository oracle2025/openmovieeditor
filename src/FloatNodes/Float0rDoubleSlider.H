#ifndef _FLOAT0R_DOUBLE_SLIDER_H_
#define _FLOAT0R_DOUBLE_SLIDER_H_

#include <FL/Fl.H>
#include <FL/Fl_Value_Slider.H>
#include "float0r.h"

typedef void (*f0r_set_param_value_f)(f0r_instance_t instance, f0r_param_t param, int param_index);
typedef void (*f0r_get_param_value_f)(f0r_instance_t instance, f0r_param_t param, int param_index);  


class Float0rDoubleSlider : public Fl_Value_Slider
{
	public:
		Float0rDoubleSlider( int x, int y, int w, int h, const char *l = 0 );
		~Float0rDoubleSlider();
		void apply_value();
		void set_instance( f0r_instance_t i, f0r_set_param_value_f g, f0r_set_param_value_f s, int param_index );
	private:
		f0r_instance_t m_instance;
		f0r_set_param_value_f f0r_set_param_value;
		f0r_get_param_value_f f0r_get_param_value;
		int m_param_index;	
};

#endif /* _FLOAT0R_DOUBLE_SLIDER_H_ */
