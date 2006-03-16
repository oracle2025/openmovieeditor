/*  AudioClipBase.cxx
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

#include "AudioClipBase.H"
#include "IAudioFile.H"

namespace nle
{

AudioClipBase::AudioClipBase( Track *track, int64_t position, IAudioFile* af )
	: Clip( track, position )
{
	m_audioFile = af;
	m_mute = false;
}
AudioClipBase::~AudioClipBase()
{
	if ( m_audioFile ) {
		delete m_audioFile;
		m_audioFile = 0;
	}
}

void AudioClipBase::reset()
{
	if ( m_audioFile ) {
		m_audioFile->seek( audioTrimA() );
	}
	m_lastSamplePosition = -256; // FIXME no hardcoded number!
}

int64_t AudioClipBase::maxAudioLength()
{
	return audioLength();
}
int64_t AudioClipBase::audioLength()
{
	if ( !m_audioFile ) {
		return 0;
	}
	return m_audioFile->length() - ( audioTrimA() + audioTrimB() );
}
int64_t AudioClipBase::realAudioLength()
{
	int64_t a;
	int64_t b;
	a = audioLength();
	b = maxAudioLength();
	return a > b ? b : a;
}

int AudioClipBase::fillBuffer( float* output, unsigned long frames, int64_t position )
{
	if ( m_mute ) {
		return 0;
	}
	unsigned int frames_written = 0;
	int64_t currentPosition = audioPosition();
	int64_t aLength = realAudioLength();
	int64_t trimA = audioTrimA();
	if ( !m_audioFile ) {
		return 0;
	}
	if ( currentPosition + aLength < position ) { return 0; }
	if ( currentPosition > position ) {
		unsigned long empty_frames = ( currentPosition - position )
				< frames ? ( currentPosition - position ) : frames;
		for ( unsigned long i = 0; i < frames * 2; i++ ) {
			//TODO eingentlich sollten nur empty_frames geschrieben werden
			output[i] = 0.0;
		}
		frames_written += empty_frames;
		if ( empty_frames == frames ) {
			return frames_written;
		}
	}
	if ( m_lastSamplePosition + frames != position ) {
		m_audioFile->seek( position - frames_written - currentPosition + trimA );
	}
	m_lastSamplePosition += frames;
	return frames_written + m_audioFile->fillBuffer(
			&output[frames_written], frames - frames_written
			);
}
string AudioClipBase::audioFilename()
{
	return m_audioFile->filename();
}

} /* namespace nle */
