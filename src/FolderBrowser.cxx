/*  FolderBrowser.cxx
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
#include <FL/filename.H>
#include "FolderBrowser.H"

#include <iostream>

namespace nle
{

void strip_dots( char* out, const char* in ) 
{
	char* dirptr;
	char buffer[FL_PATH_MAX];
	strncpy( buffer, in, FL_PATH_MAX );
	dirptr = buffer + strlen(buffer) - 1;
	if ( ( *dirptr == '/' || *dirptr == '\\' ) && dirptr > buffer )
		*dirptr = '\0';

	dirptr = buffer + strlen( buffer ) - 3;
	if ( dirptr >= buffer && strcmp( dirptr, "/.." ) == 0 )  {
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
	strncpy( out, buffer, FL_PATH_MAX );
}
	
FolderBrowser::FolderBrowser( int x, int y, int w, int h, const char *l )
	: Fl_Hold_Browser( x, y, w, h, l )
{
	load( getenv( "HOME" ) );
}
void FolderBrowser::load( string folder )
{
	m_folder = folder;
	clear();
	dirent	**folders;
	int count;
	count = fl_filename_list( folder.c_str(), &folders, fl_casealphasort );
	for ( int i = 0; i < count; i++ ) {
		if ( folders[i]->d_name[0] != '.' && fl_filename_isdir( string(folder + "/" + folders[i]->d_name).c_str() ) ) {
			add( folders[i]->d_name );
		}
	}
	for ( int i = count; i > 0; ) {
		free( (void*)(folders[--i]) );
	}
	free( (void*)folders );
}
void FolderBrowser::up()
{
	char buffer[FL_PATH_MAX];
	strip_dots( buffer, string( m_folder + "/.." ).c_str() );
	if ( buffer[0] == '\0' ) {
		strncpy( buffer, "/", FL_PATH_MAX );
	}
	load( buffer );
}

void FolderBrowser::click()
{
	int v = value();
	if ( v ) {
		if ( m_folder == "/" ) {
			load( m_folder + text( v ) );
		} else {
			load( m_folder + "/" + text( v ) );
		}
	}
}

} /* namespace nle */
