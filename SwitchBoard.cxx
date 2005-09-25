/*  SwitchBoard.cxx
 *
 *  Copyright (C) 2003 Richard Spindler <richard.spindler AT gmail.com>
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

#include <iostream>

#include "SwitchBoard.H"
#include "Timeline.H"
#include "TimelineView.H"

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
