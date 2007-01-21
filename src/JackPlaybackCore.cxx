/* JackPlaybackCore.cxx
 *
 *  Copyright (C) 2005 Richard Spindler <richard.spindler AT gmail.com>
 * 
 *  patch to support jackit.sf.net audio out and transport sync
 *   05/2006 Robin Gareus <robin AT gareus.org>
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
#include "JackPlaybackCore.H"
#include "IAudioReader.H"
#include "IVideoReader.H"
#include "IVideoWriter.H"
#include "Timeline.H"
#include "ErrorDialog/IErrorHandler.H"

#define VIDEO_DRIFT_LIMIT 2 //Calculate this based on frame size
#define FRAMES 4096

extern bool g_use_jack_transport;
extern bool g_scrub_audio;
extern bool g_seek_audio;

namespace nle
{

/*
 * static functions
 */

static void video_idle_callback( void* data )
{
	JackPlaybackCore* pc = (JackPlaybackCore*)data;
	pc->flipFrame();
}
static void timer_callback( void* data )
{
	JackPlaybackCore* pc = (JackPlaybackCore*)data;
	pc->checkPlayButton();
}


/*
 * private jack 
 */

static jack_client_t *jack_client = NULL;
static jack_port_t *output_port[2]; // stereo
static char jackid[16];
static jack_nframes_t jack_bufsiz = 64;
static jack_nframes_t jack_latency_comp = 0;

/*
 * The process callback for this JACK application is called in a
 * special realtime thread once for each audio cycle.
 */
int jack_callback (jack_nframes_t nframes, void *data)
{
	JackPlaybackCore* pc = (JackPlaybackCore*)data;
	void *outL, *outR;
	jack_position_t	jack_position;
	jack_transport_state_t ts;
	jack_nframes_t latencyL , latencyR;

	latencyL = jack_port_get_total_latency(jack_client,output_port[0]);
	latencyR = jack_port_get_total_latency(jack_client,output_port[1]);
	jack_latency_comp = max(latencyL,latencyR);

	outL = jack_port_get_buffer (output_port[0], nframes);
	outR = jack_port_get_buffer (output_port[1], nframes);
	jack_transport_query(jack_client, &jack_position);
	ts = jack_transport_query(jack_client, NULL);

	pc->jackreadAudio(outL, outR, ts, jack_position.frame, nframes);
	return(0);
}


/* when jack shuts down... */
void jack_shutdown(void *data)
{
	jack_client=NULL;
	cerr << "jack server shutdown." << endl;
	JackPlaybackCore* pc = (JackPlaybackCore*)data;
	pc->hardstop(); 
}

void close_jack(void)
{
	if (jack_client)
		jack_client_close (jack_client);
	jack_client=NULL;
}

int jack_connected()
{
	if (jack_client) return(1);
	return(0);
}

void connect_jackports()
{
	if (!jack_client) return;
	// TODO: skip auto-connect if user wishes so.
	char * port_name = NULL; // TODO: get from preferences 
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
		cerr << "already connected to jack." << endl;
		return;
	}

	int i = 0;
	do {
		snprintf(jackid,16,"ome-%i",i);
		// TODO : jack_client_open(..)
		jack_client = jack_client_new (jackid);
	} while (jack_client == 0 && i++<16);

	if (!jack_client) {
		cerr << "could not connect to jack." << endl;
		return;
	}	

	jack_on_shutdown (jack_client, jack_shutdown, data);
	jack_set_process_callback(jack_client,jack_callback,data);

	output_port[0] = jack_port_register (jack_client, "output-L", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
	output_port[1] = jack_port_register (jack_client, "output-R", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);

	if (!output_port[0] || !output_port[1]) {
		cerr << "no more jack ports availabe." << endl;
		close_jack();
		return;
	}
	jack_bufsiz= jack_get_buffer_size(jack_client);
	cout << "Jack Samplerate: " << jack_get_sample_rate(jack_client) << endl;
	if ( jack_get_sample_rate(jack_client) != 48000 ) {
		CLEAR_ERRORS();
		ERROR_DETAIL( "Jack is running with a samplerate other than 48000" );
		SHOW_ERROR( "Jack Soundoutput failed" );
		close_jack();
		return;
	}

	if (jack_bufsiz > FRAMES) { 
		CLEAR_ERRORS();
		ERROR_DETAIL( "please decrease jackd buffer size" );
		SHOW_ERROR( "Jack Soundoutput failed" );
		close_jack();
		return;
	}

	 jack_activate(jack_client); 
	 connect_jackports();
}

long jack_poll_frame (void)
{
	jack_position_t	jack_position;
	double		jack_time;
	long 		frame;

	if (!jack_client) return (-1);
	/* Calculate frame. */
	jack_transport_query(jack_client, &jack_position);
	jack_time = jack_position.frame / (double) jack_position.frame_rate;
	frame = (int) rint(1200.0 * jack_time );
	return(frame);
}

