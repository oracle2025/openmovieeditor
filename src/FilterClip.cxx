/*  FilterClip.cxx
 *
 *  Copyright (C) 2007 Richard Spindler <richard.spindler AT gmail.com>
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

#include "sl/sl.h"
#include "FilterClip.H"

namespace nle
{

FilterClip::FilterClip( Track* track, int64_t position, int id = -1 );
	: Clip( track, position, id )
{
	m_filters = 0;
}
FilterClip::~FilterClip()
{
	filter_stack* node;
	while ( ( node = (filter_stack*)sl_pop( &m_filters ) ) ) {
		delete node->filter;
		delete node;
	}
}
void FilterClip::reset()
{
	for ( filter_stack* node = m_filters; node; node = node->next ) {
		node->filter->reset();
	}
}
//TODO: Either push or append should be removed
void FilterClip::pushFilter( FilterFactory* factory )
{
	//TODO change FilterFactory to FilterClip
	FilterBase* filter = factory->get( this );
	assert(filter);
	filter_stack* n = new filter_stack;
	n->next = 0;
	n->filter = filter;
	m_filters = (filter_stack*)sl_push( m_filters, n );
	
}
FilterBase* FilterClip::appendFilter( FitlerFactory* factory )
{
	FilterBase* filter = factory->get( this );
	assert(filter);
	filter_stack* n = new filter_stack;
	n->next = 0;
	n->filter = filter;
	m_filters = (filter_stack*)sl_unshift( m_filters, n );
	return filter;
}

static filter_stack* sl_swap( filter_stack* root )
{
	filter_stack* q = root;
	filter_stack* r;
	if ( !q || !q->next ) {
		return q;
	}
	r = q->next;
	q->next = r->next;
	r->next = q;
	return r;
}


void FilterClip::moveFilterUp( int num )
{
	if ( !m_filters || num <= 1 ) {
		return;
	}
	if ( num == 2 ) {
		m_filters = sl_swap( m_filters );
		return;
	}
	filter_stack* p = m_filters;
	for ( int i = 3; i < num; i++ ) {
		p = p->next;
	}
	p->next = sl_swap( p->next );

}
void FilterClip::moveFilterDown( int num )
{
	if ( !m_filters ) {
		return;
	}
	if ( num == 1 ) {
		m_filters = sl_swap( m_filters );
		return;
	}
	filter_stack* p = m_filters;
	for ( int i = 2; i < num; i++ ) {
		p = p->next;
	}
	p->next = sl_swap( p->next );

}
static int remove_filter_helper( void*, void* data )
{
	int* num = (int*)data;
	if ( *num ) {
		(*num)--;
		return 0;
	}
	return 1;
}

void FilterClip::removeFilter( int num )
{
	int count = num - 1;
	filter_stack* node = (filter_stack*)sl_remove( &m_filters, remove_filter_helper, &count );
	if ( node ) {
		delete node->filter;
		delete node;
	}

}
	
} /* namespace nle */

