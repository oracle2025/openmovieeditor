/*  RemoveCommand.cxx
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


#include "globals.H"
#include "timeline/Clip.H"
#include "timeline/Track.H"
#include "Timeline.H"
#include "VideoTrack.H"
#include "RemoveCommand.H"
#include "AudioClip.H"
#include "VideoClip.H"
#include "AudioFileFactory.H"


namespace nle
{

RemoveCommand::RemoveCommand( Clip* clip )
{
	m_automationsCount = 0;
	m_automationPoints = 0;
	m_track = clip->track()->num();
	m_clip = clip->id();
	m_filename = clip->filename();
	m_trimA = clip->trimA();
	m_trimB = clip->trimB();
	m_position = clip->position();
	m_length = clip->length();
	m_audioClip = 0;
	AudioClip* ac = dynamic_cast<AudioClip*>(clip);
	if ( ac ) {
		m_audioClip = true;
		auto_node* n = ac->getAutoPoints();
		int i = 0;
		while ( n ) {
			n = n->next;
			i++;
		}
		m_automationsCount = i;
		m_automationPoints = new auto_node[i];
		n = ac->getAutoPoints();
		i = 0;
		while ( n ) {
			m_automationPoints[i] = *n;
			m_automationPoints[i].next = 0;
			n = n->next;
			i++;
		}
	} else {
		VideoClip* vc = dynamic_cast<VideoClip*>(clip);
		if ( vc ) { m_mute = vc->m_mute; }
	}
}
RemoveCommand::~RemoveCommand()
{
	if ( m_automationPoints ) {
		delete [] m_automationPoints;
		m_automationPoints = 0;
	}
	m_automationsCount = 0;
}
void RemoveCommand::doo()
{
	Track* t = g_timeline->getTrack( m_track );
	Clip* c = t->getClip( m_clip );
	t->removeClip( c );
	delete c;
}

void RemoveCommand::undo()
{
	Track* t = g_timeline->getTrack( m_track );
	Clip* c;
	if ( m_audioClip ) {
		IAudioFile *af = AudioFileFactory::get( m_filename );
		if ( !af ) { return; }
		AudioClip* ac = new AudioClip( t, m_position, af, m_trimA, m_trimB, m_clip );
		c = ac;
		/* add automations */
		auto_node* n = ac->getAutoPoints();
		n->x = m_automationPoints[0].x;
		n->y = m_automationPoints[0].y;
		n = n->next;
		n->x = m_automationPoints[1].x;
		n->y = m_automationPoints[1].y;
		for ( int i = 2; i < m_automationsCount; i++ ) {
			n->next = new auto_node;
			n->next->next = 0;
			n->next->x = m_automationPoints[i].x;
			n->next->y = m_automationPoints[i].y;
			n = n->next;
		}
		g_timeline->addClip( m_track, c );
	} else {
		g_timeline->addFile( m_track, m_position, m_filename, m_trimA, m_trimB, m_mute, m_clip, m_length );
	}
	VideoTrack* vt = dynamic_cast<VideoTrack*>(t);
	if ( vt ) { vt->reconsiderFadeOver(); }
}

} /* namespace nle */
