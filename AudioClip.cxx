/*  AudioClip.cxx
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

#include <iostream>

#include "AudioClip.H"
#include "IAudioFile.H"
#include "WavArtist.H"

namespace nle
{

AudioClip::AudioClip( Track *track, int64_t position, IAudioFile* af )
	: AudioClipBase( track, position, af )
{
	g_wavArtist->add( af );
}
AudioClip::~AudioClip()
{
	g_wavArtist->remove( m_audioFile->filename() );
}
int64_t AudioClip::length()
{
	return audioLength();
}
string AudioClip::filename()
{
	return audioFilename();
}
int64_t AudioClip::audioTrimA()
{
	return m_trimA;
}
int64_t AudioClip::audioTrimB()
{
	return m_trimB;
}
int64_t AudioClip::audioPosition()
{
	return position();
}

} /* namespace nle */
