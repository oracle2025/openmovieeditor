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
#include "DocManager.H"
#include "AutomationDuoMoveCommand.H"

namespace nle
{



ShiftAutomationDragHandler::ShiftAutomationDragHandler( Clip* clip, const Rect& rect, int x_, int )
	: DragHandler( g_timelineView, clip )
{
	m_outline = rect;
	m_audioClip = dynamic_cast<AudioClip*>(m_clip);
	m_nodeA = 0;
	m_nodeB = 0;
	auto_node* r = m_audioClip->getAutoPoints();
	m_nodesOriginal = m_audioClip->getAutoPoints();
	{
		auto_node* q  = 0;
		auto_node* s  = 0;
		m_nodesCopy = 0;
		/* Copy them to m_nodesCopy; */
		for ( ; r; r = r->next ) {
			s = new auto_node;
			s->next = 0;
			s->x = r->x;
			s->y = r->y;
			if ( q ) {
				q->next = s;
				q = s;
			} else {
				q = s;
				m_nodesCopy = s;
			}
		}
		m_audioClip->setAutoPoints( m_nodesCopy );
		r = m_audioClip->getAutoPoints();
	}
	int i = 0;
	for ( ; r && r->next; r = r->next ) {
		int x_1 = g_timelineView->get_screen_position( m_clip->position() + r->x, m_clip->track()->stretchFactor() );
		int x_2 = g_timelineView->get_screen_position( m_clip->position() + r->next->x, m_clip->track()->stretchFactor() );
		if ( x_ > x_1 && x_ < x_2 ) {
			m_nodeA = r;
			m_nodeB = r->next;
			m_node_number = i;
			break;
		}
		i++;
	}
	//m_firstA = ( m_nodeA == m_audioClip->getAutoPoints() ); // is this even relevant? I don't think so
	//m_lastB = ( m_nodeB->next == 0 );
}
ShiftAutomationDragHandler::~ShiftAutomationDragHandler()
{
}
void ShiftAutomationDragHandler::OnDrag( int, int y )
{
	if ( y < m_outline.y ) { y = m_outline.y; }
	if ( y > m_outline.y + m_outline.h - 10 ) { y = m_outline.y + m_outline.h - 10; }
	float y_ = 1.0 - ( ((float)y - m_outline.y) / ((float)m_outline.h - 10) );
	m_nodeA->y = y_;
	m_nodeB->y = y_;
	g_timelineView->redraw();
}
static void clear_node_list( auto_node** l )
{
	auto_node* n;
	while ( ( n = (auto_node*)sl_pop( l ) ) ) {
		delete n;
	}
}

void ShiftAutomationDragHandler::OnDrop( int, int y )
{
	if ( y < m_outline.y ) { y = m_outline.y; }
	if ( y > m_outline.y + m_outline.h - 10 ) { y = m_outline.y + m_outline.h - 10; }
	float y_ = 1.0 - ( ((float)y - m_outline.y) / ((float)m_outline.h - 10) );
	m_nodeA->y = y_;
	m_nodeB->y = y_;
	m_audioClip->setAutoPoints( m_nodesOriginal );
	m_nodesOriginal = 0;
	clear_node_list( &m_nodesCopy );
	Command* cmd = new AutomationDuoMoveCommand( m_clip, m_node_number, m_node_number + 1, y_ );
	submit( cmd );
	g_timelineView->redraw();
}

} /* namespace nle */
