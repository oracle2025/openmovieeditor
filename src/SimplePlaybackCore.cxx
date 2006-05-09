/* SimplePlaybackCore.cxx
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

#include <unistd.h>
#include <pthread.h>

#include <FL/Fl.H>
#include <FL/Fl_Button.H>

#include "global_includes.H"
#include "globals.H"
#include "portaudio/portaudio.h"
#include "SimplePlaybackCore.H"
#include "IAudioReader.H"
#include "IVideoReader.H"
#include "IVideoWriter.H"
#include "Timeline.H"
 
#define FRAMES 256
#define VIDEO_DRIFT_LIMIT 2

namespace nle
{

pthread_mutex_t condition_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  condition_cond  = PTHREAD_COND_INITIALIZER;

static int portaudio_callback( void *input, void *output, unsigned long frames, PaTimestamp time, void* data )
{
	SimplePlaybackCore* pc = (SimplePlaybackCore*)data;
	return pc->readAudio( (float*)output, frames );
}

static void video_idle_callback( void* data )
{
	SimplePlaybackCore* pc = (SimplePlaybackCore*)data;
	pc->flipFrame();
}
static void timer_callback( void* data )
{
	SimplePlaybackCore* pc = (SimplePlaybackCore*)data;
	pc->checkPlayButton();
}

static PortAudioStream* g_stream;
static bool portaudio_start( int rate, int frames, void* data )
{
	PortAudioCallback *cb = (PortAudioCallback*)portaudio_callback;
	PaError err;
	
	err = Pa_Initialize();
	if ( err != paNoError ) goto error;

	err = Pa_OpenDefaultStream( &g_stream, 0, 2, paFloat32, rate, frames, 0, cb, data );
	if ( err != paNoError ) goto error;

	err = Pa_StartStream( g_stream );
	if ( err != paNoError ) goto error;
	
	return 1;
error:
	cerr << "Soundoutput failed: " << Pa_GetErrorText( err ) << endl;
	return 0;
}
static bool portaudio_stop()
{
	PaError err;

	err = Pa_StopStream( g_stream );
	if( err != paNoError ) goto error;
	
	err = Pa_CloseStream( g_stream );
	if( err != paNoError ) goto error;
	
	Pa_Terminate();

	return 1;
	
error:
	cerr << "Portaudio Error: " << Pa_GetErrorText( err ) << endl;
	return 0;
}

SimplePlaybackCore* g_simplePlaybackCore;
SimplePlaybackCore::SimplePlaybackCore( IAudioReader* audioReader, IVideoReader* videoReader, IVideoWriter* videoWriter )
	: m_audioReader(audioReader), m_videoReader(videoReader), m_videoWriter(videoWriter)
{
	g_simplePlaybackCore = this;
	m_active = false;
}
SimplePlaybackCore::~SimplePlaybackCore()
{
}
void SimplePlaybackCore::play()
{
	if ( m_active ) {
		return;
	}
	m_currentFrame = g_timeline->m_seekPosition;
	m_lastFrame = g_timeline->m_seekPosition;
	m_audioPosition = m_currentFrame * ( 48000 / 25 );
	if ( portaudio_start( 48000, FRAMES, this ) ) {
		m_active = true;
		Fl::add_timeout( 0.1, timer_callback, this );
		Fl::add_timeout( 0.04, video_idle_callback, this );
	}
}
void SimplePlaybackCore::stop()
{
	if ( !m_active ) {
		return;
	}
	m_active = false;
	portaudio_stop();
}
void SimplePlaybackCore::checkPlayButton()
{
	if ( !m_active ) {
		return;
	}
	if ( !Pa_StreamActive( g_stream ) ) {
		stop();
		g_playButton->label( "@>" );
		g_firstButton->activate();
		g_lastButton->activate();
		g_forwardButton->activate();
		g_backButton->activate();
	} else {
		Fl::repeat_timeout( 0.1, timer_callback, this );
	}
}
int SimplePlaybackCore::readAudio( float* output, unsigned long frames )
{
	unsigned long r = m_audioReader->fillBuffer( output, frames );
	m_audioPosition += r;
	pthread_mutex_lock( &condition_mutex );
	if ( m_audioPosition / ( 48000 / 25 ) > m_currentFrame ) {
		m_currentFrame++;
		pthread_cond_signal( &condition_cond );
	}
	pthread_mutex_unlock( &condition_mutex );
	return r != frames;
}
void SimplePlaybackCore::flipFrame()
{
	if ( !m_active ) {
		return;
	}
	m_lastFrame++;
	pthread_mutex_lock( &condition_mutex );
	int64_t diff = m_lastFrame - m_currentFrame;
	if ( abs( diff ) > VIDEO_DRIFT_LIMIT ) {
		if ( diff > 0 ) {
			while( ( m_lastFrame - m_currentFrame ) > VIDEO_DRIFT_LIMIT ) {
				pthread_cond_wait( &condition_cond, &condition_mutex );
			}
		} else {
			m_lastFrame++;
		}
	}
	pthread_mutex_unlock( &condition_mutex );
	static frame_struct** fs = 0;
	if ( fs ) {
		m_videoWriter->pushFrameStack( fs );
	}
	fs = m_videoReader->getFrameStack( m_lastFrame );
	Fl::repeat_timeout( 0.04, video_idle_callback, this );
}

} /* namespace nle */
