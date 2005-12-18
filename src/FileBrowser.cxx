/*  FileBrowser.cxx
 *
 *  Copyright (C) 2005 Richard Spindler <richard.spindler AT gmail.com>
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

#include <cstring>

#include <FL/filename.H>

#include "FileBrowser.H"
#include "Prefs.H"
#include "globals.H"

namespace nle
{
	
FileBrowser::FileBrowser( int x, int y, int w, int h, const char *l )
	: Fl_File_Browser( x, y, w, h, l )
{
	type( 1 );
	m_path = g_preferences->getBrowserFolder();
	load( m_path.c_str() );
}

int FileBrowser::handle( int event )
{
	if ( event == FL_DRAG ) {
		string s = m_path;
		if ( text( value() )[0] != '/' && m_path.length() > 1 )
			s += '/';
		s += text( value() );
		Fl::copy( s.c_str(), s.length(), 0 );
		Fl::dnd();
		return 1;
	}
	return Fl_File_Browser::handle( event );
}
void FileBrowser::load_rel()
{
	const char *d = text( value() );
	char buffer[FL_PATH_MAX];
	char *dirptr;
	if ( d == 0 )
		return;
	strncpy( buffer, m_path.c_str(), sizeof(buffer) );
	buffer[FL_PATH_MAX - 1] = '\0';
	if ( strlen(buffer) > 1 ) {
		strncat( buffer, "/", sizeof(buffer) - strlen( buffer ) - 1 );
	}
	strncat( buffer, d, sizeof(buffer) - strlen( buffer ) - 1 );

	// '..' removal, shamelessly copied from fltk file-selector ;)
	dirptr = buffer + strlen( buffer ) - 1;
	if ( ( *dirptr == '/' || *dirptr == '\\' ) && dirptr > buffer )
		*dirptr =  '\0';

	dirptr = buffer + strlen( buffer ) - 3;
	if ( dirptr >= buffer && strcmp( dirptr, "/.." ) == 0 ) {
		*dirptr = '\0';
		while ( dirptr > buffer ) {
			if ( *dirptr == '/' ) break;
			dirptr --;
		}
		if ( dirptr >= buffer && *dirptr == '/' )
			*dirptr = '\0';
	} else if ( ( dirptr + 1 ) >= buffer && strcmp( dirptr + 1, "/." ) == 0 ) {
		dirptr[1] = '\0';
	}
	
	string new_path = buffer;
	if ( new_path.length() == 0 )
		new_path = "/";
	if ( fl_filename_isdir( new_path.c_str() ) ) {
		load( new_path.c_str() );
		m_path = new_path;
		g_preferences->setBrowserFolder( m_path.c_str() );
	}
}

} /* namespace nle */
