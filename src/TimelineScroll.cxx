/*  TimelineScroll.cxx
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

#include "TimelineScroll.H"


namespace nle
{

TimelineScroll::TimelineScroll(  int x, int y, int w, int h, const char *label  )
	: Fl_Scroll( x, y, w, h, label )
{
}

TimelineScroll::~TimelineScroll() {}
void TimelineScroll::resize( int x, int y, int w, int h )
{
	child(0)->size( w, child(0)->h() );
	Fl_Scroll::resize( x, y, w, h );
}


} /* namespace nle */
