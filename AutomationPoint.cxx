/*  Automation.cxx
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

#include "AutomationPoint.H"
#include "timeline/Clip.H"
#include "timeline/Track.H"
#include "TimelineView.H"
#include "globals.H"

namespace nle
{

Rect AutomationPoint::getScreenRect( Clip* clip )
{
	Rect tmp = g_timelineView->get_clip_rect( clip );
	int x_a = g_timelineView->get_screen_position( x + clip->position(), clip->track()->stretchFactor() );
	x_a -= 4;
	int y_a = ( ( 100 - y ) * 30 / 100 ) + tmp.y;
	y_a -= 4;
	Rect res( x_a, y_a, 8, 8 );
	return res;
}

} /* namespace nle */