void jack_reposition(int vframe)
{
	jack_nframes_t frame= vframe * 48000/NLE_TIME_BASE;
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

jack_transport_state_t jack_poll_ts(void) {
	return (jack_transport_query(jack_client, NULL));
}







/*
 * simple playback core
 */

//JackPlaybackCore* g_simplePlaybackCore;
JackPlaybackCore::JackPlaybackCore( IAudioReader* audioReader, IVideoReader* videoReader, IVideoWriter* videoWriter )
	: m_audioReader(audioReader), m_videoReader(videoReader), m_videoWriter(videoWriter)
{
	g_playbackCore = this;
	m_active = false;
	open_jack(this); 
}

bool JackPlaybackCore::ok() { return jack_connected(); }

/* hardstop() use only for error abort - else use stop */
void JackPlaybackCore::hardstop() { m_active=false; } 

JackPlaybackCore::~JackPlaybackCore()
{
	if (jack_connected()) close_jack(); 
}

void suspend_idle_handlers(); //defined in IdleHandlers.cxx

void resume_idle_handlers();


void JackPlaybackCore::play()
{
	int scrublen = 3*1920;  // TODO: get from preferences :  scrub_freq = sample_rate/scrublen   
				// good values for scrub_freq are 1..50 Hz
				// scrub length is rounded up to next multiple of jack buffer size.
	if (m_active) return; 

	if (!jack_connected()) open_jack(this); 
	if (!jack_connected()) return;

	m_currentFrame = g_timeline->m_seekPosition;
	m_lastFrame = g_timeline->m_seekPosition;
	m_scrubpos = 0;
	m_scrubmax = (jack_bufsiz!=0)?(int)ceil((double)(scrublen/jack_bufsiz)):1;

	if (!g_use_jack_transport) {
		m_audioPosition = m_currentFrame * 48000 / NLE_TIME_BASE; 
	} else {
		jack_reposition(m_currentFrame);

		/* workaround: possible backwards seek.
		 *
		 * if we do not wait for the reposition (seek)
		 * to complete,  ome already sends few samples
		 * at the latest jack transport time, 
		 * before the seek reposition is done
		 * possibly resulting in a backwards seek.
		 */
		int spin = 1000000;
		while (abs(jack_poll_frame()-m_currentFrame) > 2 && spin-- > 0 );
	}
	if (g_use_jack_transport) jack_play();

	suspend_idle_handlers();
	m_active = true;
	Fl::add_timeout( 0.1, timer_callback, this );
	Fl::add_timeout( 0.04, video_idle_callback, this ); // looks like hardcoded 25fps 
}

void JackPlaybackCore::pause()
{
	if (!m_active) return;
	if (!g_use_jack_transport) return; 
	jack_transport_state_t ts = jack_poll_ts();
	if (ts == JackTransportRolling) jack_stop();
	else /* if (ts == JackTransportStopped) */ jack_play();
}

void JackPlaybackCore::stop()
{
	if (!m_active) return;
	resume_idle_handlers();
	m_active = false;
	if (jack_connected() && g_use_jack_transport) jack_stop();
}

void JackPlaybackCore::checkPlayButton()
{
	if (!m_active) return;
	Fl::repeat_timeout( 0.1, timer_callback, this );
}

void JackPlaybackCore::jackreadAudio( void *outL, void *outR, jack_transport_state_t ts, jack_nframes_t position, jack_nframes_t nframes )
{
	if (!m_active)  {
		memset(outR,0, sizeof (jack_default_audio_sample_t) * nframes);
		memset(outL,0, sizeof (jack_default_audio_sample_t) * nframes);
		return;
	}

	if (g_use_jack_transport) {
		if (position < jack_latency_comp || (!g_scrub_audio && ts != JackTransportRolling)) {
			// if transport is not rolling - remain silent.
			memset(outR,0, sizeof (jack_default_audio_sample_t) * nframes);
			memset(outL,0, sizeof (jack_default_audio_sample_t) * nframes);

			if (ts == JackTransportStarting) {
				m_audioReader->sampleseek(1, position<jack_bufsiz?0:position-jack_latency_comp); 
			}
			return;
		}

		position-=jack_latency_comp;

		if (g_scrub_audio && ts == JackTransportStopped) {
			position+=(m_scrubpos) * jack_bufsiz;
			m_scrubpos=(m_scrubpos+1)%(m_scrubmax);
		} else m_scrubpos=0;
		
		m_audioReader->sampleseek(1, position); // set absolute audio sample position
		m_audioPosition = position;
	}

	float stereobuf[(2*FRAMES)];  // combined stereo signals in one buffer from OME
	unsigned long rv = m_audioReader->fillBuffer(stereobuf, nframes);

	if (!g_use_jack_transport) {
		m_audioReader->sampleseek(0,rv); // set relative sample position;
		m_audioPosition += rv;
	}

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

void JackPlaybackCore::flipFrame()
{
	if ( !m_active ) {
		return;
	}
	if (jack_connected() && g_use_jack_transport ) {

		jack_transport_state_t ts = jack_poll_ts();
		m_currentFrame=jack_poll_frame();

		if( g_seek_audio  && ts == JackTransportStopped) {
			if (m_prevSeek != g_timeline->m_seekPosition && 
		   	    m_currentFrame != g_timeline->m_seekPosition )   {
		   		m_currentFrame=g_timeline->m_seekPosition;
		   		jack_reposition (m_currentFrame);
				g_timeline->m_playPosition = m_currentFrame ;
				m_nexttolastFrame=m_lastFrame=-1;
			}
		}

		m_prevSeek = g_timeline->m_seekPosition;
		g_timeline->m_seekPosition = m_currentFrame;

		if (m_nexttolastFrame==m_currentFrame) goto noflip;
		m_nexttolastFrame=m_lastFrame; // first flip loads this frame into bg buffer
		m_lastFrame=m_currentFrame; // this flip display this frame.

	} else if (jack_connected()) { // jack audio with local transport
		// FIXME: for larger jack buffer sizes, this can become inaccurate due to rounding
		// issues. -> do something similar as the portaudio drift sync...
		m_lastFrame = llrint(m_audioPosition * NLE_TIME_BASE / 48000);
	} 
	static frame_struct** fs = 0;
	if ( fs ) {
		m_videoWriter->pushFrameStack( fs );
	}
	fs = m_videoReader->getFrameStack( m_lastFrame );
noflip:	
	Fl::repeat_timeout( 0.04, video_idle_callback, this ); // more hardcoded framerates.
}


} /* namespace nle */
