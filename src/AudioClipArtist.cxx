/*  AudioClipArtist.cxx
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

#include "AudioClipArtist.H"
#include "AudioClip.H"
#include "WavArtist.H"

namespace nle
{

AudioClipArtist::AudioClipArtist( AudioClip* clip )
	: m_clip( clip )
{
}
AudioClipArtist::~AudioClipArtist()
{
}
void AudioClipArtist::render( Rect& rect, int64_t, int64_t )
{
	g_wavArtist->render( m_clip->filename(), rect, m_clip->trimA(), m_clip->trimA() + m_clip->length() );
}

	
} /* namespace nle */
