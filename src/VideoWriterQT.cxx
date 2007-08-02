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

namespace nle
{
VideoWriterQT::VideoWriterQT( quicktime_t *qt, video_format &format )
{
	m_qt = qt;
	m_format = format;
	m_samplerate = quicktime_sample_rate( m_qt, 0 );
}
VideoWriterQT::~VideoWriterQT()
{
	quicktime_close( m_qt );
}
void VideoWriterQT::encodeVideoFrame( frame_struct* frame )
{
	quicktime_encode_video( m_qt, frame->rows, 0 );
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

