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
#include "AutomationPoint.H"
#include "timeline/Clip.H"
#include "globals.H"
#include "TimelineView.H"


namespace nle
{

AutomationDragHandler::AutomationDragHandler( Clip* clip, AutomationPoint* aPoint, const Rect& rect )
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
}

} /* namespace nle */
