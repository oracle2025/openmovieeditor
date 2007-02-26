/*  AudioClip.cxx
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

#include "AudioClip2.H"


namespace nle
{
AudioClip::AudioClip( Track* track, int64_t position, IAudioFile* af, int id )
	: FilterClip( track, position, id )
{
	m_audioFile = af;
}
virtual AudioClip::~AudioClip()
{
	if ( m_audioFile ) {
		delete m_audioFile;
		m_audioFile = 0;
	}
}
int AudioClip::fillBufferRaw( float* output, unsigned long frames, int64_t position )
{
}
int AudioClip::fillBuffer( float* output, unsigned long frames, int64_t position )
{
}
virtual void AudioClip::reset()
{
	if ( m_audioFile ) {
		m_audioFile->seek( audioTrimA() );
	}
	m_lastSamplePosition = 0;
	FilterClip::reset();
}
int64_t AudioClip::trimA( int64_t trim )
{
	//TODO this is tricky
}
int64_t AudioClip::trimB( int64_t trim )
{
}

DragHandler* AudioClip::onMouseDown( Rect& rect, int x, int y, bool shift )
{
}

virtual int64_t AudioClip::audioTrimA()
{
}
virtual int64_t AudioClip::audioTrimB()
{
}
int64_t AudioClip::audioLength()
{
}


	
} /* namespace nle */
