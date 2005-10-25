/*  VideoFile.H
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

#ifndef _VIDEO_FILE_H_
#define _VIDEO_FILE_H_

#include <string>

namespace nle
{

#define VF_READ 1
#define VF_WRITE 2

class VideoFile
{
	public:
		// constructors and destructors
		~VideoFile();
		
		// method declerations
		int64_t length();
		void size( int &width, int &height );
		int fps();
		uchar* read();
		void seek( int64_t frame );
	protected:
		VideoFile( const std::string &filename, int mode = VF_READ );
	private:
		// member variable declerations
		int             m_videoStream;
		AVCodecContext  *m_CodecCtx;
		AVCodec         *m_Codec;
		AVFrame         *m_Frame; 
		AVFrame         *m_FrameRGB;
		AVPacket        m_packet;
		int             m_frameFinished;
		int             m_numBytes;
		uint8_t         *m_buffer;
		AVFormatContext *m_FormatCtx;
		int64_t calc_frame_cnt();
		int64_t frame_to_timestamp( int64_t frame );
		int64_t calc_duration();
		int64_t m_frameCnt;
		int64_t m_duration;

};

}

#endif /* _VIDEO_FILE_H_ */
