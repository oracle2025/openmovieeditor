#include "Sound.h"
#include <unistd.h>

namespace nle
{
static int callback( void *inputBuffer, void *outputBuffer,
		unsigned long framesPerBuffer, PaTimestamp outTime,
		void *userData )
{
	Sound* snd = (Sound*)userData;
	return snd->fillBuffer( (float*)outputBuffer, framesPerBuffer );
}

Sound::Sound()
{
	pipe( m_pipe );
}

Sound::~Sound()
{
}

Sound::Play() {}
Sound::Stop() {}

int Sound::fillBuffer( float* output, unsigned long frames )
{
	//irgendwie den GUI Thread wecken wenn nächstes Bild fällig wird.
	// und wann ist das? nach 44100/25 (48000/29.97)Samples?
	write( m_pipe[1], '0', 1 );
	/* audio_length, samplerate
	 * video_length, fps
	 */
}


	
} /* namespace nle */
