/*  AutomationDragHandler.cxx
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

#include <FL/Fl_Window.H>
#include <FL/fl_draw.H>

#include "AutomationDragHandler.H"
#include "globals.H"
#include "global_includes.H"
#include "TimelineView.H"
#include "AudioClip.H"
#include "timeline/Track.H"


namespace nle
{

static int g_x_off;
static int g_y_off;

bool inside_node( auto_node* n, Rect& r, AudioClip* clip, int _x, int _y, bool first = false, bool last = false )
{
	if ( first ) {
		_x = _x - 5;
	} else if ( last ) {
		_x = _x + 5;
	}
	int x = g_timelineView->get_screen_position( clip->position() + n->x, clip->track()->stretchFactor() ) - 5;
	int y = (int)( r.y + ( ( TRACK_HEIGHT - 10 ) * ( 1.0 - n->y ) ) );
	int w = 10;
	int h = 10;
	g_x_off = x - _x;
	g_y_off = y - _y;
	Rect node( x, y, w, h );
	return node.inside( _x, _y );
}
void screen_to_node( int64_t& x, float& y, int in_x, int in_y, auto_node* n, auto_node* n_before, Rect& r, AudioClip* clip, bool first = false, bool last = false )
{
	if ( in_x < r.x ) { in_x = r.x; }
	if ( in_x > r.x + r.w - 10 ) { in_x = r.x + r.w - 10; }
	if ( in_y < r.y ) { in_y = r.y; }
	if ( in_y > r.y + r.h - 10 ) { in_y = r.y + r.h - 10; }
	
	y = 1.0 - ( ((float)in_y - r.y) / ((float)r.h - 10) );
	if ( first ) {
		x = 0;
	} else if ( last ) {
		x = clip->length();
	} else {
		x = g_timelineView->get_real_position( in_x + 5, clip->track()->stretchFactor() ) - clip->position();
	}
	if ( !first && !last ) {
		if ( n->next && x > n->next->x - 1000 ) {
			x = n->next->x - 1000;
		} else if ( x < n_before->x + 1000 ) {
			x = n_before->x + 1000;
		}	
	}
}

AutomationDragHandler::AutomationDragHandler( Clip* clip, const Rect& rect, struct _auto_node* n, int x_off, int y_off )
	: DragHandler( g_timelineView, clip ), m_x_off( x_off ), m_y_off( y_off )
{
	m_dragging = false;
	m_outline = rect;
	m_node = n;
	m_audioClip = (AudioClip*)m_clip;
	auto_node* r = m_audioClip->getAutoPoints();
	m_firstNode = false;
	m_lastNode = false;
	m_node_before = 0;
	for ( ; r; r = r->next ) {
		if ( r->next == 0 ) {
			m_lastNode = true;
		} else if ( r == m_audioClip->getAutoPoints() ) {
			m_firstNode = true;
		}
		if ( inside_node( r, m_outline, m_audioClip, x_off, y_off, m_firstNode, m_lastNode ) ) {
			m_node = r;
			m_dragging = true;
			m_x_off = g_x_off;
			m_y_off = g_y_off;
			if ( !m_firstNode ) {
				auto_node* q = m_audioClip->getAutoPoints();
				while ( q->next && q->next != m_node ) {
					q = q->next;
				}
				m_node_before = q;
			}
			break;
		}
		m_firstNode = m_lastNode = false;
	}

}
AutomationDragHandler::~AutomationDragHandler()
{
}

void AutomationDragHandler::OnDrag( int x, int y )
{
/*	if ( y > m_outline.y + m_outline.h - 2 ) {
		y = m_outline.y + m_outline.h - 2;
	} else if ( y < m_outline.y ) {
		y = m_outline.y;
	}
	fl_overlay_rect( m_outline.x, y + g_timelineView->y(), m_outline.w, 1 );
	*/
	if ( m_dragging ) {
		screen_to_node( m_node->x, m_node->y, x + m_x_off, y + m_y_off, m_node, m_node_before, m_outline, m_audioClip, m_firstNode, m_lastNode );

		if ( !m_firstNode && !m_lastNode && ( y < m_outline.y - 30 || y > m_outline.y + m_outline.h + 30 ) ) {
			//remove Node
			m_node_before->next = m_node->next;
			delete m_node;
			m_node = 0;
			m_dragging = false;
		}
		g_timelineView->redraw();
#if 0   //Code aus dem Beispiel Programm
		drag->r.x = _x - of_x;
		drag->r.y = _y - of_y;
		fit_to_clip( drag->r );
		if ( drag == nodes ) {
			drag->r.x = 0;
		}
		auto_node* r = nodes;
		while ( r->next ) {
			r = r->next;
		}
		if ( r == drag ) {
			drag->r.x = C_W - 10;
		}
		if ( drag->next && drag->r.x > drag->next->r.x - 10 ) {
			drag->r.x = drag->next->r.x - 10;
		}
		auto_node* f = nodes;
		while ( f->next && f->next != drag ) {
			f = f->next;
		}
		if ( drag != nodes && f && drag->r.x < f->r.x + 10 ) {
			drag->r.x = f->r.x + 10;
		}
		if ( ( _y < -30  || _y > C_H + 30 ) && r != drag && r != nodes ) {
			auto_node* q = nodes;
			for ( ; q; q = q->next ) {
				if ( q->next == drag ) {
					q->next = drag->next;
					delete drag;
					drag = 0;
					redraw();
					return 1;
				}
			}
		}
		redraw();

#endif

		
//		fl_overlay_rect( x + m_x_off, y + g_timelineView->y() + m_y_off, 10, 10 );
	}
}
void AutomationDragHandler::OnDrop( int x, int y )
{
	if ( !m_dragging && m_outline.inside( x, y ) ) {
		auto_node* r = m_audioClip->getAutoPoints();
		int64_t _x = g_timelineView->get_real_position( x + 5, m_audioClip->track()->stretchFactor() ) - m_audioClip->position();

		for ( ;r ; r = r->next ) {
			if ( r->x < _x && r->next && r->next->x > _x ) {
				auto_node* p = new auto_node;
				p->next = r->next;
				r->next = p;
				screen_to_node( p->x, p->y, x, y, p, r, m_outline, m_audioClip );
				g_timelineView->redraw();
				break;
			}
		}
		//Add Node
#if 0   //Code aus dem Beispiel Programm
		if ( !drag && cl.inside( _x, _y ) ) {
			// Add node if appropirate
			auto_node* r = nodes;
			for ( ; r; r = r->next ) {
				if ( r->r.x < _x && r->next && r->next->r.x > _x  ) {
					auto_node* p = new auto_node;
					p->next = r->next;
					p->r = Rect( _x - 5, _y - 5, 10, 10 );
					r->next = p;
					fit_to_clip( p->r );	
					redraw();
					break;
				}
			}
		}
#endif
	}
}
/*AutomationDragHandler::AutomationDragHandler( Clip* clip, AutomationPoint* aPoint, const Rect& rect )
	: DragHandler(g_timelineView, clip)
{
	m_outline = rect;
	m_clip = clip;
	m_aPoint = aPoint;
	std::list<AutomationPoint>::iterator i;
	std::list<AutomationPoint>* l = clip->getAutomation();
	for ( i = l->begin(); i != l->end(); i++ ) {
		AutomationPoint *current = &(*i);
		Rect* t = new Rect(current->getScreenRect( clip ));
		if ( current == aPoint ) {
			m_activeRect = t;
		}
		m_rects.push_back( t );
	}
}
AutomationDragHandler::~AutomationDragHandler()
{
	std::list<Rect*>::iterator i;
	for ( i = m_rects.begin(); i != m_rects.end(); i++ ) {
		delete (*i);
	}
	m_rects.clear();
}
void fitRectInside( const Rect& outline, Rect* fitling ){
	if ( fitling->x < outline.x )
		fitling->x = outline.x;
	if ( fitling->x + 8 > outline.x + outline.w )
		fitling->x = outline.x + outline.w - 8;
	if ( fitling->y < outline.y )
		fitling->y = outline.y;
	if ( fitling->y + 8 > outline.y + outline.h )
		fitling->y = outline.y + outline.h - 8;
}
void AutomationDragHandler::OnDrag( int x, int y )
{
	m_activeRect->x = x - 4;
	m_activeRect->y = y - 4;
	fitRectInside( m_outline, m_activeRect );
	//drawWithRects( clip, m_rects);
	g_timelineView->window()->make_current();
	fl_draw_box( FL_UP_BOX, g_timelineView->x() + m_outline.x, g_timelineView->y() + m_outline.y, m_outline.w, m_outline.h, FL_GRAY );

	std::list<Rect*>::iterator i;
	int xx = -1;
	int yy = -1;
	fl_color(FL_WHITE);
	fl_line_style(FL_SOLID);
	for ( i = m_rects.begin(); i != m_rects.end(); i++ ) {
		Rect *current = *i;
		if ( xx > 0 && yy > 0 ) {
			fl_line( xx, yy, g_timelineView->x() + current->x + 4, g_timelineView->y() + current->y + 4 );
		}
		xx  = g_timelineView->x() + current->x + 4;
		yy  = g_timelineView->y() + current->y + 4;
	}
	for ( i = m_rects.begin(); i != m_rects.end(); i++ ) {
		Rect *current = *i;
		fl_draw_box( FL_UP_BOX, g_timelineView->x() + current->x, g_timelineView->y() + current->y, current->w, current->h, FL_GREEN );
	}
}
void AutomationDragHandler::OnDrop( int x, int y )
{
	//setAutomation
	g_timelineView->window()->make_current();
	g_timelineView->redraw();
}*/

} /* namespace nle */
