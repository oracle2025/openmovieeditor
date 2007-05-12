/*  ThreadedAudioReader.cxx
 *
 *  Copyright (C) 2007 Richard Spindler <richard.spindler AT gmail.com>
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
#include <errno.h>
#include "ThreadedAudioReader.H"
#include <string>
#include <iostream>
using namespace std;

#include "IAudioFile.H"

#define DEFAULT_RB_SIZE 16384
#define FRAMES 4096

namespace nle
{

static void* start_audio_reader_thread( void* data )
{
	ThreadedAudioReader* reader = (ThreadedAudioReader*)data;
	reader->run();
	return 0;
}

ThreadedAudioReader::ThreadedAudioReader( IAudioFile* file )
	: m_file( file )
{
	m_ringBuffer = jack_ringbuffer_create( DEFAULT_RB_SIZE * sizeof(float) * 2 );
	m_buffer = new float[ FRAMES * 2 ];
	m_running = true;
	m_seek = false;
	m_fileDone = false;
	pthread_mutex_init( &m_readMutex, 0 );
	pthread_cond_init( &m_readCondition, 0 );
	pthread_create( &m_thread, NULL, start_audio_reader_thread, (void*)this );
	
}

ThreadedAudioReader::~ThreadedAudioReader()
{
	
	pthread_mutex_lock( &m_readMutex );
	m_running = false;
	pthread_cond_signal( &m_readCondition );
	pthread_mutex_unlock( &m_readMutex );
	pthread_join( m_thread, 0 );
	pthread_mutex_destroy( &m_readMutex );
	pthread_cond_destroy( &m_readCondition );
	jack_ringbuffer_free( m_ringBuffer );
	delete [] m_buffer;
}

int ThreadedAudioReader::fillBuffer( float* output, unsigned long frames )
{
	int bytes_read;
	unsigned int frames_read;
	bool fileDone = false;
	bytes_read = jack_ringbuffer_read( m_ringBuffer, (char*)output, frames * sizeof(float) * 2 );
	frames_read = bytes_read / 2 / sizeof(float);
	if ( pthread_mutex_trylock( &m_readMutex ) != EBUSY ) {
		if  ( m_seekPending ) {
			jack_ringbuffer_reset( m_ringBuffer );
			m_seek = true;
			m_sample = m_samplePending;
			m_seekPending = false;
			m_fileDone = false;
		}
		fileDone = m_fileDone;
		pthread_cond_signal( &m_readCondition );
		pthread_mutex_unlock( &m_readMutex );
	}
	if ( frames_read < frames && fileDone ) {
		return frames_read;
	}
	return frames;
}	

void ThreadedAudioReader::seek( int64_t sample )
{
	if ( pthread_mutex_trylock( &m_readMutex ) != EBUSY ) {
		jack_ringbuffer_reset( m_ringBuffer );
		m_seek = true;
		m_sample = sample;
		m_fileDone = false;
		pthread_cond_signal( &m_readCondition );
		pthread_mutex_unlock( &m_readMutex );
	} else {
		m_seekPending = true;
		m_samplePending = sample;
	}
}

void ThreadedAudioReader::run()
{
	int frames_read;
	pthread_mutex_lock( &m_readMutex );
	while( m_running ) {
		if ( m_seek ) {
			m_file->seek( m_sample );
			m_seek = false;
		}
		while ( !m_fileDone && jack_ringbuffer_write_space( m_ringBuffer ) >= ( FRAMES * sizeof(float) * 2) ) {
			frames_read = m_file->fillBuffer( m_buffer, FRAMES );
			jack_ringbuffer_write( m_ringBuffer, (char*)m_buffer, frames_read * sizeof(float) * 2 );
			if ( frames_read < FRAMES ) {
				m_fileDone = true;
			}
		}
		pthread_cond_wait( &m_readCondition, &m_readMutex );
	}
	pthread_mutex_unlock( &m_readMutex );
	pthread_exit( 0 );
}

} /* namespace nle */
