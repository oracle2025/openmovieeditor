/*  SplitCommand.cxx
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

#include "SplitCommand.H"
#include "Timeline.H"
#include "VideoTrack.H"
#include "VideoClip.H"
#include "AudioClip.H"
#include "AudioFileFactory.H"
#include "TitleClip.H"

namespace nle
{

SplitCommand::SplitCommand( Clip* clip, int64_t position )
{
	m_automationsCount = 0;
	m_automationPoints = 0;
	m_track = clip->track()->num();
	m_clipNr1 = clip->id();
	m_clipNr2 = getClipId();
	m_position = position;
	m_length = clip->length();
	m_audioClip = 0;
	m_titleClip = 0;
/*	AudioClip* ac = dynamic_cast<AudioClip*>(clip);
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
	}*/
	m_data = clip->getClipData();
	TitleClip* tc = dynamic_cast<TitleClip*>(clip);
	if ( tc ) {
		m_titleClip = true;
	}
}
SplitCommand::~SplitCommand()
{
/*	if ( m_automationPoints ) {
		delete [] m_automationPoints;
		m_automationPoints = 0;
	}
	m_automationsCount = 0;*/
	if ( m_data ) {
		delete m_data;
	}
}
void SplitCommand::doo()
{
	Track* t = g_timeline->getTrack( m_track );
	Clip* c = t->getClip( m_clipNr1 );
	int mute = 0;
	if ( VideoClip* vc = dynamic_cast<VideoClip*>(c) ) {
		mute = vc->m_mute;
	}

	if ( m_audioClip ) {
		IAudioFile *af = AudioFileFactory::get( c->filename() );
		if ( !af ) { return; }
		int64_t trim = m_position - c->position();
		AudioClip* ac = new AudioClip( t, m_position, af, trim + c->trimA(), c->trimB(), m_clipNr2 );
/*		auto_node* autopoints = ac->getAutoPoints();
		auto_node* n = autopoints;
		int i = 0;
		int64_t last_x = 0;
		float last_y = m_automationPoints[0].y;
		while ( m_automationPoints[i].x < trim ) {
			last_x = m_automationPoints[i].x;
			last_y = m_automationPoints[i].y;
			i++;
		}
		n->x = 0;
		n->y = ( ( m_automationPoints[i].y - last_y ) * ( (float)(trim - last_x) / (float)(m_automationPoints[i].x - last_x) ) ) + last_y;
		n = n->next;
		n->x = m_automationPoints[i].x - trim;
		n->y = m_automationPoints[i].y;
		while ( i < m_automationsCount ) {
			n->next = new auto_node;
			n->next->next = 0;
			n->next->x = m_automationPoints[i].x - trim;
			n->next->y = m_automationPoints[i].y;
			n = n->next;
			i++;
		}*/
		g_timeline->addClip( m_track, ac );
	} else if ( m_titleClip ) {
		TitleClip* tc = new TitleClip( t, m_position, m_length - ( m_position - c->position() ) + c->trimA() - c->trimB(), m_clipNr2, m_data );
		g_timeline->addClip( m_track, tc );
	} else {
		g_timeline->addFile( m_track, m_position, c->filename(), ( m_position - c->position() ) + c->trimA(), c->trimB(), mute, m_clipNr2, m_length, m_data );
	}
	c->trimB( ( c->position() + c->length() ) - m_position );
	VideoTrack* vt = dynamic_cast<VideoTrack*>(t);
	if ( vt ) { vt->reconsiderFadeOver(); }
}
void SplitCommand::undo()
{
	Track* t = g_timeline->getTrack( m_track );
	Clip* c = t->getClip( m_clipNr2 );
	int64_t l = c->length();
	t->removeClip( c );
	delete c;
	c = t->getClip( m_clipNr1 );
	c->trimB( (-1) * l );

/*	if ( m_audioClip ) {
		AudioClip* ac = dynamic_cast<AudioClip*>(c);
		auto_node* autopoints = ac->getAutoPoints();
		auto_node* n;
		while ( ( n = (auto_node*)sl_pop( &autopoints ) ) ) {
			delete n;
		}
		n = new auto_node;
		autopoints = n;
		n->next = 0;
		n->x = m_automationPoints[0].x;
		n->y = m_automationPoints[0].y;
		for ( int i = 1; i < m_automationsCount; i++ ) {
			n->next = new auto_node;
			n->next->next = 0;
			n->next->x = m_automationPoints[i].x;
			n->next->y = m_automationPoints[i].y;
			n = n->next;
		}
		ac->setAutoPoints( autopoints );
		
	}*/
	
	VideoTrack* vt = dynamic_cast<VideoTrack*>(t);
	if ( vt ) { vt->reconsiderFadeOver(); }
}

} /* namespace nle */


