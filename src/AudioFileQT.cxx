/*  AudioFileQT.cxx
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

#include <cstring>

#include "AudioFileQT.H"
#include "ErrorDialog/IErrorHandler.H"


namespace nle
{

AudioFileQT::AudioFileQT( string filename )
{
	m_ok = false;
	m_qt = NULL;
	m_oneShot = true;
	m_filename = filename;
	if ( !quicktime_check_sig( (char*)filename.c_str() ) ) {
		ERROR_DETAIL( "This is not a Quicktime audio file" );
		return;
	}
	m_qt = quicktime_open( (char*)filename.c_str(), true, false );
	if ( !m_qt ) {
		ERROR_DETAIL( "Could not open Quicktime file" );
		return;
	}
	if ( quicktime_audio_tracks( m_qt ) == 0 ) {
		ERROR_DETAIL( "This Quicktime file does not have a audio track" );
		return;
	}
	if ( !quicktime_supported_audio( m_qt, 0 ) ) {
		ERROR_DETAIL( "This Audio Codec is not supported" );
		return;
	}
	m_samplerate = quicktime_sample_rate( m_qt, 0 );
	//m_length = quicktime_audio_length( m_qt, 0 ) * NLE_TIME_BASE / m_samplerate;
	m_length = quicktime_audio_length( m_qt, 0 );
	if ( m_samplerate != 48000 && m_samplerate != 44100 ) {
		CLEAR_ERRORS();
		ERROR_DETAIL( "Audio samplerates other than 48000 and 44100 are not supported" );
		return;
	}
	if ( quicktime_track_channels( m_qt, 0 ) != 2 ) {
		CLEAR_ERRORS();
		ERROR_DETAIL( "Only Stereo audio files are supported" );
		return;
	}
	m_ok = true;
}
AudioFileQT::~AudioFileQT()
{
	if ( m_qt )
		quicktime_close( m_qt );
}
void AudioFileQT::seek( int64_t position )
{
	//quicktime_set_audio_position( m_qt, position * m_samplerate / NLE_TIME_BASE, 0 );
	quicktime_set_audio_position( m_qt, position, 0 );
	m_oneShot = true;
}
int AudioFileQT::fillBuffer( float* output, unsigned long frames )
{
	// l_buffer and r_buffer are defined in the class definition
	float *left_buffer = l_buffer;
	float *right_buffer = r_buffer;
	int64_t diff = lqt_last_audio_position( m_qt, 0 );
	if ( frames > 4096) {
		left_buffer = new float[frames];
		right_buffer = new float[frames];
	}
	float *buf_pointer[2] = { left_buffer, right_buffer };
	lqt_decode_audio_track( m_qt, NULL, buf_pointer, frames, 0 );
	diff = lqt_last_audio_position( m_qt, 0 ) - diff;
	if ( m_oneShot ) {
		diff = frames;
		m_oneShot = false;
	}
	for ( int i = 0; i < diff; i++ ) {
		output[i * 2] = left_buffer[i]; // use left shift for *2 ??
		output[i * 2 + 1] = right_buffer[i];
	}
	if ( frames > 4096) {
		delete [] left_buffer;
		delete [] right_buffer;
	}
	return diff;
}




} /* namespace nle */
