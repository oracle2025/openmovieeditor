/*  FilmStripFactory.cxx
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

#include <sl.h>

#include "FilmStripFactory.H"
#include "IVideoFile.H"
#include "FilmStrip.H"


namespace nle
{

FilmStripFactory* g_filmStripFactory;
	
FilmStripFactory::FilmStripFactory()
{
	g_filmStripFactory = this;
	m_strips = 0;
}
FilmStripFactory::~FilmStripFactory()
{
	g_filmStripFactory = 0;
	assert( m_strips == 0 );
}

static int find_strip_helper( void* p, void* data )
{
	film_strip_node* node = (film_strip_node*)p;
	const char* filename = (const char*)data;
	if ( strcmp( node->filename.c_str(), filename ) == 0 ) {
		return 1;
	}
	return 0;
}
FilmStrip* FilmStripFactory::get( IVideoFile* vf )
{
	film_strip_node* node = (film_strip_node*)sl_map( m_strips, find_strip_helper, (void*)( vf->filename().c_str() ) );
	if ( node ) {
		node->ref_count++;
		return node->strip;
	}
	node = new film_strip_node;
	node->next = 0;
	node->ref_count = 1;
	node->filename = vf->filename();
	node->strip = new FilmStrip( vf );
	m_strips = (film_strip_node*)sl_push( m_strips, node );
	return node->strip;
}
static int remove_strip_helper( void* p, void* data )
{
	film_strip_node* node = (film_strip_node*)p;
	FilmStrip* s = (FilmStrip*)data;
	if ( s == node->strip ) {
		node->ref_count--;
		if ( node->ref_count == 0 ) {
			return 1;
		}
	}
	return 0;
}
void FilmStripFactory::remove( FilmStrip* fs )
{
	film_strip_node* node = (film_strip_node*)sl_remove( &m_strips, remove_strip_helper, (void*)fs );
	if ( node ) {
		delete node->strip;
		delete node;
	}
}



} /* namespace nle */
