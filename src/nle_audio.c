/* nle_audio.c
 *
 *  Copyright (C) 2006 Richard Spindler <richard.spindler AT gmail.com>
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

#include <stdio.h>
#include <stdlib.h>
#include <alsa/asoundlib.h>

#include "nle_audio.h"

typedef struct a_handle_s {
	snd_pcm_t *playback_handle;
	snd_pcm_hw_params_t *hw_params;
} a_handle;

static unsigned int rate = 48000;
//static unsigned int buffer_time = 500000;
//static unsigned int period_time = 100000;
static unsigned int buffer_time = 500000;
static unsigned int period_time = 100000;
#define FRAMES 16384

int err;

nle_audio_handle prepare_audio()
{
	a_handle* h = (a_handle*)malloc( sizeof(a_handle) );
	
	if ( ( err = snd_pcm_open( &(h->playback_handle), "default", SND_PCM_STREAM_PLAYBACK, 0 ) ) < 0 ) {
		fprintf( stderr, "cannot open audio device %s (%s)\n",
				"default",
				snd_strerror( err ) );
		exit( 1 );
	}
	if ( ( err = snd_pcm_hw_params_malloc( &(h->hw_params) ) ) < 0 ) {
		fprintf( stderr, "cannot allocate hardware parameter structure (%s)\n",
				snd_strerror( err ) );
		exit( 1 );
	}
	err = snd_pcm_hw_params_any( h->playback_handle, h->hw_params );
	if (err < 0) {
		printf("Broken configuration for playback: no configurations available: %s\n", snd_strerror(err));
		exit( 1 );
	}
	err = snd_pcm_hw_params_set_rate_resample( h->playback_handle, h->hw_params, 1);
        if (err < 0) {
                printf("Resampling setup failed for playback: %s\n", snd_strerror(err));
		exit( 1 );
        }

	if ( ( err = snd_pcm_hw_params_set_access( h->playback_handle, h->hw_params, SND_PCM_ACCESS_RW_INTERLEAVED ) ) < 0 ) {
		fprintf ( stderr, "cannot set access type (%s)\n",
				snd_strerror( err ) );
		exit ( 1 );
	}
	if ( ( err = snd_pcm_hw_params_set_format( h->playback_handle, h->hw_params, /* SND_PCM_FORMAT_S16_LE */SND_PCM_FORMAT_FLOAT_LE ) ) < 0 ) {
		fprintf( stderr, "cannot set sample format (%s)\n",
				snd_strerror( err ) );
		exit( 1 );
	}
	int dir=0;
	if ( ( err = snd_pcm_hw_params_set_rate_near( h->playback_handle, h->hw_params, &rate, &dir ) ) < 0 ) {
		fprintf( stderr, "cannot set sample rate (%s)\n",
				snd_strerror( err ) );
		exit( 1 );
	}
	if ( ( err = snd_pcm_hw_params_set_channels( h->playback_handle, h->hw_params, 2 ) ) < 0 ) {
		fprintf( stderr, "cannot set channel count (%s)\n",
				snd_strerror( err ) );
		exit( 1 );
	}
	err = snd_pcm_hw_params_set_buffer_time_near( h->playback_handle, h->hw_params, &buffer_time, &dir);
	if (err < 0) {
		printf("Unable to set buffer time %i for playback: %s\n", buffer_time, snd_strerror(err));
		exit( 1 );
	}

	err = snd_pcm_hw_params_set_period_time_near( h->playback_handle, h->hw_params, &period_time, &dir);
	if (err < 0) {
		printf("Unable to set period time %i for playback: %s\n", period_time, snd_strerror(err));
		exit( 1 );
	}

	snd_pcm_uframes_t size;
	err = snd_pcm_hw_params_get_buffer_size(h->hw_params, &size);
        if (err < 0) {
                printf("Unable to get buffer size for playback: %s\n", snd_strerror(err));
		exit( 1 );
        }
	printf( "Buffer Size: %lu\n", size);


	if ( ( err = snd_pcm_hw_params( h->playback_handle, h->hw_params ) ) < 0 ) {
		fprintf( stderr, "cannot set parameters (%s)\n",
				snd_strerror( err ) );
		exit( 1 );
	}
	snd_pcm_hw_params_free( h->hw_params );

//	if ( ( err = snd_pcm_prepare( h->playback_handle ) ) < 0 ) {
//		fprintf( stderr, "cannot prepare audio interface for use (%s)\n",
//				snd_strerror( err ) );
//		exit( 1 );
//	}
	
//	float output[256*2] = {0};
//	if ( ( err = snd_pcm_writei( h->playback_handle, output, 256 ) ) != 256 ) {
//		fprintf( stderr, "write to audio interface fails (%s)\n",
//				snd_strerror( err ) );
//		exit( 1 );
//	}
	int count = snd_pcm_poll_descriptors_count( h->playback_handle );
	printf( "Poll Descriptors: %d\n", count );

