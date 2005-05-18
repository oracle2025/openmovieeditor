#include "AudioClip.H"
#include "AudioFileQT.H"
#include <string.h>
#include <iostream>

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
