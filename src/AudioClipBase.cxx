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
#include "sl/sl.h"
#include "AudioFilter.H"
#include "FilterFactory.H"

namespace nle
{

AudioClipBase::AudioClipBase( Track *track, int64_t position, IAudioFile* af, int id )
	: Clip( track, position, id )
{
	m_audioFile = af;
	m_mute = false;
	m_filters = 0;
}
AudioClipBase::~AudioClipBase()
{
	if ( m_audioFile ) {
		delete m_audioFile;
		m_audioFile = 0;
	}
	filter_stack* node;
	while ( ( node = (filter_stack*)sl_pop( &m_filters ) ) ) {
		delete node->filter;
		delete node;
	}

}

void AudioClipBase::reset()
{
	if ( m_audioFile ) {
		m_audioFile->seek( audioTrimA() );
	}
	m_lastSamplePosition = 0; // FIXME no hardcoded number!
	//This es especially dangerous, because it forces continous seeking when
	//rendereing <-- is this still true?
	for ( filter_stack* node = m_filters; node; node = node->next ) {
		node->filter->reset();
	}
}
int64_t AudioClipBase::trimA( int64_t trim )
{
	trim = Clip::trimA( trim );
	for ( filter_stack* node = m_filters; node; node = node->next ) {
		node->filter->trimA( trim );
	}
	return trim;
}

int64_t AudioClipBase::trimB( int64_t trim )
{
	trim = Clip::trimB( trim );
	for ( filter_stack* node = m_filters; node; node = node->next ) {
		node->filter->trimB( trim );
	}
	return trim;
}

AudioFilter* AudioClipBase::appendFilter( FilterFactory* factory )
{
	FilterBase* f = factory->get( this );
	AudioFilter* e = dynamic_cast<AudioFilter*>(f);
	if ( !e ) {
		delete f;
		return 0;
	}
	filter_stack* n = new filter_stack;
	n->next = 0;
	n->filter = e;
	m_filters = (filter_stack*)sl_unshift( m_filters, n );
	return e;
}
void AudioClipBase::pushFilter( FilterFactory* factory )
{
	FilterBase* f = factory->get( this );
	AudioFilter* e = dynamic_cast<AudioFilter*>(f);
	if ( !e ) {
		delete f;
		return;
	}
	filter_stack* n = new filter_stack;
	n->next = 0;
	n->filter = e;
	m_filters = (filter_stack*)sl_push( m_filters, n );
}

int64_t AudioClipBase::audioLength()
{
	if ( !m_audioFile ) {
		return 0;
	}
	return m_audioFile->length() - ( audioTrimA() + audioTrimB() );
}

int AudioClipBase::fillBufferForEnvelope( float* output, unsigned long frames, int64_t position )
{
	return AudioClipBase::fillBuffer( output, frames, position );
}
int AudioClipBase::fillBuffer( float* output, unsigned long frames, int64_t position )
{
	int result = fillBufferRaw( output, frames, position );
	filter_stack* node = m_filters;
	while ( node ) {
		node->filter->fillBuffer( output, frames, position );
		node = node->next;
	}
	return result;
}
int AudioClipBase::fillBufferRaw( float* output, unsigned long frames, int64_t position )
{
	if ( m_mute ) {
		return 0;
	}
	int64_t frames_written = 0;
	int64_t currentPosition = audioPosition();
	int64_t aLength = audioLength();
	int64_t trimA = audioTrimA();
	int64_t frames64 = frames;
	if ( !m_audioFile ) {
		return 0;
	}
	if ( currentPosition + aLength < position ) { return 0; }
	if ( currentPosition > position ) {
		int64_t empty_frames = ( currentPosition - position )
				< frames64 ? ( currentPosition - position ) : frames64;
		for ( unsigned long i = 0; i < frames64 * 2; i++ ) {
			//TODO eingentlich sollten nur empty_frames geschrieben werden
			output[i] = 0.0;
		}
		frames_written += empty_frames;
		if ( empty_frames == frames64 ) {
			return frames_written;
		}
	}
	if ( m_lastSamplePosition + frames64 != position ) {
		m_audioFile->seek( position + frames_written - currentPosition + trimA );
	}
	m_lastSamplePosition = position;
	return frames_written + m_audioFile->fillBuffer(
			&output[frames_written], frames64 - frames_written
			);
}
string AudioClipBase::audioFilename()
{
	return m_audioFile->filename();
}

} /* namespace nle */
