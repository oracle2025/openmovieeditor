/*  Sound.cxx
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

#include <iostream>

#include "portaudio/portaudio.h"

#include "Sound.H"
#include "NextFrameListener.H"
#include "Timeline.H"
#include "SwitchBoard.H"
#include "globals.H"

using namespace std;


//long audio_length
//int64_t video_duration
namespace nle
{

static PortAudioStream* stream;
//static PaStream* stream;

static int callback( void *inputBuffer, void *outputBuffer,
		unsigned long framesPerBuffer, PaTimestamp outTime,
		void *userData )
//static int callback( const void *inputBuffer, void *outputBuffer,
//		unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo,
//		PaStreamCallbackFlags statusFlags,
//		void *userData )
{
	Sound* snd = (Sound*)userData;
	return ( framesPerBuffer != snd->fillBuffer( (float*)outputBuffer, framesPerBuffer ) );
	/*if ( framesPerBuffer == snd->fillBuffer( (float*)outputBuffer, framesPerBuffer ) ) {
		return paContinue;
	} else {
		return paComplete;
	}*/
}
Sound* g_sound;
Sound::Sound( NextFrameListener *nfl )
	: m_nfl(nfl)
{
	g_sound = this;
	m_playing = false;
}

Sound::~Sound()
{
}

void Sound::Play()
{
	if ( m_playing )
		return;
	m_tl = g_timeline;
	m_soundSamples = 0;
	m_videoFrames = 0;

	PortAudioCallback *cb = (PortAudioCallback*)callback;
	//PaStreamCallback *cb = (PaStreamCallback*)callback;
	PaError err;
	err = Pa_Initialize();
	cout << "Pa_Initialize done" << endl;
	if ( err != paNoError ) goto error;
	err = Pa_OpenDefaultStream( &stream, 0, 2, paFloat32, 48000, 256, 0, cb, this );
	//err = Pa_OpenDefaultStream( &stream, 0, 2, paFloat32, 48000, 256, cb, this );
	cout << "Pa_OpenDefaultStream done" << endl;
	if( err != paNoError ) goto error;
	err = Pa_StartStream( stream );
	cout << "Pa_StartStream done" << endl;
	if( err != paNoError ) goto error;
	cout << "Sound::Play" << endl;
	m_playing = true;
	return;
	error:
	cerr << "Soundoutput failed: " << Pa_GetErrorText( err ) << endl;
	return;

}
void Sound::Stop()
{
	if ( !m_playing )
		return;
	PaError err;
	err = Pa_StopStream( stream );
	if( err != paNoError ) goto error;
	err = Pa_CloseStream( stream );
	if( err != paNoError ) goto error;
	Pa_Terminate();
	m_playing = false;
	cout << "Sound::Stop" << endl;
	return;
	error:
	cerr << "Portaudio Error: " << Pa_GetErrorText( err ) << endl;
	return;

}

unsigned int Sound::fillBuffer( float* output, unsigned long frames )
{
	//irgendwie den GUI Thread wecken wenn nächstes Bild fällig wird.
	// und wann ist das? nach 44100/25 (48000/29.97)Samples?
	/* audio_length, samplerate
	 * video_length, fps
	 */
	int res = m_tl->fillBuffer( output, frames );
	m_soundSamples += res;
	//
	if ( m_soundSamples / ( 48000 / g_fps ) > m_videoFrames ) {
		m_videoFrames++;
		m_nfl->nextFrame( m_videoFrames );
	//	cout << "Frame: " << m_videoFrames << " Samples: " << m_soundSamples << endl;
	}
	return res;
}

bool Sound::stillPlaying()
{
	return Pa_StreamActive( stream ) == 1;
}

	
} /* namespace nle */
