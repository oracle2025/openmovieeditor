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

namespace nle
{

SplitCommand::SplitCommand( Clip* clip, int64_t position )
{
	m_track = clip->track()->num();
	m_clipNr1 = clip->id();
	m_clipNr2 = getClipId();
	m_position = position;
}
void SplitCommand::doo()
{
	Track* t = g_timeline->getTrack( m_track );
	Clip* c = t->getClip( m_clipNr1 );
	int mute = 0;
	if ( VideoClip* vc = dynamic_cast<VideoClip*>(c) ) {
		mute = vc->m_mute;
	}
	g_timeline->addFile( m_track, m_position, c->filename(), ( m_position - c->position() ) + c->trimA(), c->trimB(), mute, m_clipNr2 );
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
	VideoTrack* vt = dynamic_cast<VideoTrack*>(t);
	if ( vt ) { vt->reconsiderFadeOver(); }
}

} /* namespace nle */


