/*  AutomationRemoveCommand.cxx
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


#include "AutomationRemoveCommand.H"
#include "Timeline.H"
#include "AudioClipBase.H"
#include "timeline/Track.H"



namespace nle
{

AutomationRemoveCommand::AutomationRemoveCommand( Clip* clip, int node )
{
	m_node = node;
	m_clipNr = clip->id();
	m_track = clip->track()->num();

	AudioClipBase* ac = dynamic_cast<AudioClipBase*>(clip);
	auto_node* q = ac->getAutoPoints();
	for ( int i = 0; i < node && q; i++ ){
		q = q->next;
	}
	m_position = q->x;
	m_value = q->y;
	m_node = node - 1;
}

void AutomationRemoveCommand::doo()
{
	auto_node* p;
	auto_node* q;
	Track* t = g_timeline->getTrack( m_track );
	Clip* c = t->getClip( m_clipNr );
	AudioClipBase* ac = dynamic_cast<AudioClipBase*>(c);
	q = ac->getAutoPoints();
	for ( int i = 0; i < m_node && q; i++ ){
		q = q->next;
	}
	p = q->next;
	q->next = p->next;
	delete p;
}
void AutomationRemoveCommand::undo()
{
	auto_node* p = new auto_node;
	p->x = m_position;
	p->y = m_value;
	auto_node* q;
	Track* t = g_timeline->getTrack( m_track );
	Clip* c = t->getClip( m_clipNr );
	AudioClipBase* ac = dynamic_cast<AudioClipBase*>(c);
	q = ac->getAutoPoints();
	for ( int i = 0; i < m_node && q; i++ ){
		q = q->next;
	}
	p->next = q->next;
	q->next = p;
}


} /* namespace nle */
