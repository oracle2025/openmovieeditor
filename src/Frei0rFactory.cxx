/*  Frei0rFactory.cxx
 *
 *  Copyright (C) 2006 Richard Spindler <richard.spindler AT gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <FL/filename.H>

#include "sl/sl.h"
#include "Frei0rFactory.H"
#include "Frei0rFactoryPlugin.H"
#include "IEffectMenu.H"

#define FREI0R_DIR_1 "/usr/lib/frei0r-1/"
#define FREI0R_DIR_2 "/usr/local/lib/frei0r-1/"
#define FREI0R_DIR_3 "/.frei0r-1/lib/"

namespace nle
{


typedef struct _dir_list_node {
	struct _dir_list_node *next;
	string name;
} dir_node;

Frei0rFactory* g_frei0rFactory;

Frei0rFactory::Frei0rFactory( IEffectMenu* menu )
{
	//Scan directorys and enumerate Plugins
	int n;
	dirent** list;
	dir_node* folders = 0;
	dir_node* p;
	effect_node* e;
	m_effects = 0;
	g_frei0rFactory = this;
	n = scandir( FREI0R_DIR_2, &list, 0, alphasort );
	while( n-- ) {
		p = new dir_node;
		p->next = 0;
		p->name = string( FREI0R_DIR_2 ) + list[n]->d_name;
		folders = (dir_node*)sl_push( folders, p );
	}
	for ( int i = n; i > 0; ) { // This is some bad ass hacking style from the fltk manual ;)
		free( (void*)( list[--i] ) );
	}
	free( (void*)list );


	while( folders ) {
		//TODO: endless loop somewhere
		//cout 
		p = (dir_node*)sl_pop( &folders );
		if ( fl_filename_isdir( p->name.c_str() ) ) {
			n = scandir( p->name.c_str(), &list, 0, alphasort );
			while( n-- ) {
				if ( list[n]->d_name[0] != '.'  ) {
					p = new dir_node;
					p->next = 0;
					p->name = string( FREI0R_DIR_2 ) + list[n]->d_name;
					folders = (dir_node*)sl_push( folders, p );
				}
			}
			for ( int i = n; i > 0; ) { // This is some bad ass hacking style from the fltk manual ;)
				free( (void*)( list[--i] ) );
			}
			free( (void*)list );
		} else {
			Frei0rFactoryPlugin* effect = new Frei0rFactoryPlugin( p->name.c_str() );
			if ( effect->ok() ) {
				menu->addEffect( effect );
				e = new effect_node;
				e->next = 0;
				e->effect = effect;
				m_effects = (effect_node*)sl_push( m_effects, e );
			} else {
				delete effect;
			}
		}
		
	}

}

Frei0rFactory::~Frei0rFactory()
{
	effect_node* node;
	while ( ( node = (effect_node*)sl_pop( &m_effects ) ) ) {
		delete node->effect;
		delete node;
	}

}

void Frei0rFactory::list()
{
}

AbstractEffectFactory* Frei0rFactory::get( string name )
{
	effect_node* p;
	for ( p = m_effects; p; p = p->next ) {
		if ( name == p->effect->name() ) {
			return p->effect;
		}
	}
	return 0;
}


} /* namespace nle */
