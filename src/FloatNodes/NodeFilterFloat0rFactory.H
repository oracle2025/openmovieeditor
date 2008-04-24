#ifndef _NODE_FILTER_FLOAT0R_FACTORY_H_
#define _NODE_FILTER_FLOAT0R_FACTORY_H_

#include <string>
#include "NodeFilterFrei0rFactory.H"

class NodeFilterFloat0rFactoryPlugin;

typedef struct _effect_list_node_float0r {
	struct _effect_list_node_float0r* next;
	NodeFilterFloat0rFactoryPlugin* effect;
} effect_node_float0r;

class NodeFilterFloat0rFactory;
extern NodeFilterFloat0rFactory* g_node_filter_float0r_factory;

class NodeFilterFloat0rFactory
{
	public:
		NodeFilterFloat0rFactory();
		~NodeFilterFloat0rFactory();
		NodeFilterFloat0rFactoryPlugin* get( std::string name );
		effect_node_float0r* m_effects;
	private:
		void enumerate( std::string folder );
};

#endif /* _NODE_FILTER_FLOAT0R_FACTORY_H_ */
