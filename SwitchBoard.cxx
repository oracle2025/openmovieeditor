#include "SwitchBoard.h"
#include "Timeline.h"
#include "TimelineView.h"

namespace nle
{

SwitchBoard::SwitchBoard()
{
	m_zoom = 1.0;
	m_timeline = 0;
}

void SwitchBoard::zoom( float zoom )
{
	m_zoom = zoom;
}
float SwitchBoard::zoom()
{
	return m_zoom;
}
void SwitchBoard::timeline( Timeline* tl )
{
	m_timeline = tl;
}
void SwitchBoard::timelineView( TimelineView* tlv )
{
	m_tlv = tlv;
}
Timeline* SwitchBoard::timeline()
{
	return m_timeline;
}
void SwitchBoard::move_cursor()
{
	m_tlv->move_cursor( m_timeline->m_playPosition );
}

} /* namespace nle */
