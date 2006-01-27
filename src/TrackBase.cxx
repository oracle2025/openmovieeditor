/*  Track.cxx
 *
 *  Copyright (C) 2005 Richard Spindler <richard.spindler AT gmail.com>
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

#include "TrackBase.H"
#include "AudioClipBase.H"

namespace nle
{

TrackBase::TrackBase( int num )
	: Track( num )
{
}
int TrackBase::fillBuffer( float* output, unsigned long frames, int64_t position )
{
	unsigned long inc;
	unsigned long written = 0;
	unsigned long emptyItems = 0;
	float* incBuffer = output;
//	ASSERT(m_current)
	while ( m_current && m_current->clip->type() != CLIP_TYPE_VIDEO && m_current->clip->type() != CLIP_TYPE_AUDIO ) {
		m_current = m_current->next;
	}
	while( written < frames && m_current ) {
		inc = ((AudioClipBase*)(m_current->clip))->fillBuffer( incBuffer,
				 frames - written, position + written
				);
		written += inc;
		incBuffer += inc;
		if ( written < frames ) { m_current = m_current->next; }
		while ( m_current && m_current->clip->type() != CLIP_TYPE_VIDEO && m_current->clip->type() != CLIP_TYPE_AUDIO ) {
			m_current = m_current->next;
		}
	}
	if ( m_current == 0 ) {
		while( written < frames ) {
			*incBuffer = 0.0;
			written++;
			incBuffer++;
			emptyItems++;
		}
	}
	return written - emptyItems;
}

} /* namespace nle */
