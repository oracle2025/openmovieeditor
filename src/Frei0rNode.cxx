#include <dlfcn.h>
#include "Frei0rNode.H"
#include "Frei0rDoubleSlider.H"


Frei0rNode::Frei0rNode( f0r_plugin_info_t* info, void* handle, int w, int h)
{
	f0r_construct = (f0r_construct_f)dlsym( handle, "f0r_construct" );
	f0r_destruct = (f0r_destruct_f)dlsym( handle, "f0r_destruct" );
	f0r_update = (f0r_update_f)dlsym( handle, "f0r_update" );
	f0r_update2 = (f0r_update2_f)dlsym( handle, "f0r_update2" );
	f0r_get_param_info = (f0r_get_param_info_f)dlsym( handle, "f0r_get_param_info" );
	f0r_set_param_value = (f0r_set_param_value_f)dlsym( handle, "f0r_set_param_value" );
	f0r_get_param_value = (f0r_get_param_value_f)dlsym( handle, "f0r_get_param_value" );
	m_info = info;
	m_instance = f0r_construct( w, h );
	m_frame = new uint32_t[w*h];
}
Frei0rNode::~Frei0rNode()
{
	if ( f0r_destruct ) {
		f0r_destruct( m_instance );
	}
	for ( int i = 0; node->widgets[i]; i++ ) {
		delete node->widgets[i];
	}
}
void Frei0rNode::setInput( int input, INode* node )
{
}
int Frei0rNode::getInputCount()
{
	switch ( m_info->plugin_type ) {
		case F0R_PLUGIN_TYPE_FILTER:
			return 1;
		case F0R_PLUGIN_TYPE_SOURCE:
			return 0;
		case F0R_PLUGIN_TYPE_MIXER2:
			return 2;
		case F0R_PLUGIN_TYPE_MIXER3:
			return 3;
	};
	return 0;
}
int Frei0rNode::getOutputCount()
{
	return 1;
}
void Frei0rNode::init()
{
	f0r_param_info param_info;
	int k = 0;
	for ( int i = 0; i < m_info->num_params; i++ ) {
		f0r_get_param_info( &param_info, i );
		if ( param_info.type == F0R_PARAM_DOUBLE ) {
			Frei0rDoubleSlider* slider = new Frei0rDoubleSlider( 0, 0, 25, 25 );
			slider->set_instance( m_instance, f0r_set_param_value, f0r_get_param_value, i );
			slider->show();
			node->widgets[k] = slider;
			k++;
		}
	}
	node->widgets[k] = 0;
}

uint32_t* Frei0rNode::getFrame( int output, double position )
{
	uint32_t* input_frame[3];
	for ( int i = 0; i < node->input_count && i < 3; i++ ) {
		if ( node->inputs[i] == 0 || !node->inputs[i]->node ) {
			return 0;
		}
		input_frame[i] = node->inputs[i]->node->getFrame( 0, position );
		if ( !input_frame[i] ) {
			return 0;
		}
	}
	switch ( m_info->plugin_type ) {
		case F0R_PLUGIN_TYPE_FILTER:
			f0r_update( m_instance, position , input_frame[0], m_frame );
			return m_frame;
		case F0R_PLUGIN_TYPE_SOURCE:
			f0r_update( m_instance, position , 0, m_frame );
			return m_frame;
		case F0R_PLUGIN_TYPE_MIXER2:
			f0r_update2( m_instance, position , input_frame[0], input_frame[1], 0, m_frame );
			return m_frame;
		case F0R_PLUGIN_TYPE_MIXER3:
			f0r_update2( m_instance, position , input_frame[0], input_frame[1], input_frame[2], m_frame );
			return m_frame;
	};
	return 0;
}

