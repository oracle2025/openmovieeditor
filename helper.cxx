/*  helper.cxx
 *
 *  Copyright (C) 2003 Richard Spindler <richard.spindler AT gmail.com>
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

#include <stdio.h>

#include "globals.H"
#include "helper.H"

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

} /* namespace nle */
