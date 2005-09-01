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
#include <iostream>

using namespace std;

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
#define SNAP_TOLERANCE 10
bool is_in_tolerance( int64_t A, int64_t B, unsigned int tolerance )
{
	return ( B - tolerance <= A && B + tolerance >= A );
}
int64_t Track::getSnapA( Clip* clip, int64_t A )
{
	std::list< Clip* >::iterator j;
	for ( j = m_clips.begin(); j != m_clips.end(); j++ ) {
		Clip* current = *j;
		int64_t B = current->B();
		if ( current != clip && is_in_tolerance( A, B, (unsigned int)(SNAP_TOLERANCE * stretchFactor()) ) ) {
			return B + 1;
		}
	}
	return A;
}
int64_t Track::getSnapB( Clip* clip, int64_t B )
{
	std::list< Clip* >::iterator j;
	for ( j = m_clips.begin(); j != m_clips.end(); j++ ) {
		Clip* current = *j;
		int64_t A = current->position();
		if ( current != clip && is_in_tolerance( B + clip->length(), A, (unsigned int)(SNAP_TOLERANCE * stretchFactor()) ) ) {
			return A - 1 - clip->length();
		}
	}
	return B;
}

	
} /* namespace nle */