//	if ( ( err = snd_pcm_pause( h->playback_handle, 1 ) ) < 0 ) {
//		fprintf( stderr, "cannot pause audio interface (%s)\n",
//				snd_strerror( err ) );
//		exit( 1 );
//	}
	return h;
}
void end_audio( nle_audio_handle h )
{
	a_handle* handle = (a_handle*)h;
	snd_pcm_close( handle->playback_handle );
	free( h );
}
void play_audio( nle_audio_handle h )
{
	a_handle* handle = (a_handle*)h;
	if ( SND_PCM_STATE_SETUP == snd_pcm_state( handle->playback_handle ) ) {
		if ( ( err = snd_pcm_prepare( handle->playback_handle ) ) < 0 ) {
			fprintf( stderr, "cannot prepare audio interface for use (%s)\n",
					snd_strerror( err ) );
			exit( 1 );
		}
	}
/*	printf( "avail: %d\n", get_available_frames( h ) );
	float output[256*2] = {0};
	if ( ( err = snd_pcm_writei( handle->playback_handle, output, 256 ) ) != 256 ) {
		fprintf( stderr, "write to audio interface fails (%s)\n",
				snd_strerror( err ) );
		exit( 1 );
	}*/

/*	if ( ( err = snd_pcm_start( handle->playback_handle ) ) < 0 ) {
		fprintf( stderr, "cannot start playback (%s)\n",
				snd_strerror( err ) );
		if ( SND_PCM_STATE_XRUN == snd_pcm_state( handle->playback_handle ) )
			printf( "SND_PCM_STATE_XRUN\n" );
		if ( SND_PCM_STATE_OPEN == snd_pcm_state( handle->playback_handle ) )
			printf( "SND_PCM_STATE_OPEN\n" );
		if ( SND_PCM_STATE_SETUP == snd_pcm_state( handle->playback_handle ) )
			printf( "SND_PCM_STATE_SETUP\n" );
		if ( SND_PCM_STATE_PREPARED == snd_pcm_state( handle->playback_handle ) )
			printf( "SND_PCM_STATE_PREPARED\n" );
		if ( SND_PCM_STATE_RUNNING == snd_pcm_state( handle->playback_handle ) )
			printf( "SND_PCM_STATE_RUNNING\n" );
		if ( SND_PCM_STATE_DRAINING == snd_pcm_state( handle->playback_handle ) )
			printf( "SND_PCM_STATE_DRAINING\n" );
		if ( SND_PCM_STATE_PAUSED == snd_pcm_state( handle->playback_handle ) )
			printf( "SND_PCM_STATE_PAUSED\n" );
		if ( SND_PCM_STATE_SUSPENDED == snd_pcm_state( handle->playback_handle ) )
			printf( "SND_PCM_STATE_SUSPENDED\n" );
		if ( SND_PCM_STATE_DISCONNECTED == snd_pcm_state( handle->playback_handle ) )
			printf( "SND_PCM_STATE_DISCONNECTED\n" );
		exit( 1 );
	}*/
/*	if ( ( err = snd_pcm_pause( handle->playback_handle, 0 ) ) < 0 ) {
		fprintf( stderr, "cannot pause audio interface (%s)\n",
				snd_strerror( err ) );
		exit( 1 );
	} */
}
void stop_audio( nle_audio_handle h )
{
	a_handle* handle = (a_handle*)h;
	if ( ( err = snd_pcm_drain( handle->playback_handle ) ) < 0 ) {
		fprintf( stderr, "cannot stop playback (%s)\n",
				snd_strerror( err ) );
		exit( 1 );
	}
}

int get_audio_poll_fd( nle_audio_handle h )
{
	a_handle* handle = (a_handle*)h;
	int fd;
	struct pollfd* ufds;
	int count = snd_pcm_poll_descriptors_count( handle->playback_handle );
	if ( count <= 0 ) {
		fprintf( stderr, "Invalid poll descriptors count\n" );
		exit( 1 );
	}
	ufds = (struct pollfd*)malloc( sizeof(struct pollfd) * count );
	if ( ufds == NULL ) {
		fprintf( stderr, "Not enough memory\n" );
		exit( 1 );
	}
	if ( ( err = snd_pcm_poll_descriptors( handle->playback_handle, ufds, count ) ) < 0 ) {
		fprintf( stderr,
				"Unable to obtain poll descriptors for playback: %s\n",
				snd_strerror( err ) );
		exit( 1 );
	}
	fd = ufds[0].fd;
	free( ufds );
	return fd;
}
int get_available_frames( nle_audio_handle h )
{
	a_handle* handle = (a_handle*)h;
	return snd_pcm_avail_update( handle->playback_handle );
}

void write_audio_frames( nle_audio_handle h, float* output, int frames )
{
	a_handle* handle = (a_handle*)h;
	if ( ( err = snd_pcm_writei( handle->playback_handle, output, frames ) ) != frames ) {
		if ( err == -EPIPE && SND_PCM_STATE_XRUN == snd_pcm_state( handle->playback_handle ) ) {
			fprintf( stderr, "xrun\n" );
			if ( ( err = snd_pcm_prepare( handle->playback_handle ) ) < 0 ) {
				fprintf( stderr, "cannot prepare audio interface for use (%s)\n",
						snd_strerror( err ) );
				exit( 1 );
			}
			if ( ( err = snd_pcm_writei( handle->playback_handle, output, frames ) ) != frames ) {
				fprintf( stderr, "1 write to audio interface fails (%s)\n",
						snd_strerror( err ) );
				exit( 1 );
			}
			return;
		}
		fprintf( stderr, "2 write to audio interface fails (%s)\n",
				snd_strerror( err ) );
		exit( 1 );
	}

}

