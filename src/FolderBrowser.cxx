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

#include "FolderBrowser.H"

namespace nle
{
FolderBrowser::FolderBrowser( int x, int y, int w, int h, const char *l )
	: Fl_Browser( x, y, w, h, l )
{
	load( getenv( "HOME" ) );
}
void FolderBrowser::load( string folder )
{
	dirent	**folders;
	int count;
	count = fl_filename_list( folder.c_str(), &folders, fl_casealphasort );
	for ( int i = 0; i < count; i++ ) {
		if ( files[i]->d_name[0] != '.' && fl_filename_isdir( files[i]->d_name ) ) {
			add( files[i]->d_name );
		}
	}
	for ( int i = count; i > 0; ) {
		free( (void*)(folders[--i]) );
	}
	free( (void*)folders );
}
void FolderBrowser::up()
{
}

} /* namespace nle */
