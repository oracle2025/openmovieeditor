/*  AutomationDuoMoveCommand.cxx
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

#include "AutomationDuoMoveCommand.H"
#include "Timeline.H"
#include "AudioClip.H"
#include "timeline/Track.H"


namespace nle
{

static auto_node* find_n( int n, auto_node* start )
{
	auto_node* p = start;
	for ( int i = 0; i < n && p; i++ ){
		p = p->next;
	}
	return p;
}

AutomationDuoMoveCommand::AutomationDuoMoveCommand( Clip* clip, int node1, int node2, float value )
{
	m_node1 = node1;
	m_node2 = node2;
	m_clipNr = clip->id();
	m_track = clip->track()->num();
	AudioClip* ac = dynamic_cast<AudioClip*>(clip);
	auto_node* p = find_n( node1, ac->getAutoPoints() );
	m_srcValue1 = p->y;
	p = find_n( node2, ac->getAutoPoints() );
	m_srcValue2 = p->y;
	m_dstValue = value;

}

void AutomationDuoMoveCommand::doo()
{
	Track* t = g_timeline->getTrack( m_track );
	Clip* c = t->getClip( m_clipNr );
	AudioClip* ac = dynamic_cast<AudioClip*>(c);
	auto_node* p = find_n( m_node1, ac->getAutoPoints() );
	p->y = m_dstValue;
	p = find_n( m_node2, ac->getAutoPoints() );
	p->y = m_dstValue;
}

void AutomationDuoMoveCommand::undo()
{
	Track* t = g_timeline->getTrack( m_track );
	Clip* c = t->getClip( m_clipNr );
	AudioClip* ac = dynamic_cast<AudioClip*>(c);
	auto_node* p = find_n( m_node1, ac->getAutoPoints() );
	p->y = m_srcValue1;
	p = find_n( m_node2, ac->getAutoPoints() );
	p->y = m_srcValue2;
}

} /* namespace nle */
