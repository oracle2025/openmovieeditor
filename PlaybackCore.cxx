/* PlaybackCore.cxx
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

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>

#include <FL/Fl.H>

#include "portaudio/portaudio.h"
#include "PlaybackCore.H"
#include "IAudioReader.H"
#include "IVideoReader.H"
#include "IVideoWriter.H"

using namespace std;

#define FRAMES 256

#define DEFAULT_RB_SIZE ( 16384 * 5 )           /* ringbuffer size in frames */

namespace nle
{

static PortAudioStream* stream;

static int callback( void *input, void *output, unsigned long frames, PaTimestamp time, void* data )
{
	PlaybackCore* pc = (PlaybackCore*)data;
	unsigned long ret = pc->fillBuffer( (float*)output, frames );
	if ( frames != ret ) {
		cout << "Ret final: " << ret << endl;
	}
	return ( frames != ret );
}

static void gui( int p, void* data )
{
	PlaybackCore* pc = (PlaybackCore*)data;
	pc->flipScreen( p );
}

static void* video( void* data )
{
	PlaybackCore* pc = (PlaybackCore*)data;
	pc->videoThread();
	return 0;
}

static void* audio( void* data )
{
	PlaybackCore* pc = (PlaybackCore*)data;
	pc->audioThread();
	return 0;
}

PlaybackCore::PlaybackCore( IAudioReader* audioReader, IVideoReader* videoReader, IVideoWriter* videoWriter )
{
	m_audioReader = audioReader;
	m_videoReader = videoReader;
	m_videoWriter = videoWriter;
	m_active = false;
	m_rb = jack_ringbuffer_create( DEFAULT_RB_SIZE );
	
	m_soundSamples = 0;
	m_stop_playback = 0;
	m_stop_playback_gui = false;
	m_videoFrames = 0;
	m_videoFrames_protected = 0;
	m_xruns = 0;


	pthread_mutex_init( &m_video_lock, 0 );
	pthread_mutex_init( &m_audio_lock, 0 );
	pthread_cond_init( &m_audio_ready, 0 );
	pthread_cond_init( &m_video_ready, 0 );

	key_t key;
	union semun arg;
	key = ftok( "/openmovieeditor", 'E' );
	m_semid = semget( key, 2, 0600 | IPC_EXCL | IPC_CREAT );
	if ( !m_semid ) {
		perror( "PlaybackCore" );
	}
	arg.val = 1;
	if ( semctl( m_semid, 0, SETVAL, arg ) == -1 ) {
		perror( "PlaybackCore" );
	}
	if ( pipe( m_pipe ) == -1 ) {
		perror( "PlaybackCore" );
	}
	Fl::add_fd( m_pipe[0], FL_READ, gui, this );


	pthread_create( &m_video_thread, NULL, video, this );
	pthread_create( &m_audio_thread, NULL, audio, this );
}
PlaybackCore::~PlaybackCore()
{
	stop();

	pthread_cancel( m_video_thread );
	pthread_cancel( m_audio_thread );
	
	pthread_join( m_video_thread, NULL );
	pthread_join( m_audio_thread, NULL );

	Fl::remove_fd( m_pipe[0] );

	close( m_pipe[0] );
	close( m_pipe[1] );

	if ( m_rb ) {
		jack_ringbuffer_free( m_rb );
	}
	if ( m_semid ) {
		semctl( m_semid, 0, IPC_RMID );
	}

}
void PlaybackCore::play()
{
	if ( m_active ) {
		return;
	}
	//empty buffers
	cout << "PlaybackCore::play()" << endl;
	m_soundSamples = 0;
	m_stop_playback = 0;
	m_stop_playback_gui = false;
	m_videoFrames = 0;
	m_videoFrames_protected = 0;
	m_xruns = 0;
	m_active = false;

	PortAudioCallback *cb = (PortAudioCallback*)callback;
	PaError err;
	
	err = Pa_Initialize();
	if ( err != paNoError ) goto error;

	err = Pa_OpenDefaultStream( &stream, 0, 2, paFloat32, 48000, FRAMES, 0, cb, this );
	if ( err != paNoError ) goto error;

	err = Pa_StartStream( stream );
	if ( err != paNoError ) goto error;
	
	m_active = true;

	return;

error:
	cerr << "Soundoutput failed: " << Pa_GetErrorText( err ) << endl;
	return;
}
void PlaybackCore::stop()
{
	if ( !m_active ) {
		return;
	}
	cout << "XRuns: " << m_xruns << endl;

	m_active = false;
	
	PaError err;

	err = Pa_StopStream( stream );
	if( err != paNoError ) goto error;
	
	err = Pa_CloseStream( stream );
	if( err != paNoError ) goto error;
	
	Pa_Terminate();

	return;
	
error:
	cerr << "Portaudio Error: " << Pa_GetErrorText( err ) << endl;
	return;
}	

void PlaybackCore::flipScreen( int p )
{
	char buf;
	struct sembuf sb_enter = { 1, -1, 0 };
	struct sembuf sb_leave = { 0, 1, 0 };
	read( p, &buf, sizeof(buf) );
	if ( semop( m_semid, &sb_enter, 1 ) == -1 ) {
		perror( "flipScreen" );
		exit( 1 );
	}
	if ( m_stop_playback_gui ) {
	}

	m_videoWriter->pushFrame( m_fs );

	if ( semop( m_semid, &sb_leave, 1) == -1 ) {
		perror( "flipScreen" );
		exit( 1 );
	}
}

unsigned long PlaybackCore::fillBuffer( float* output, unsigned long frames )
{
	cout << "Ringbuffer read: " << ( frames * sizeof(float) * 2 ) << endl;
	unsigned long rt = jack_ringbuffer_read( m_rb, (char*)output, frames * sizeof(float) * 2 );
	rt = rt / sizeof(float) / 2;
	if ( rt < frames ) {
		for ( unsigned long i = rt * 2; i < (frames * 2); i++ ) {
			output[i] = 0.0;
		}
		m_xruns++;
	}
	cout << "trying to lock mutex" << endl;
	if ( pthread_mutex_trylock( &m_audio_lock ) == 0 ) {
		cout << "mutex locked in fillBuffer" << endl;
		pthread_cond_signal( &m_audio_ready );
		pthread_mutex_unlock( &m_audio_lock );
	}
	return rt;

}

void PlaybackCore::videoThread()
{
	struct sembuf sb_enter = { 0, -1, 0 };
	struct sembuf sb_leave = { 1, 1, 0 };
	char buf = 'b';
	int64_t videoFrame;
	pthread_setcanceltype( PTHREAD_CANCEL_ASYNCHRONOUS, NULL );
	while ( 1 ) {
		if ( semop( m_semid, &sb_enter, 1 ) == -1 ) {
			perror( "video" );
			exit( 1 );
		}
		
		pthread_mutex_lock( &m_video_lock );
		pthread_cond_wait( &m_video_ready, &m_video_lock );

		m_stop_playback_gui = m_stop_playback;
		
		videoFrame = m_videoFrames;
		
		pthread_mutex_unlock( &m_video_lock);
		
		m_fs = m_videoReader->getFrame( videoFrame );

		if ( semop( m_semid, &sb_leave, 1) == -1 ) {
			perror( "disk" );
			exit( 1 );
		}
		write( m_pipe[1], &buf, sizeof(buf) );
	}
}

void PlaybackCore::audioThread()
{
	unsigned long rt = FRAMES;
	float buffer[FRAMES * 2];
	pthread_setcanceltype( PTHREAD_CANCEL_ASYNCHRONOUS, NULL );
	pthread_mutex_lock( &m_audio_lock );
	while ( 1 ) {
		if ( ( jack_ringbuffer_write_space( m_rb ) >= ( sizeof(float) * FRAMES * 2 ) ) && rt == FRAMES ) {
			rt = m_audioReader->fillBuffer( buffer, FRAMES );
			cout << "Ringbuffer write: " << ( sizeof(float) * rt * 2 ) << endl;
			jack_ringbuffer_write( m_rb, (char*)buffer, sizeof(float) * rt * 2 );

			m_soundSamples += rt;
			if ( m_soundSamples / ( 48000 / 25 ) > m_videoFrames_protected ) {
				pthread_mutex_lock( &m_video_lock );
				
				m_videoFrames_protected++;
				m_videoFrames = m_videoFrames_protected;
				cout << "notifying Video: " << m_videoFrames << endl;

				pthread_cond_signal( &m_video_ready );
				pthread_mutex_unlock( &m_video_lock );
			}
			
		} else {
			cout << "waiting" << endl;
			pthread_cond_wait( &m_audio_ready, &m_audio_lock );
		}
	}
	pthread_mutex_unlock( &m_audio_lock ); // <= can't be reached
}	

} /* namespace nle */
