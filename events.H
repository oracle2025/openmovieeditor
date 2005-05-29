/*  events.H
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

#ifndef _EVENTS_H_
#define _EVENTS_H_


#include "Ruler.H"
#include "Flmm_Scalebar.H"
#include "globals.H"
#include "nle.h"
#include "VideoViewGL.H"

namespace nle
{


inline void e_stylus_position( long stylus_pos )
{
	g_ruler->stylus( stylus_pos );
}
inline void e_seek_position( int64_t position )
{
	g_videoView->seek( position );
	g_timeline->seek( position );
}

inline void e_scroll_position( long scroll_pos, long num, long len )
{
	/*
	position
	number = width * zoom
	top = scroll_pos
	length = 1024
	*/
	g_scrollBar->value( scroll_pos, num, 0, len );
}


} /* namespace nle */
#endif /* _EVENTS_H_ */

