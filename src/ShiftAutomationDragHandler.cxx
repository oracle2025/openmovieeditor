/*  ShiftAutomationDragHandler.cxx
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


#include "ShiftAutomationDragHandler.H"
#include "globals.H"
#include "global_includes.H"
#include "TimelineView.H"
#include "AudioClip.H"
#include "timeline/Track.H"

namespace nle
{



ShiftAutomationDragHandler::ShiftAutomationDragHandler( Clip* clip, const Rect& rect, int x_, int y_ )
	: DragHandler( g_timelineView, clip )
{
	m_outline = rect;
	m_audioClip = dynamic_cast<AudioClip*>(m_clip);
	m_nodeA = 0;
	m_nodeB = 0;
	auto_node* r = m_audioClip->getAutoPoints();
	for ( ; r && r->next; r = r->next ) {
		int x_1 = g_timelineView->get_screen_position( m_clip->position() + r->x, m_clip->track()->stretchFactor() );
		int x_2 = g_timelineView->get_screen_position( m_clip->position() + r->next->x, m_clip->track()->stretchFactor() );
		if ( x_ > x_1 && x_ < x_2 ) {
			m_nodeA = r;
			m_nodeB = r->next;
			break;
		}
	}
	m_firstA = ( m_nodeA == m_audioClip->getAutoPoints() ); // is this even relevant? I don't think so
	m_lastB = ( m_nodeB->next == 0 );
}
ShiftAutomationDragHandler::~ShiftAutomationDragHandler()
{
}
void ShiftAutomationDragHandler::OnDrag( int x, int y )
{
	if ( y < m_outline.y ) { y = m_outline.y; }
	if ( y > m_outline.y + m_outline.h - 10 ) { y = m_outline.y + m_outline.h - 10; }
	float y_ = 1.0 - ( ((float)y - m_outline.y) / ((float)m_outline.h - 10) );
	m_nodeA->y = y_;
	m_nodeB->y = y_;
	g_timelineView->redraw();
}
void ShiftAutomationDragHandler::OnDrop( int x, int y )
{
	if ( y < m_outline.y ) { y = m_outline.y; }
	if ( y > m_outline.y + m_outline.h - 10 ) { y = m_outline.y + m_outline.h - 10; }
	float y_ = 1.0 - ( ((float)y - m_outline.y) / ((float)m_outline.h - 10) );
	m_nodeA->y = y_;
	m_nodeB->y = y_;
	g_timelineView->redraw();
}

} /* namespace nle */
