/*  ShiftAutomationDragHandler.H
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

#ifndef _SHIFT_AUTOMATION_DRAG_HANDLER_H_
#define _SHIFT_AUTOMATION_DRAG_HANDLER_H_

#include "DragHandler.H"

namespace nle
{

struct _auto_node;
class AudioClip;

class ShiftAutomationDragHandler : public DragHandler
{
	public:
		ShiftAutomationDragHandler( Clip* clip, const Rect& rect, int x_, int y_ );
		~ShiftAutomationDragHandler();
		void OnDrag( int x, int y );
		void OnDrop( int x, int y );
	private:
		Rect m_outline;
		struct _auto_node* m_nodeA;
		struct _auto_node* m_nodeB;
		//bool m_firstA;
		//bool m_lastB;
		AudioClip* m_audioClip;
		struct _auto_node* m_nodesCopy;
		struct _auto_node* m_nodesOriginal;
		int m_node_number;
};

} /* namespace nle */

#endif /* _SHIFT_AUTOMATION_DRAG_HANDLER_H_ */

