#include <FL/Fl.H>
#include <FL/Fl_Widget.H>
#include <FL/Fl_Window.H>
#include <FL/fl_draw.H>

#include "MoveDragHandler.h"
#include "TimelineView.h"


namespace nle
{

class Track;

MoveDragHandler::MoveDragHandler( TimelineView *tlv,
		Clip *clip, int x, int y,
		const Rect& rect )
	: DragHandler(tlv, clip)
{
	m_rect = rect;
	m_x = x;
	m_y = y;
}
void MoveDragHandler::OnDrag( int x, int y )
{
	Rect tmp( m_tlv->x() + x - ( m_x - m_rect.x ),
			m_tlv->y() + y - ( m_y - m_rect.y ),
			m_rect.w, m_rect.h );
	Track *tr = m_tlv->get_track( x, y );
	if ( tr ) {
		Rect tr_rect = m_tlv->get_track_rect( tr );
		tmp.y = m_tlv->y() + tr_rect.y;
	}
	m_tlv->window()->make_current();
	fl_overlay_rect( tmp.x, tmp.y, tmp.w, tmp.h );
}
void MoveDragHandler::OnDrop( int x, int y )
{
	m_tlv->window()->make_current();
	fl_overlay_clear();
	m_tlv->move_clip( m_clip, x, y, m_x - m_rect.x );
}


} /* namespace nle */
