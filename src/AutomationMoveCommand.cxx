/*  AutomationMoveCommand.cxx
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

/*
 * How to implement this right?
 * One could store the initial state of the present state in
 * AutomationDragHandler. And restore this before going into this Command.
 */

#include "AutomationMoveCommand.H"
#include "Timeline.H"
#include "AudioClipBase.H"
#include "timeline/Track.H"


namespace nle
{
static auto_node* find_n( int n, auto_node* start );
static auto_node* find_n( int n, int track, int clip )
{
	Track* t = g_timeline->getTrack( track );
	Clip* c = t->getClip( clip );
	AudioClipBase* ac = dynamic_cast<AudioClipBase*>(c);
	return find_n( n, ac->getAutoPoints() );
}
static auto_node* find_n( int n, auto_node* start )
{
	auto_node* p = start;
	for ( int i = 0; i < n && p; i++ ){
		p = p->next;
	}
	return p;
}

AutomationMoveCommand::AutomationMoveCommand( Clip* clip, int node, int64_t position, float value )
{
	m_node = node;
	m_clipNr = clip->id();
	m_track = clip->track()->num();
	AudioClipBase* ac = dynamic_cast<AudioClipBase*>(clip);
	auto_node* p = find_n( node, ac->getAutoPoints() );
	m_srcPos = p->x;
	m_srcVal = p->y;
	m_dstPos = position;
	m_dstVal = value;
}

void AutomationMoveCommand::doo()
{
	auto_node* p = find_n( m_node, m_track, m_clipNr );
	p->x = m_dstPos;
	p->y = m_dstVal;
}

void AutomationMoveCommand::undo()
{
	auto_node* p = find_n( m_node, m_track, m_clipNr );
	p->x = m_srcPos;
	p->y = m_srcVal;
}

} /* namespace nle */
