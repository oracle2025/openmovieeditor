/* SimplePlaybackCore.cxx
 *
 *  Copyright (C) 2005 Richard Spindler <richard.spindler AT gmail.com>
 * 
 * 
 *  patch to support jackit.sf.net audio out and transport sync
 *   05/2006 Robin Gareus <robin AT gareus.org>
 *
 *  NOTES on jack patch:
 *   - backward seeks after EndOfFile don't work.
 *   - if you use jack only (no portaudio) - you can safely increase 
 *     FRAMES and FRAMES_PER_BUFFER (eg. 4096) in AudioFileQT.cxx 
 *     Timeline.cxx, and this file.
 *     
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

#include <jack/jack.h>
#include <jack/transport.h>
#include <math.h>

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

/*
 * portaudio
 */

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


/*
 * jackit 
 */

static jack_client_t *jack_client = NULL;
static jack_port_t *output_port[2]; // stereo
static char jackid[16];

/*
 * The process callback for this JACK application is called in a
 * special realtime thread once for each audio cycle.
 */
int jack_callback (jack_nframes_t nframes, void *data)
{
	SimplePlaybackCore* pc = (SimplePlaybackCore*)data;
	void *outL, *outR;
	jack_position_t	jack_position;
	jack_transport_state_t ts = jack_transport_query(jack_client, NULL);

	outL = jack_port_get_buffer (output_port[0], nframes);
	outR = jack_port_get_buffer (output_port[1], nframes);

	jack_transport_query(jack_client, &jack_position);

	if (ts != JackTransportRolling) {
		// no scrub mode, yet.
		// pc->jackreadAudio( outL, outR, jack_position.frame, 0);  // seek to jack

		// if transport is not rolling - remain silent.
		memset(outR,0, sizeof (jack_default_audio_sample_t) * nframes);
		memset(outL,0, sizeof (jack_default_audio_sample_t) * nframes);
		return(0);
	}

	pc->jackreadAudio( outL, outR, jack_position.frame, nframes);
	return(0);
}


/* when jack shuts down... */
void jack_shutdown(void *arg)
{
	jack_client=NULL;
	cerr << "jack server shutdown." << endl;
}

void close_jack(void)
{
	if (jack_client)
		jack_client_close (jack_client);
	jack_client=NULL;
}

int jack_connected() {
	if (jack_client) return(1);
	return(0);
}

void connect_jackports()  {
	if (!jack_client) return;
	char * port_name = NULL;
	int port_flags = JackPortIsInput;
	if (!port_name) port_flags |= JackPortIsPhysical;

	const char **found_ports = jack_get_ports(jack_client, port_name, NULL, port_flags);
	for (int i = 0; found_ports && found_ports[i] && i < 2; i++) {
		if (jack_connect(jack_client, jack_port_name(output_port[i]), found_ports[i])) {
			cerr << "can not connect to jack output" << endl;
		}
	}
}

