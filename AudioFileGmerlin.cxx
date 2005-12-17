/*  AudioFileGmerlin.cxx
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

#include <avdec.h>

#include "AudioFileGmerlin.H"

namespace nle
{

AudioFileGmerlin::AudioFileGmerlin( string filename )
	: m_filename( filename )
{
	bgav_options_t* opt;
	m_file = bgav_create();
	opt = bgav_get_options( m_file );
	if(!bgav_open( file, argv[1]) ) {
		fprintf( stderr, "Could not open file %s\n", argv[1] );
		free(file);
		return -1;
	}
	if(bgav_is_redirector(file)) {
		error;
		return;
	}
	if ( bgav_num_tracks == 0 ) {
		error;
		return;
	}
	bgav_get_duration( m_file, 0 );
	bgav_num_audio_streams( m_file, 0 );
	bgav_can_seek( m_file );
	bgav_select_track( m_file, 0 );
	bgav_set_audio_stream( m_file, 0, BGAV_STREAM_DECODE );
	bgav_start( m_file );
}
AudioFileGmerlin::~AudioFileGmerlin()
{
	bgav_close( m_file );
}
void AudioFileGmerlin::seek( int64_t sample );
{
	bgav_seek( m_file, gavl_time_t* ); // I hate timestamping
}
int AudioFileGmerlin::fillBuffer( float* output, unsigned long frames )
{
	bgav_read_audio( m_file, gavl_audio_frame_t * frame, 0, frames );
}

	
} /* namespace nle */
