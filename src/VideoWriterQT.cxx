/* VideoWriterQT.cxx
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

#include "VideoWriterQT.H"
#include <cassert>
#include "LazyFrame.H"

namespace nle
{
VideoWriterQT::VideoWriterQT( quicktime_t *qt, video_format &format )
{
	m_qt = qt;
	m_format = format;
	m_samplerate = quicktime_sample_rate( m_qt, 0 );
	m_rows  = new unsigned char*[m_format.h];
}
VideoWriterQT::~VideoWriterQT()
{
	quicktime_close( m_qt );
}
void VideoWriterQT::encodeVideoFrame( LazyFrame* frame )
{
	gavl_video_format_t gavl_format;
	gavl_format.frame_width  = m_format.w;
	gavl_format.frame_height = m_format.h;
	gavl_format.image_width  = m_format.w;
	gavl_format.image_height = m_format.h;
	gavl_format.pixel_width = 1;
	gavl_format.pixel_height = 1;
	gavl_format.pixelformat = GAVL_RGB_24;
	gavl_format.interlace_mode = GAVL_INTERLACE_NONE;
	frame->set_target( &gavl_format );

	gavl_video_frame_t* gavl_frame = frame->target();

	for ( int j = 0; j < m_format.h; j++ ) {
		m_rows[j] = ((unsigned char*)(gavl_frame->planes[0])) + m_format.w * 3 * j;
	}

	quicktime_encode_video( m_qt, m_rows, 0 );
}
void VideoWriterQT::encodeAudioFrame( float* buffer, int frames )
{
	float *buffer_p[2] = { left_buffer, right_buffer };
	assert( frames <= 32000 );
	for ( int i = 0; i < frames; i++ ) {
		left_buffer[i] = buffer[i*2];
		right_buffer[i] = buffer[i*2+1];
	}
	lqt_encode_audio_track( m_qt, 0, buffer_p, frames, 0 );
}

} /* namespace nle */

