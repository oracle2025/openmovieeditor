#include "AudioTrack.h"
#include "AudioFileQT.h"
#include "AudioClip.h"
#include <iostream>

namespace nle
{

AudioTrack::AudioTrack( int num )
	: Track( num )
{
}
AudioTrack::~AudioTrack()
{
}
void AudioTrack::add_audio( int64_t position, const char* filename )
{
	AudioFileQT *af = new AudioFileQT( filename );
	if ( !af->ok() ) {
		delete af;
		std::cerr << "Error: AudioTrack::add_audio" << std::endl;
		return;
	}
	m_clips.push_back( new AudioClip( this, position, af ) );
}
void AudioTrack::reset()
{
	m_current = m_clips.begin();
	Track::reset();
}
int AudioTrack::fillBuffer( float* output, unsigned long frames, int64_t position )
{
	unsigned long inc;
	unsigned long written = 0;
	unsigned long emptyItems = 0;
	float* incBuffer = output;
//	ASSERT(m_current)
	while( written < frames && m_current != m_clips.end() ) {
		inc = ((AudioClip*)(*m_current))->fillBuffer( incBuffer,
				 frames - written, position + written
				);
		written += inc;
		incBuffer += inc;
		if ( written < frames ) { m_current++; }
	}
	if ( m_current == m_clips.end() ) {
		while( written < frames ) {
			*incBuffer = 0;
			written++;
			incBuffer++;
			emptyItems++;
		}
	}
	return written - emptyItems;
}



} /* namespace nle */
