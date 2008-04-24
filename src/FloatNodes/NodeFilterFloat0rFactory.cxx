
#include "NodeFilterFloat0rFactory.H"
#include "NodeFilterFloat0rFactoryPlugin.H"
#include "sl/sl.h"

#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <cstdlib>

#include <FL/filename.H>
#include <iostream>

#define FLOAT0R_DIR_1 "/usr/lib/float0r-1/"
#define FLOAT0R_DIR_2 "/usr/local/lib/float0r-1/"
#define FLOAT0R_DIR_3 "/.float0r-1/lib/"

static std::string home( getenv( "HOME" ) );

typedef struct _dir_list_node {
	struct _dir_list_node *next;
	std::string name;
} dir_node;

NodeFilterFloat0rFactory* g_node_filter_float0r_factory = 0;

NodeFilterFloat0rFactory::NodeFilterFloat0rFactory()
{
	m_effects = 0;
	enumerate( home + FLOAT0R_DIR_3 );
	enumerate( FLOAT0R_DIR_2 );
	enumerate( FLOAT0R_DIR_1 );
	g_node_filter_float0r_factory = this;
}

NodeFilterFloat0rFactory::~NodeFilterFloat0rFactory()
{
	effect_node_float0r* node;
	while ( ( node = (effect_node_float0r*)sl_pop( &m_effects ) ) ) {
		delete node->effect;
		delete node;
	}
}

void NodeFilterFloat0rFactory::enumerate( std::string folder )
{
	int n;
	dirent** list;
	dir_node* folders = 0;
	dir_node* p;
	effect_node_float0r* e;
	n = scandir( folder.c_str(), &list, 0, alphasort );
	if ( n <= 0 ) {
		return;
	}
	for ( int i = 0; i < n; i++ ) {
		if ( list[i]->d_name[0] != '.' ) {
			p = new dir_node;
			p->next = 0;
			p->name = std::string( folder ) + list[i]->d_name;
			folders = (dir_node*)sl_push( folders, p );
		}
	}
	for ( int i = n; i > 0; ) { // This is some bad ass hacking style from the fltk manual ;)
		free( (void*)( list[--i] ) );
	}
	if ( n > 0 ) {
		free( (void*)list );
	}

	while( folders ) {
		p = (dir_node*)sl_pop( &folders );
		if ( fl_filename_isdir( p->name.c_str() ) ) {
			n = scandir( p->name.c_str(), &list, 0, alphasort );
			while( n-- ) {
				if ( list[n]->d_name[0] != '.'  ) {
					p = new dir_node;
					p->next = 0;
					p->name = std::string( folder ) + list[n]->d_name;
					folders = (dir_node*)sl_push( folders, p );
				}
			}
			for ( int i = n; i > 0; ) { // This is some bad ass hacking style from the fltk manual ;)
				free( (void*)( list[--i] ) );
			}
			if ( n > 0 ) {
				free( (void*)list );
			}
		} else {
			NodeFilterFloat0rFactoryPlugin* effect = new NodeFilterFloat0rFactoryPlugin( p->name.c_str() );
			if ( effect->ok() ) {
				e = new effect_node_float0r;
				e->next = 0;
				e->effect = effect;
				m_effects = (effect_node_float0r*)sl_push( m_effects, e );
			} else { 
				delete effect;
			}
		}
		
	}

}

NodeFilterFloat0rFactoryPlugin* NodeFilterFloat0rFactory::get( std::string name )
{
	effect_node_float0r* p;
	for ( p = m_effects; p; p = p->next ) {
		if ( name == p->effect->name() ) {
			return p->effect;
		}
	}
	return 0;
}

