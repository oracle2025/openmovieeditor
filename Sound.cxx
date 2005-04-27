#include <iostream>
#include <portaudio.h>

#include "Sound.h"
#include "NextFrameListener.h"
#include "Timeline.h"
#include "SwitchBoard.h"

using namespace std;


//long audio_length
//int64_t video_duration
namespace nle
{

static PortAudioStream* stream;

static int callback( void *inputBuffer, void *outputBuffer,
		unsigned long framesPerBuffer, PaTimestamp outTime,
		void *userData )
{
	Sound* snd = (Sound*)userData;
	return framesPerBuffer != snd->fillBuffer( (float*)outputBuffer, framesPerBuffer );
}

Sound::Sound( NextFrameListener *nfl )
	: m_nfl(nfl)
{
}

Sound::~Sound()
{
}

void Sound::Play()
{
	m_tl = SwitchBoard::i()->timeline();
	m_soundSamples = 0;
	m_videoFrames = 0;

	PortAudioCallback *cb = (PortAudioCallback*)callback;
	PaError err;
	err = Pa_Initialize();
	if ( err != paNoError ) goto error;
	err = Pa_OpenDefaultStream( &stream, 0, 2, paFloat32, 48000, 256, 0, cb, this );
	if( err != paNoError ) goto error;
	err = Pa_StartStream( stream );
	if( err != paNoError ) goto error;
	cout << "Sound::Play" << endl;
	return;
	error:
	cerr << "Soundoutput failed: " << Pa_GetErrorText( err ) << endl;
	return;

}
void Sound::Stop()
{
	PaError err;
	err = Pa_StopStream( stream );
	if( err != paNoError ) goto error;
	err = Pa_CloseStream( stream );
	if( err != paNoError ) goto error;
	Pa_Terminate();
	cout << "Sound::Stop" << endl;
	return;
	error:
	cerr << "Portaudio Error: " << Pa_GetErrorText( err ) << endl;
	return;

}

int Sound::fillBuffer( float* output, unsigned long frames )
{
	//irgendwie den GUI Thread wecken wenn nächstes Bild fällig wird.
	// und wann ist das? nach 44100/25 (48000/29.97)Samples?
	/* audio_length, samplerate
	 * video_length, fps
	 */
	int res = m_tl->fillBuffer( output, frames );
	m_soundSamples += res;
	if ( m_soundSamples / ( 48000 / 29.97 ) > m_videoFrames ) {
		m_videoFrames++;
		m_nfl->nextFrame( m_videoFrames );
	}
	return res;
}


	
} /* namespace nle */
