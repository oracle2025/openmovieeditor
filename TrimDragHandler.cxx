#include <FL/Fl.H>
#include <FL/Fl_Widget.H>
#include <FL/Fl_Window.H>
#include <FL/fl_draw.H>

#include "TrimDragHandler.H"
#include "TimelineView.H"

namespace nle
{

TrimDragHandler::TrimDragHandler( TimelineView *tlv, Clip *clip,
		int track, int left, int right,
		bool trimRight )
	: DragHandler(tlv, clip), m_track(track), m_left(left),
	m_right(right), m_trimRight(trimRight)
{
}
void TrimDragHandler::OnDrag( int x, int y )
{
	m_tlv->window()->make_current();
	fl_overlay_rect( x,
			m_tlv->y() + TRACK_SPACING + (TRACK_SPACING + TRACK_HEIGHT)*m_track,
			1, TRACK_HEIGHT );
}
void TrimDragHandler::OnDrop( int x, int y )
{
	m_tlv->window()->make_current();
	fl_overlay_clear();
	m_tlv->trim_clip( m_clip, x, m_trimRight );
}
	
} /* namespace nle */
