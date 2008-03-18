/*  AudioFileFfmpeg.H
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


#ifndef _AUDIO_FILE_FFMPEG_H_
#define _AUDIO_FILE_FFMPEG_H_

#ifdef AVCODEC

extern "C" {
#ifdef OLD_FFMPEG
  #include <ffmpeg/avcodec.h>
  #include <ffmpeg/avformat.h>
  #ifdef SWSCALE
    #include <ffmpeg/swscale.h>
  #endif /* SWSCALE */
#else
  #include <libavcodec/avcodec.h>
  #include <libavformat/avformat.h>
  #ifdef SWSCALE
    #include <libswscale/swscale.h>
  #endif /* SWSCALE */
#endif
}

#include "global_includes.H"
#include "IAudioFile.H"

namespace nle
{

class AudioFileFfmpeg : public IAudioFile
{
	public:
		AudioFileFfmpeg( string filename );
		~AudioFileFfmpeg();
		void seek( int64_t position );
		int fillBuffer( float* output, unsigned long frames );
	private:
		AVFormatContext* m_formatContext;
		AVCodecContext*  m_codecContext;
		AVCodec*         m_codec;
		AVPacket         m_packet;
		int              m_audioStream;
		int16_t          m_tmpBuffer[AVCODEC_MAX_AUDIO_FRAME_SIZE];
		int16_t*         m_tmpBufferStart;
		unsigned long    m_tmpBufferLen;
};
	
} /* namespace  nle */

#endif /* AVCODEC */

#endif /* _AUDIO_FILE_FFMPEG_H_ */
