/*  AutomationDragHandler.H
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

#ifndef _AUTOMATION_DRAG_HANDLER_H_
#define _AUTOMATION_DRAG_HANDLER_H_

#include "DragHandler.H"
#include "Rect.H"

namespace nle
{

struct _auto_node;
class AudioClip;

class AutomationDragHandler : public DragHandler
{
	public:
		AutomationDragHandler( Clip* clip, const Rect& rect, struct _auto_node* n = 0, int x_off = 0, int y_off = 0 );
		~AutomationDragHandler();
		void OnDrag( int x, int y );
		void OnDrop( int x, int y );
	private:
		bool m_dragging;
		Rect m_outline;
		struct _auto_node* m_node;
		struct _auto_node* m_node_before;
		bool m_firstNode;
		bool m_lastNode;
		int m_x_off;
		int m_y_off;
		AudioClip* m_audioClip;
};

/*class AutomationPoint;

class AutomationDragHandler : public DragHandler
{
	public:
		AutomationDragHandler( Clip* clip, AutomationPoint* aPoint, const Rect& rect );
		~AutomationDragHandler();
		void OnDrag( int x, int y );
		void OnDrop( int x, int y );
	private:
		AutomationPoint *m_aPoint;
		std::list<Rect*> m_rects;
		Rect* m_activeRect;
		Clip *m_clip;
		Rect m_outline;
}; */

} /* namespace nle */
#endif /* _AUTOMATION_DRAG_HANDLER_H_ */

