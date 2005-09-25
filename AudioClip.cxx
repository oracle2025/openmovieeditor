/*  AudioClip.cxx
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

#include <iostream>

#include "AudioClip.H"
#include "AudioFileQT.H"

namespace nle
{

AudioClip::AudioClip( Track *track, int64_t position, AudioFileQT* af )
	: Clip( track, position )
{
	m_audioFile = af;
}
AudioClip::~AudioClip()
{
	delete m_audioFile;
}
int64_t AudioClip::length()
{
	return m_audioFile->length() - ( m_trimA + m_trimB );
}
const char* AudioClip::filename()
{
	return m_audioFile->filename();
}
void AudioClip::reset()
{
	m_audioFile->seek( m_trimA );
	m_lastSamplePosition = -256; //FIXME no nummber!
}
int AudioClip::fillBuffer( float* output, unsigned long frames, int64_t position )
{
	unsigned int frames_written = 0;
	// float a = 0;
	// int *b = (int*)&a; //FIXME This is a goddamn evil hack
	if ( m_position + length() < position ) { return 0; }
	if ( m_position > position ) {
		unsigned long empty_frames = ( m_position - position ) < frames ? ( m_position - position ) : frames;
		//memset( (void*)output, *b, sizeof(float) * empty_frames * 2 );
		for ( unsigned long i = 0; i < frames * 2; i++ ) { //TODO eingentlich sollten nur empty_frames geschrieben werden
			output[i] = 0.0;
		}
		frames_written += empty_frames;
		if ( empty_frames == frames ) {
			return frames_written;
		}
	}
	if ( m_lastSamplePosition + frames != position ) {
		m_audioFile->seek( position - frames_written - m_position + m_trimA );
	}
	m_lastSamplePosition += frames;
	return frames_written + m_audioFile->fillBuffer(
			&output[frames_written], frames - frames_written
			);
}

} /* namespace nle */
