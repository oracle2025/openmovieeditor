#include "AudioTrack.h"

namespace nle
{

AudioTrack::AudioTrack( int num )
	: Track( num )
{
}
AudioTrack::~AudioTrack()
{
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
	while( written < frames && m_current != m_clips.end() ) {
		inc = (AudioClip*)(*m_current)->fillBuffer( incBuffer,
				position + written, count -written
				);
		written += inc;
		incBuffer += inc;
		if ( written < count ) { m_current++; }
	}
	if ( m_current == m_clips.end() ) {
		while( written < count ) {
			*incBuffer = 0;
			written++;
			incBuffer++;
			emptyItems++;
		}
	}
	return written - emptyItems;
}



} /* namespace nle */
