#ifndef _NODE_FILTER_FLOAT0R_FACTORY_PLUGIN_H_
#define _NODE_FILTER_FLOAT0R_FACTORY_PLUGIN_H_

#include "float0r.h"
#include "IFloatNodeFilterFactoryPlugin.H"
#include "Float0rNode.H"

#include <string>

typedef void (*f0r_deinit_f)(void);
typedef int (*f0r_init_f)(void);
typedef void (*f0r_get_plugin_info_f)(f0r_plugin_info_t *info);

class Float0rNode;

class NodeFilterFloat0rFactoryPlugin : public IFloatNodeFilterFactoryPlugin
{
	public:
		NodeFilterFloat0rFactoryPlugin( std::string filename );
		~NodeFilterFloat0rFactoryPlugin();
		Float0rNode* get( int w, int h );
		IFloatNode* get_i_node( int w, int h ) { return (Float0rNode*)get( w, h ); }
		bool ok() { return m_ok; }
		const char* name();
		const char* identifier();
	private:
		f0r_init_f            f0r_init;
		f0r_deinit_f          f0r_deinit;
		f0r_get_plugin_info_f f0r_get_plugin_info;
		void*                 m_handle;
		f0r_plugin_info_t     m_info;
		bool                  m_ok;
};

#endif /* _NODE_FILTER_FLOAT0R_FACTORY_PLUGIN_H_ */
