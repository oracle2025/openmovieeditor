/*  FilterAddCommand.cxx
 *
 *  Copyright (C) 2007 Richard Spindler <richard.spindler AT gmail.com>
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

#include "FilterAddCommand.H"
#include "FilterClip.H"
#include "timeline/Track.H"
#include "FilterFactory.H"
#include "MainFilterFactory.H"
#include "Timeline.H"

#include <cassert>

namespace nle
{

FilterAddCommand::FilterAddCommand( Clip* clip, const char* identifier )
{
	m_identifier = identifier;
	m_clip = clip->id();
	m_track = clip->track()->num();
	m_filter = 0;
}

FilterAddCommand::~FilterAddCommand()
{
}
void FilterAddCommand::doo()
{
	Track* t = g_timeline->getTrack( m_track );
	Clip* bc = t->getClip( m_clip );
	FilterClip* c = dynamic_cast<FilterClip*>(bc);
	
	FilterFactory* f = g_mainFilterFactory->get( m_identifier.c_str() );
	assert( f );
	m_filter = c->pushFilter( f );
}
void FilterAddCommand::undo()
{
	Track* t = g_timeline->getTrack( m_track );
	Clip* bc = t->getClip( m_clip );
	FilterClip* c = dynamic_cast<FilterClip*>(bc);
	
	c->popFilter();
	m_filter = 0;
}

} /* namespace nle */
