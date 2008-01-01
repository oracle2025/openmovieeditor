/*  AudioFileMpeg3.cxx
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

#ifdef LIBMPEG3

#include <cstring>

#include "AudioFileMpeg3.H"
#include "ErrorDialog/IErrorHandler.H"


namespace nle
{

AudioFileMpeg3::AudioFileMpeg3( string filename )
{
	m_ok = false;
	m_file = NULL;
	m_filename = filename;

	if ( !mpeg3_check_sig( (char*)filename.c_str() ) ) {
		ERROR_DETAIL( "This is not an LibMPEG3 readable Audio or TOC File" );
		return;
	}
	m_file = mpeg3_open( (char*)filename.c_str() );
	if ( !m_file ) {
		ERROR_DETAIL( "Could not open MPEG file" );
		return;
	}
	if ( !mpeg3_has_audio( m_file ) || mpeg3_total_astreams( m_file ) <= 0 ) {
		ERROR_DETAIL( "This MPEG file does not have an audio track" );
		return;
	}


	m_samplerate = mpeg3_sample_rate( m_file, 0 );
	m_length = mpeg3_audio_samples( m_file, 0 );
	if ( m_samplerate != 48000 && m_samplerate != 44100 ) {
		CLEAR_ERRORS();
		ERROR_DETAIL( "Audio samplerates other than 48000 and 44100 are not supported" );
		return;
	}
	if ( mpeg3_audio_channels( m_file, 0 ) != 2 ) {
		CLEAR_ERRORS();
		ERROR_DETAIL( "Only Stereo audio files are supported" );
		return;
	}
	m_ok = true;
}
AudioFileMpeg3::~AudioFileMpeg3()
{
	if ( m_file )
		mpeg3_close( m_file );
}
void AudioFileMpeg3::seek( int64_t position )
{
	mpeg3_set_sample( m_file, position, 0 );
}
int AudioFileMpeg3::fillBuffer( float* output, unsigned long frames )
{
	// l_buffer and r_buffer are defined in the class definition
	float *left_buffer = l_buffer;
	float *right_buffer = r_buffer;
	if ( frames > 4096) {
		left_buffer = new float[frames];
		right_buffer = new float[frames];
	}
	int64_t diff = (-1) * mpeg3_get_sample( m_file, 0 );
	mpeg3_read_audio( m_file, 
		left_buffer,      // Pointer to pre-allocated buffer of floats
		0,      // Pointer to pre-allocated buffer if int16's
		0,          // Channel to decode
		frames,         // Number of samples to decode
		0);    
	mpeg3_reread_audio( m_file, 
		right_buffer,      // Pointer to pre-allocated buffer of floats
		0,      // Pointer to pre-allocated buffer if int16's
		1,          // Channel to decode
		frames,         // Number of samples to decode
		0);
	diff += mpeg3_get_sample( m_file, 0 );
	if ( mpeg3_get_sample( m_file, 0 ) > m_length  ) {
		diff = frames - ( mpeg3_get_sample( m_file, 0 ) - m_length );
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

#endif /* LIBMPEG3 */

