#ifndef _MOVE_DRAG_HANDLER_H_
#define _MOVE_DRAG_HANDLER_H_

#include "Rect.h"
#include "DragHandler.h"
namespace nle
{
class Clip;
class TimelineView;

class MoveDragHandler : public DragHandler
{
	public:
		MoveDragHandler( TimelineView *tlv,
				Clip *clip,
				int x, int y,
				const Rect& rect );
		~MoveDragHandler(){}
		void OnDrag( int x, int y );
		void OnDrop( int x, int y );
	private:
		Rect m_rect;
		int m_x;
		int m_y;
};

	
} /* namespace nle */

#endif /* _MOVE_DRAG_HANDLER_H_ */

