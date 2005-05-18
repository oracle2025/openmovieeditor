#include <iostream>

#include "SwitchBoard.h"
#include "Timeline.h"
#include "TimelineView.h"

using namespace std;

namespace nle
{

//extern bool USING_AUDIO;

SwitchBoard::SwitchBoard()
{
	/* len = 1024
	 * width = z.B. 640
	 * m_zoom ??
	 * slider_size -> 0.0 -> 1.0
	 * slider_size = slider_size / 1025
	 * if slider_size == 1.0 -> len * m_zoom == width      !
	 * if slider_size == 0.5 -> len * m_zoom == 2 * width  !
	 * m_zoom = ( width / slider_size ) / len;
	 */
	m_zoom = 1.0; //0.2 -> 2.0
	m_timeline = 0;
}

void SwitchBoard::zoom( float zoom )
{
	m_zoom = zoom;
}
float SwitchBoard::zoom()
{
	/*if (USING_AUDIO) {
		return m_zoom / ( 48000 / 29.97 );
	} else {*/
		return m_zoom;
//	}
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
