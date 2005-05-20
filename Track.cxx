/*  Track.cxx
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

#include <algorithm>

#include "Track.H"
#include "Clip.H"

namespace nle
{


Track::Track( int num )
{
	m_num = num;
}
Track::~Track()
{
	Clip *clip;
	while( m_clips.size() ) {
		clip = *m_clips.end();
		m_clips.pop_back();
		delete clip;
	}
}
void Track::add( Clip* clip )
{
	m_clips.push_back( clip );
}
void Track::remove( Clip* clip )
{
	std::list< Clip* >::iterator j;
	for ( j = m_clips.begin(); j != m_clips.end(); j++ ) {
		if ( *j == clip ) {
			m_clips.erase(j);
			return;
		}
	}
}
void reset_helper( Clip* clip ) { clip->reset(); }
void Track::reset()
{
	for_each( m_clips.begin(), m_clips.end(), reset_helper );
}
	
} /* namespace nle */
