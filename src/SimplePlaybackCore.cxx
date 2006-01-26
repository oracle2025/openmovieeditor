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
#include <fcntl.h>
#include <cerrno>

#include <FL/Fl.H>

#include "global_includes.H"
#include "globals.H"
#include "portaudio/portaudio.h"
#include "SimplePlaybackCore.H"
#include "IAudioReader.H"
#include "IVideoReader.H"
#include "IVideoWriter.H"
#include "Timeline.H"
 
#define FRAMES 256

namespace nle
{

typedef struct _audio_chunk {
	float buffer[2 * FRAMES];
	int64_t timestamp;
} audio_chunk;

static int portaudio_callback( void *input, void *output, unsigned long frames, PaTimestamp time, void* data )
{
	SimplePlaybackCore* pc = (SimplePlaybackCore*)data;
	return pc->readAudio( (float*)output, frames );
}

static void audio_pipe_callback( int p, void* data )
{
	SimplePlaybackCore* pc = (SimplePlaybackCore*)data;
	pc->fillBuffer( p );
}
static void video_idle_callback( void* data )
{
	SimplePlaybackCore* pc = (SimplePlaybackCore*)data;
	pc->flipFrame();
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
	if ( pipe( m_pipe ) == -1 ) {
		perror( "SimplePlaybackCore" );
	}
//	fcntl( m_pipe[0], F_SETFL, O_NONBLOCK );
//	fcntl( m_pipe[1], F_SETFL, O_NONBLOCK );
}
SimplePlaybackCore::~SimplePlaybackCore()
{
}
void SimplePlaybackCore::play()
{
	if ( m_active ) {
		return;
	}
	m_videoPosition = g_timeline->m_seekPosition;
	m_nextVideoFrame = g_timeline->m_seekPosition;
	m_audioPosition = m_videoPosition * ( 48000 / 25 );
//fill pipe
/*	while ( true ) {
		static audio_chunk chunk;
		chunk.timestamp = m_audioPosition;
		m_audioPosition += m_audioReader->fillBuffer( chunk.buffer, FRAMES );
		ssize_t r = write( m_pipe[1], chunk.buffer, sizeof(chunk.buffer) );
		if ( r < sizeof(chunk.buffer) || errno == EAGAIN ) {
			break;
		}
	} */
	Fl::add_fd( m_pipe[0], FL_READ, audio_pipe_callback, this );
	Fl::add_check( video_idle_callback, this );
	if ( portaudio_start( 48000, FRAMES, this ) ) {
		m_active = true;
	}
}
void SimplePlaybackCore::stop()
{
	if ( !m_active ) {
		return;
	}
	m_active = false;
	Fl::remove_fd( m_pipe[1] );
	portaudio_stop();
//empty pipe
	/*float b[FRAMES];
	while ( read( m_pipe[0], b, sizeof(b) ) == sizeof(b) ) {
		// empty
	}*/
	
}
int SimplePlaybackCore::readAudio( float* output, unsigned long frames )
{
	static char c = '\0';
	m_audioPosition += m_audioReader->fillBuffer( output, frames );
	if ( m_audioPosition / ( 48000 / 25 ) > m_videoPosition ) {
		m_nextVideoFrame++;
		write( m_pipe[1], &c, sizeof(char) );
	}
	return 0;
/*	static audio_chunk chunk;
	int rt;
	if ( ( rt = read( m_pipe[0], output, sizeof(chunk.buffer) ) ) < sizeof(chunk.buffer) ) {
		cout << rt << endl;
		perror( "readAudio" );
		return 1;
	}
	return 0;*/
}
void SimplePlaybackCore::fillBuffer( int p )
{
	static char c;
	read( m_pipe[0], &c, sizeof(char) );
	m_nextVideoFrame++;
//	Fl::add_idle( video_idle_callback, this );
	
/*	if ( !m_active ) {
		return;
	}
	static audio_chunk chunk;
	chunk.timestamp = m_audioPosition;
	m_audioPosition += m_audioReader->fillBuffer( chunk.buffer, FRAMES );
	if ( write( p, chunk.buffer, sizeof(chunk.buffer) ) < sizeof(chunk.buffer) ) {
		perror( "fillBuffer" );
	}
	if ( m_audioPosition / ( 48000 / 25 ) > m_videoPosition ) {
		m_nextVideoFrame++;
		Fl::add_idle( video_idle_callback, this );
	}*/
}
void SimplePlaybackCore::flipFrame()
{
//	Fl::remove_idle( video_idle_callback, this );
	if ( m_nextVideoFrame == m_videoPosition ) {
		return;
	}
	frame_struct** fs = m_videoReader->getFrameStack( m_nextVideoFrame );
	m_videoPosition = m_nextVideoFrame;
	m_videoWriter->pushFrameStack( fs );
}

} /* namespace nle */
