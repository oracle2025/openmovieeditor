#ifndef _TRIM_DRAG_HANDLER_H_
#define _TRIM_DRAG_HANDLER_H_

#include "DragHandler.h"

namespace nle
{

class TrimDragHandler : public DragHandler
{
	public:
		TrimDragHandler( TimelineView *tlv, Clip *clip,
				int track,
				int left, int right,
				bool trimRight );
		void OnDrag( int x, int y );
		void OnDrop( int x, int y );
	private:
		int m_track, m_left, m_right;
		bool m_trimRight;
};

} /* namespace nle */


#endif /* _TRIM_DRAG_HANDLER_H_ */

