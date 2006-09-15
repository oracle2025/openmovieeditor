/*  IdleHandler.cxx
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

#include <FL/Fl.H>
#include "sl/sl.h"

#include "IdleHandler.H"

namespace nle
{

void process_callback( void* data );

typedef struct idle_node_ {
	struct idle_node_ *next;
	IdleHandler* handler;
} idle_node;

idle_node* g_idleHandlers = 0;
int remove_idle_node( void* p, void* data ) {
	IdleHandler* h = (IdleHandler*)data;
	idle_node* n = (idle_node*)p;
	return ( n->handler == h );
}

void suspend_idle_handlers()
{
	idle_node* p;
	for ( p = g_idleHandlers; p; p = p->next ) {
		Fl::remove_idle( process_callback, p->handler );
	}
}
void resume_idle_handlers()
{
	idle_node* p;
	for ( p = g_idleHandlers; p; p = p->next ) {
		Fl::add_idle( process_callback, p->handler );
	}
}

void process_callback( void* data )
{
	IdleHandler* h = (IdleHandler*)data;
	if ( !h->process() ) {
		Fl::remove_idle( process_callback, data );
		idle_node* p = (idle_node*)sl_remove( &g_idleHandlers, remove_idle_node, h );
		if ( p ) {
			delete p;
		}
	}
}

void IdleHandler::start()
{
	Fl::add_idle( process_callback, this );
	idle_node* p = new idle_node;
	p->next = 0;
	p->handler = this;
	g_idleHandlers = (idle_node*)sl_push( g_idleHandlers, p );
}
void IdleHandler::stop()
{
	Fl::remove_idle( process_callback, this );
	idle_node* p = (idle_node*)sl_remove( &g_idleHandlers, remove_idle_node, this );
	if ( p ) {
		delete p;
	}
}


}
