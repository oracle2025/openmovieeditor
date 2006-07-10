/*  MediaBrowser.cxx
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

#include <iostream>
#include <cstring>

#include <FL/Fl.H>
#include <FL/filename.H>
#include <FL/fl_draw.H>

#include "MediaBrowser.H"


namespace nle
{

MediaBrowser::MediaBrowser( int x, int y, int w, int h, const char *l )
	: Fl_Browser_( x, y, w, h, l )
{
	m_last = m_items = 0;
	m_item_selected = 0; 
	load( getenv( "HOME" ) );
}

MediaBrowser::~MediaBrowser()
{
	clear();
}

void MediaBrowser::load( string folder )
{
	m_folder = folder;
	clear();
	dirent **files;
	int count;
	count = fl_filename_list( folder.c_str(), &files, fl_casealphasort );
	for ( int i = 0; i < count; i++ ) {
		if ( files[i]->d_name[0] != '.' && !fl_filename_isdir( string(folder + "/" + files[i]->d_name).c_str() ) ) {
			file_item* f = new file_item;
			f->value = files[i]->d_name;
			append( f );
			//add( files[i]->d_name );
		}
	}
	for ( int i = count; i > 0; ) {
		free( (void*)(files[--i]) );
	}
	free( (void*)files );
	new_list();
	redraw_lines();
}

void MediaBrowser::click()
{
	//cout << ( m_folder + "/" + text( value() ) ) << endl;
}
void MediaBrowser::append( file_item* f )
{
	f->next = 0;
	f->prev = m_last;
	if ( m_items ) {
		m_last->next = f;
		m_last = f;
	} else {
		m_last = m_items = f;
	}
}
void MediaBrowser::clear()
{
	while ( m_items ) {
		file_item* f = m_items;
		m_items = m_items->next;
		delete f;
	}
	m_last = 0;
	m_item_selected = 0;
}

int MediaBrowser::handle( int event )
{
	int x, y, w, h;
	bbox( x, y, w, h );
	if ( !Fl::event_inside( x, y, w, h ) ) {
		return Fl_Browser_::handle( event );
	}
	switch ( event ) {
		case FL_PUSH:
			m_item_selected = (file_item*)find_item( Fl::event_y() );
			if ( m_item_selected ) {
				redraw_line( m_item_selected );
			}
			return 1;
		case FL_RELEASE:
			if ( m_item_selected ) {
				redraw_line( m_item_selected );
				m_item_selected = 0;
			}
			return 1;
		case FL_DRAG:
			if ( m_item_selected ) {
				string s = m_folder + "/" + m_item_selected->value;
				cout << s << endl;
				const char* c = s.c_str();
				Fl::copy( c, strlen(c), 0 );
				Fl::dnd();
				redraw_line( m_item_selected );
				m_item_selected = 0;
			}
			return 1;
		case FL_FOCUS:
			return 1;
	}
	return Fl_Browser_::handle( event );
}

void MediaBrowser::item_draw( void* p, int x, int y, int w, int h ) const
{
	file_item* f = (file_item*)p;
	if ( m_item_selected == f ) {
		fl_draw_box( FL_UP_BOX, x, y, w, h, FL_SELECTION_COLOR );
	} else {
		fl_draw_box( FL_UP_BOX, x, y, w, h, FL_BACKGROUND_COLOR );
	}
	fl_font( FL_HELVETICA, 14 );
	fl_color( FL_FOREGROUND_COLOR );
	fl_draw( f->value.c_str(), x, y + 20 );
}
void* MediaBrowser::item_first() const
{
	return m_items;
}
int MediaBrowser::item_height( void* p ) const
{
	return 40;
}
void* MediaBrowser::item_next( void* p ) const
{
	file_item* f = (file_item*)p;
	return f->next;
}
void* MediaBrowser::item_prev( void* p ) const
{
	file_item* f = (file_item*)p;
	return f->prev;
}
int MediaBrowser::item_width( void* p ) const
{
	return 20;
}

} /* namespace nle */
