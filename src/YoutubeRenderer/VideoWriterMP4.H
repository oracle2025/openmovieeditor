/*  VideoWriterMP4.H
 *
 *  Copyright (C) 2009 Richard Spindler <richard.spindler AT gmail.com>
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

#include "IVideoFileWriter.H"
#include "RenderDialog/video_format.H"
#include <lqt.h>
#include <gavl/gavl.h>

namespace nle
{

class VideoWriterMP4 : public IVideoFileWriter
{
	public:
		/* w, h, fps, rate */
		VideoWriterMP4( const char* filename );
		~VideoWriterMP4();
		void encodeVideoFrame( LazyFrame* frame );
		void encodeAudioFrame( float* buffer, int frames );
		video_format* format();
		bool ok();
	private:
		void initFormats();
		quicktime_t*        m_encoder;
		float               m_left_buffer[32000];
		float               m_right_buffer[32000];
		video_format        m_ome_format;
		unsigned char**     m_rows;
		gavl_video_format_t m_video_format;
		gavl_audio_format_t m_audio_format;
		int64_t             m_timestamp;
		bool                m_ok;
};

} /* namespace nle */