void open_jack(void *data) 
{
	if (jack_client) {
//		cerr << "already connected to jack." << endl;
		return;
	}

	int i = 0;
	do {
		snprintf(jackid,16,"ome-%i",i);
		jack_client = jack_client_new (jackid);
	} while (jack_client == 0 && i++<16);

	if (!jack_client) {
		cerr << "could not connect to jack." << endl;
		return;
	}	

	jack_on_shutdown (jack_client, jack_shutdown, 0);
	jack_set_process_callback(jack_client,jack_callback,data);

	output_port[0] = jack_port_register (jack_client, "output-L", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
	output_port[1] = jack_port_register (jack_client, "output-R", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);

	if (!output_port[0] || !output_port[1]) {
		cerr << "no more jack ports availabe." << endl;
		close_jack();
		return;
	}

	jack_activate(jack_client);

	connect_jackports();
}

long jack_poll_frame (void) {
	jack_position_t	jack_position;
	double		jack_time;
	long 		frame;

	if (!jack_client) return (-1);
	/* Calculate frame. */
	jack_transport_query(jack_client, &jack_position);
	jack_time = jack_position.frame / (double) jack_position.frame_rate;
	frame = (int) floor(25.0 * jack_time );
	return(frame);
}

void jack_reposition(int vframe)
{
	jack_nframes_t frame= 48000/25 * vframe;
	if (jack_client)
		jack_transport_locate (jack_client, frame);
}

void jack_stop(void)
{
	if (jack_client)
		jack_transport_stop (jack_client);
}

void jack_play(void)
{
	if (jack_client)
		jack_transport_start (jack_client);
}


/*
 * simple playback core
 */

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
	open_jack(this);  // try to connect to jack

	if (jack_connected()) {
		m_currentFrame = g_timeline->m_seekPosition;
		m_lastFrame = g_timeline->m_seekPosition;
		jack_reposition(m_currentFrame);

		/* woraround: possible backwards seek.
		 *
		 * if we do not wait for the reposition (seek)
		 * to complete,  ome already sends few samples
		 * before the seek reposition is done
		 * at the latest jack transport time, 
		 * possibly 
		 * resulting in a backwards seek.
		 * the OME timeline audio playback supports
		 * backwards seek in the current sample only....
		 * so we better wait.
		 */
		int spin = 1000000;
		while (abs(jack_poll_frame()-m_currentFrame) > 2 && spin-- > 0 );
	} else {
		m_currentFrame = g_timeline->m_seekPosition;
		m_lastFrame = g_timeline->m_seekPosition;
		m_audioPosition = m_currentFrame * ( 48000 / 25 );
	}

	jack_play();

	if (jack_connected() || portaudio_start( 48000, FRAMES, this ) ) 
	{
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
	if (jack_connected()) {
		jack_stop();
		close_jack(); 
	} else portaudio_stop();
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

void SimplePlaybackCore::jackreadAudio( void *outL, void *outR, jack_nframes_t position, jack_nframes_t nframes )
{

	m_audioReader->sampleseek(1, position); // set absolute audio sample position

	if ( !m_active || nframes==0 )  return; 

	if (nframes >= FRAMES) { // FIXME: check  buffer size during open_jack(). 
		cerr << "Soundoutput : please decrease jackd buffer size :)"  << endl;
		exit (1);
	}

	float stereobuf[FRAMES];  // combined stereo signals in one buffer from OME
	unsigned long rv = m_audioReader->fillBuffer(stereobuf, nframes);

	if (rv != nframes) {
	//	cerr << "Soundoutput buffer underrun: " << rv << "/" << nframes << endl;
		// FIXME: clear only the missing part of the buffer or better:
		// fill in average missing piece...
		memset(outR,0, sizeof (jack_default_audio_sample_t) * nframes);
		memset(outL,0, sizeof (jack_default_audio_sample_t) * nframes);
	}
	
	// demux stereo buffer into the jack mono buffers.
	jack_default_audio_sample_t *jackbufL, *jackbufR;
	jackbufL= (jack_default_audio_sample_t*)outL;
	jackbufR= (jack_default_audio_sample_t*)outR;
	
	for (unsigned int i=0;i<nframes && i< rv;i++) {
		jackbufL[i]= stereobuf[2*i];
		jackbufR[i]= stereobuf[2*i+1];
	}
}

int SimplePlaybackCore::readAudio( float* output, unsigned long frames )
{
	unsigned long r = m_audioReader->fillBuffer( output, frames );
	m_audioReader->sampleseek(0,r); // set relative sample position;
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
	if (jack_connected() ) {
		m_currentFrame=jack_poll_frame();
//		if (m_lastFrame==m_currentFrame) goto noflip;
		m_lastFrame=m_currentFrame;
	} else { // portaudio
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
	}

	static frame_struct** fs = 0;
	if ( fs ) {
		m_videoWriter->pushFrameStack( fs );
	}
	fs = m_videoReader->getFrameStack( m_lastFrame );
//noflip:	
	Fl::repeat_timeout( 0.04, video_idle_callback, this );
}


} /* namespace nle */
