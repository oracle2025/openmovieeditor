/*  Clip.cxx
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

#include "Clip.H"

namespace nle
{

Clip::Clip( Track *track, int64_t position )
{
	m_position = position;
	m_track = track;
	m_trimA = 0;
	m_trimB = 0;
	AutomationPoint p( 100, 50 );
	m_automation.push_back(p);
	p = AutomationPoint( 10, 50 );
	m_automation.push_back(p);
}
void Clip::trimA( int64_t trim )
{
	m_position = m_position - m_trimA;
	m_trimA += trim;
	if ( m_trimA < 0 )
		m_trimA = 0;
	m_position = m_position + m_trimA;
}
void Clip::trimB( int64_t trim )
{
	m_trimB += trim;
	if ( m_trimB < 0 )
		m_trimB = 0;
}
AutomationPoint* Clip::getAutomationRect( int x, int y )
{
	std::list<AutomationPoint>::iterator k;
	for ( k = m_automation.begin(); k != m_automation.end(); k++ ) {
		AutomationPoint *current = &(*k);
		Rect t = current->getScreenRect( this );
		if ( t.inside( x, y ) ) {
			return current;
		}
	}
	return 0;
}

} /* namespace nle */
