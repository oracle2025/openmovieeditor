#ifndef _VIDEO_FILE_QT_H
#define _VIDEO_FILE_QT_H

#include <quicktime/lqt.h>
#include "IVideoFile.H"
#include "frame_struct.h"
namespace nle
{

class VideoFileQT : public IVideoFile
{
	public:
		VideoFileQT( const char* filename );
		~VideoFileQT();
		bool ok();
		int64_t length();
		int fps();
		frame_struct* read();
		void seek( int64_t frame );
	private:
		quicktime_t* m_qt;
		unsigned char *m_frame;
		unsigned char **m_rows;
		frame_struct m_framestruct;
		bool m_ok;
};

} /* namespace nle */
#endif /* _VIDEO_FILE_QT_H */
