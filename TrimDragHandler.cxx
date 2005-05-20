/*  TrimDragHandler.cxx
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

#include <FL/Fl.H>
#include <FL/Fl_Widget.H>
#include <FL/Fl_Window.H>
#include <FL/fl_draw.H>

#include "TrimDragHandler.H"
#include "TimelineView.H"

namespace nle
{

TrimDragHandler::TrimDragHandler( TimelineView *tlv, Clip *clip,
		int track, int left, int right,
		bool trimRight )
	: DragHandler(tlv, clip), m_track(track), m_left(left),
	m_right(right), m_trimRight(trimRight)
{
}
void TrimDragHandler::OnDrag( int x, int y )
{
	m_tlv->window()->make_current();
	fl_overlay_rect( x,
			m_tlv->y() + TRACK_SPACING + (TRACK_SPACING + TRACK_HEIGHT)*m_track,
			1, TRACK_HEIGHT );
}
void TrimDragHandler::OnDrop( int x, int y )
{
	m_tlv->window()->make_current();
	fl_overlay_clear();
	m_tlv->trim_clip( m_clip, x, m_trimRight );
}
	
} /* namespace nle */
