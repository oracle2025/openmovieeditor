/*  helper.cxx
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

#include <cstdio>
#include <cstring>
#include <cerrno>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "globals.H"
#include "helper.H"

#define BUFFER_LEN 1024

namespace nle
{

const char* timestamp_to_string( int64_t timestamp )
{
	static char buffer[256];
	int hours = (int) ( timestamp / ( g_fps * 60 * 60 ) );
	int minutes = (int) ( ( timestamp / ( g_fps * 60 ) ) ) % 60;
	int seconds = (int) ( ( timestamp / g_fps ) ) % 60;
	snprintf( buffer, 256, "%02d:%02d:%02d", hours, minutes, seconds );
	return buffer;
}

int mkdirp( const char* pathname )
{
	char* p = (char*)pathname;
	int len = strlen(pathname);
	char buffer[BUFFER_LEN];
	struct stat statbuf;
	strncpy( buffer, pathname, BUFFER_LEN );
	while ( p - pathname < len ) {
		buffer[p - pathname] = pathname[p - pathname];
		p++;
		while ( *p != '/' && p - pathname < len )
			p++;
		strncpy( buffer, pathname, BUFFER_LEN/*p - pathname*/ );
		buffer[p - pathname] = '\0';
		if ( stat( buffer, &statbuf ) == -1 && errno == ENOENT ) {
			mkdir( buffer, 0700 );
		}
		
	}
	return 0;
}
void findpath( const char* filename, char* buffer, int bufferlen )
{
	char* p = (char*)filename;
	int len = strlen(filename);
	struct stat statbuf;
	char buffer2[BUFFER_LEN];
	strncpy( buffer2, filename, BUFFER_LEN );
	while( p - filename < len ) {
		buffer2[p - filename] = filename[p - filename];
		p++;
		while ( *p != '/' && p - filename < len )
			p++;
		buffer2[p - filename] = '\0';
		if ( stat( buffer2, &statbuf ) != -1 ) {
			if ( !S_ISDIR( statbuf.st_mode ) ) {
				return;
			}
		}
		strncpy( buffer, buffer2, bufferlen );
	}
	return;
}

} /* namespace nle */
