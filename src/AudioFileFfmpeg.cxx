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

#ifdef AVCODEC

#include "render_helper.H"
#include "AudioFileFfmpeg.H"
#include "ErrorDialog/IErrorHandler.H"

namespace nle
{

AudioFileFfmpeg::AudioFileFfmpeg( string filename )
{
	cout << "AudioFileFfmpeg: " << filename.c_str() << endl;
	m_ok = false;
	m_filename = filename;
	m_formatContext = NULL;
	av_register_all(); // TODO: once should be enought -> main

	AVInputFormat *file_iformat = av_find_input_format( filename.c_str() );
	
	if ( av_open_input_file( &m_formatContext, filename.c_str(), file_iformat, 0, NULL ) < 0 ) {
		ERROR_DETAIL( "This file cannot be read by ffmpeg" );
		return;
	}
	if ( av_find_stream_info( m_formatContext ) < 0 ) {
		ERROR_DETAIL( "av_find_stream_info failed" );
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
		ERROR_DETAIL( "No Audio Stream found in file" );
		return;
	}
	m_codecContext = m_formatContext->streams[m_audioStream]->codec;
	m_samplerate = m_codecContext->sample_rate;
	m_codec = avcodec_find_decoder( m_codecContext->codec_id );
	if ( m_codec == NULL ) {
		ERROR_DETAIL( "Codec not supported" );
		return;
	}
	if ( avcodec_open( m_codecContext, m_codec ) < 0 ) {
		ERROR_DETAIL( "avcodec_open failed" );
		return;
	}
	if ( m_samplerate != 48000 ) {
		CLEAR_ERRORS();
		ERROR_DETAIL( "Audio samplerates other than 48000 are not supported" );
		return;
	}
	if ( m_codecContext->channels != 2 && m_codecContext->channels != 1) {
		CLEAR_ERRORS();
		ERROR_DETAIL( "Only Stereo and Mono audio files are supported" );
		return;
	}

	int64_t len = m_formatContext->duration - m_formatContext->start_time;
	m_length = (int64_t)( len * m_samplerate / AV_TIME_BASE );
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
			int s = min( m_tmpBufferLen / m_codecContext->channels, frames - written );
			s = s * m_codecContext->channels;
			decode_int16_to_float( m_tmpBufferStart, &optr, 1, s );
			if ( m_codecContext->channels == 1 ) {
				for ( int i = s - 1; i >= 0; i-- ) {
					optr[2 * i + 1] = optr[i];
					optr[2 * i] = optr[i];
				}
			}
			optr += s;
			m_tmpBufferStart += s;
			m_tmpBufferLen -= s;
			written += s / m_codecContext->channels;
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

#endif /* AVCODEC */

