#ifndef _DRAG_HANDLER_H_
#define _DRAG_HANDLER_H_

#include "Rect.h"

namespace nle
{
class Clip;
class TimelineView;

class DragHandler
{
	public:
		virtual ~DragHandler(){}
		virtual void OnDrag( int x, int y ) = 0;
		virtual void OnDrop( int x, int y ) = 0;
	protected:
		DragHandler( TimelineView *tlv, Clip *clip ) : m_tlv(tlv), m_clip(clip) {}
		TimelineView *m_tlv;
		Clip *m_clip;
};

	
} /* namespace nle */

#endif /* _DRAG_HANDLER_H_ */

