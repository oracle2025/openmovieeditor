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

#include <iostream>

#include <string.h>

#include "AudioFileQT.H"

using namespace std;

#define FRAMES_PER_BUFFER 256

namespace nle
{

AudioFileQT::AudioFileQT( const char* filename )
{
	m_ok = false;
	m_qt = NULL;
	char *lqt_sucks_filename = strdup( filename );
	if ( !quicktime_check_sig( lqt_sucks_filename ) )
		return;
	m_qt = quicktime_open( lqt_sucks_filename, true, false );
	free(lqt_sucks_filename);
	if ( quicktime_audio_tracks( m_qt ) == 0 )
		return;
	if ( !quicktime_supported_audio( m_qt, 0 ) )
		return;
	m_length = quicktime_audio_length( m_qt, 0 );
	//check samplerate
	//check channels
	strncpy(m_filename, filename, STR_LEN);
	m_ok = true;
	m_oneShot = true;
}
AudioFileQT::~AudioFileQT()
{
	if ( m_qt )
		quicktime_close( m_qt );
}
void AudioFileQT::seek( int64_t sample )
{
	quicktime_set_audio_position( m_qt, sample, 0 );
	m_oneShot = true;
	
}
int AudioFileQT::fillBuffer( float* output, unsigned long frames )
{
	static float left_buffer[FRAMES_PER_BUFFER];
	static float right_buffer[FRAMES_PER_BUFFER];
	static float *buf_pointer[2] = { left_buffer, right_buffer };
	if ( frames > FRAMES_PER_BUFFER ) {
		return 0;
	}
	int64_t diff = lqt_last_audio_position( m_qt, 0 );
	lqt_decode_audio_track( m_qt, NULL, buf_pointer, frames, 0 );
	diff = lqt_last_audio_position( m_qt, 0 ) - diff;
	if ( m_oneShot ) {
		diff = frames;
		m_oneShot = false;
	}
	for ( int i = 0; i < diff; i++ ) {
		output[i*2] = left_buffer[i]; // use left shift for *2 ??
		output[i*2+1] = right_buffer[i];
	}
//	cout << "AudioFileQT::fillBuffer " << diff << endl;
	return diff;
}

bool AudioFileQT::ok() { return m_ok; }



} /* namespace nle */
