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

#include <sl.h>

#include "AudioClip.H"
#include "IAudioFile.H"
#include "WavArtist.H"
#include "AudioClipArtist.H"
#include "EnvelopeClip.H"

namespace nle
{

AudioClip::AudioClip( Track *track, int64_t position, IAudioFile* af, int64_t trimA, int64_t trimB, int id )
	: AudioClipBase( track, position, af, id )
{
	m_trimA = trimA;
	m_trimB = trimB;
	g_wavArtist->add( af );
	m_artist = new AudioClipArtist( this );
	m_envelopeClip = new EnvelopeClip( this );
}
AudioClip::~AudioClip()
{
	delete m_artist;
	g_wavArtist->remove( m_audioFile->filename() );
	if ( m_envelopeClip ) {
		delete m_envelopeClip;
	}
}
auto_node* AudioClip::getAutoPoints()
{
	assert( m_envelopeClip );
	return m_envelopeClip->getAutoPoints();
}
void AudioClip::setAutoPoints( auto_node* a )
{
	assert( m_envelopeClip );
	m_envelopeClip->setAutoPoints( a );
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
void AudioClip::trimA( int64_t trim )
{
	if ( trim + m_trimA < 0 ) {
		trim = -m_trimA;
	}
	if ( length() - trim <= 0 || trim == 0 ) {
		return;
	}
	m_envelopeClip->trimA( trim );
	Clip::trimA( trim );
}
void AudioClip::trimB( int64_t trim )
{
	if ( trim + m_trimB < 0 ) {
		trim = -m_trimB;
	}
	if ( length() - trim <= 0 ) {
		return;
	}
	m_envelopeClip->trimB( trim );


	Clip::trimB( trim );
}
void AudioClip::reset()
{
	AudioClipBase::reset();
	m_envelopeClip->reset();
}
int AudioClip::fillBuffer( float* output, unsigned long frames, int64_t position )
{
	return m_envelopeClip->fillBuffer( output, frames, position );
}
int64_t AudioClip::fileLength()
{
	return m_audioFile->length();
}

} /* namespace nle */
