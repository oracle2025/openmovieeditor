/*  AudioFileFfmpeg.cxx
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

#include "render_helper.H"
#include "AudioFileFfmpeg.H"

namespace nle
{

AudioFileFfmpeg::AudioFileFfmpeg( string filename )
{
	m_ok = false;
	m_filename = filename;
	m_formatContext = NULL;
	if ( av_open_input_file( &m_formatContect, filename.c_str(), NULL, 0, NULL ) < 0 ) {
		return;
	}
	if ( av_find_stream_info( m_formatContext ) < 0 ) {
		return;
	}
	m_audioStream = -1;
	for ( int i = 0; i < m_formatContext->nb_streams; i++ ) {
		if ( m_formatContext->streams[i]->codec->codec_type == CODEC_TYPE_AUDIO ) {
			m_audioStream = i;
			break;
		}
	}
	if ( m_audioStream == -1 ) {
		return;
	}
	m_codecContext = m_formatContext->streams[m_audioStream]->codec;
	
	m_codecContext->sample_rate;

	m_codec = avcodec_find_decoder( m_codecContext->codec_id );
	if ( m_codec == NULL ) {
		return;
	}
	if ( avcodec_open( m_codecContext, m_codec ) < 0 ) {
		return;
	}
	m_framerate = av_q2d( m_formatContext->streams[m_audioStream]->r_frame_rate );
	int64_t len = m_formatContect->duration - m_formatContext->start_time;
	m_length = (int64_t)( len * m_framerate / AV_TIME_BASE );
	m_ok = true;
}
AudioFileFfmpeg::~AudioFileFfmpeg()
{
	if ( m_formatContext ) {
		av_close_input_file( m_formatContext );
	}
}
void AudioFileFfmpeg::seek( int64_t sample )
{
	avcodec_flush_buffers( m_codecContext );
	const int64_t time_base = 1;
	int64_t timestamp = ( sample * AV_TIME_BASE * time_base ) / (int64_t)m_samplerate;
	av_seek_frame( m_formatContext, -1, timestamp, 0 );

	m_tmpBufferStart = m_tmpBuffer;
	m_tmpBufferLen = 0;
}
#define min( v1, v2 ) v1 < v2 ? v1 : v2

int AudioFileFfmpeg::fillBuffer( float* output, unsigned long frames )
{
	unsigned long written = 0;
	float* optr = output;
	int ret = 0;

	while ( ret >= 0 && written < frames ) {
		if ( m_tmpBufferLen > 0 ) {
			int s = min( m_tmpBufferLen, frames - written );
			decode_uint8_to_float( m_tmpBufferStart, &optr, 1, s * 2 );
			optr += s;
			m_tmpBufferStart += s;
			m_tmpBufferLen -= s;
			ret = 0;
		} else {
			ret = av_read_frame( m_formatContext, &m_packet );
			int len = m_packet.size;
			uint8_t *ptr = m_packet.data;
			int data_size;
			while ( ptr != NULL && ret >= 0 && m_packet.stream_index == m_audioStream && len > 0 ) {
				ret = avcodec_decode_audio( m_codecContext, m_tmpBuffer, &data_size, ptr, len );
				if ( ret < 0 ) {
					ret = 0;
					break;
				}
				len -= ret;
				ptr += ret;
				if ( data_size > 0 ) {
					m_tmpBufferStart = m_tmpBuffer;
					m_tmpBufferLen = data_size / 2;
				}
			}
			av_free_packet( &m_packet );
		}
	}
	return written;
}

} /* namespace nle */
