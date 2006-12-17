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

#include "config.h"

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

void secs2HMS( double secs, int &H, int &M, int &S )
{
  S = (int)secs;
  M = S / 60; S -= M*60;
  H = M / 60; M -= H*60;
}

void guess_aspect( int w, int h, int* aspect_height, int* aspect_width, float* aspect_ratio, int* analog_blank, int* pixel_width, int* pixel_height )
{
	if ( w == 720 && h == 576 ) {
		if ( aspect_height ) *aspect_height = 4;
		if ( aspect_width ) *aspect_width = 3;
		if ( aspect_ratio ) *aspect_ratio = ( 4.0 / 3.0 );
		if ( analog_blank ) *analog_blank = 9;
		if ( pixel_width ) *pixel_width = 128;
		if ( pixel_height ) *pixel_height = 117;
	} else {
		unsigned long gcd_wh = gcd( w, h );
		if ( aspect_height ) *aspect_height = h / gcd_wh; 
		if ( aspect_width ) *aspect_width = w /gcd_wh;
		if ( aspect_ratio ) *aspect_ratio = (float)(w /gcd_wh) / (float)(h / gcd_wh);
		if ( analog_blank ) *analog_blank = 0;
		if ( pixel_width ) *pixel_width = 1;
		if ( pixel_height ) *pixel_height = 1;
	}

}



/* The following code was taken from gettext-0.14.6/gettext-tools/lib/gcd.c */

/* Arithmetic.
   Copyright (C) 2001-2002 Free Software Foundation, Inc.
   Written by Bruno Haible <bruno@clisp.org>, 2001.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.  */

unsigned long
gcd (unsigned long a, unsigned long b)
{
  /* Why no division, as in Euclid's algorithm? Because in Euclid's algorithm
     the division result floor(a/b) or floor(b/a) is very often = 1 or = 2,
     and nearly always < 8.  A sequence of a few subtractions and tests is
     faster than a division.  */
  /* Why not Euclid's algorithm? Because the two integers can be shifted by 1
     bit in a single instruction, and the algorithm uses fewer variables than
     Euclid's algorithm.  */

  unsigned long c = a | b;
  c = c ^ (c - 1);
  /* c = largest power of 2 that divides a and b.  */

  if (a & c)
    {
      if (b & c)
	goto odd_odd;
      else
	goto odd_even;
    }
  else
    {
      if (b & c)
	goto even_odd;
      else
	abort ();
    }

  for (;;)
    {
    odd_odd: /* a/c and b/c both odd */
      if (a == b)
	break;
      if (a > b)
	{
	  a = a - b;
	even_odd: /* a/c even, b/c odd */
	  do
	    a = a >> 1;
	  while ((a & c) == 0);
	}
      else
	{
	  b = b - a;
	odd_even: /* a/c odd, b/c even */
	  do
	    b = b >> 1;
	  while ((b & c) == 0);
	}
    }

  /* a = b */
  return a;
}

} /* namespace nle */
