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
		VideoFile( const std::string &filename, int mode = VF_READ );
		~VideoFile();
		
		// method declerations
		int64_t length();
		void size( int &width, int &height );
		int fps();
		uchar* read();
		void seek( int64_t frame );
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
