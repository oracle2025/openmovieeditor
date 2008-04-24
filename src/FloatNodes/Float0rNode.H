#ifndef _FLOAT0R_NODE_H_
#define _FLOAT0R_NODE_H_

#include "float0r.h"
#include "Float0rGraphEditor.H"
#include "IFloatNode.H"

typedef f0r_instance_t (*f0r_construct_f)(unsigned int, unsigned int);
typedef void (*f0r_destruct_f)(f0r_instance_t);
typedef void (*f0r_update_f)(f0r_instance_t, double, const float*, float*);
typedef void (*f0r_update2_f)(f0r_instance_t, double, const float*, const float*, const float*, float*);
typedef void (*f0r_get_param_info_f)(f0r_param_info_t *info, int param_index);
typedef void (*f0r_set_param_value_f)(f0r_instance_t instance, f0r_param_t param, int param_index);
typedef void (*f0r_get_param_value_f)(f0r_instance_t instance, f0r_param_t param, int param_index);  




class Float0rNode : public IFloatNode
{
	public:
		Float0rNode( f0r_plugin_info_t* info, void* handle, int w, int h);
		~Float0rNode();
		int getInputCount();
		int getOutputCount();
		float* getFrame( int output, double position );
		void init();
		void init_widgets();
		void delete_widgets();
		void readXML( TiXmlElement* xml_node );
		void writeXML( TiXmlElement* xml_node );
		const char* identifier();
	private:
		f0r_instance_t m_instance;
		f0r_construct_f f0r_construct;
		f0r_destruct_f f0r_destruct;
		f0r_update_f f0r_update;
		f0r_update2_f f0r_update2;
		f0r_get_param_info_f  f0r_get_param_info;
		f0r_set_param_value_f f0r_set_param_value;
		f0r_get_param_value_f f0r_get_param_value;
		f0r_plugin_info_t* m_info;
		float* m_frame;
};

#endif /* _FLOAT0R_NODE_H_ */

