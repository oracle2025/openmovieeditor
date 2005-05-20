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
void AudioClip::reset()
{
	m_audioFile->seek( m_trimA );
}
int AudioClip::fillBuffer( float* output, unsigned long frames, int64_t position )
{
	unsigned int frames_written = 0;
	float a = 0;
	int *b = (int*)&a; //FIXME This is a goddamn evil hack
	if ( m_position + length() < position ) { return 0; }
	if ( m_position > position ) {
		memset( (void*)output, *b, sizeof(float) *
				( m_position - position ) < frames ? ( m_position - position ) : frames  );
		frames_written += ( m_position - position ) < frames ? ( m_position - position ) : frames;
	}
	return frames_written + m_audioFile->fillBuffer(
			&output[frames_written], frames - frames_written
			);
}

} /* namespace nle */
