#ifndef _I_VIDEO_FILE_H_
#define _I_VIDEO_FILE_H_

#include "frame_struct.h"

namespace nle
{

class IVideoFile
{
	public:
		virtual ~IVideoFile(){}
		virtual int64_t length() = 0;
//		virtual void size( int &width, int &height ) = 0;
		inline int width() { return m_width; }
		inline int height() { return m_height; }
		virtual int fps() = 0;
		virtual frame_struct* read() = 0;
		virtual void seek( int64_t frame ) = 0;
	protected:
		int m_width;
		int m_height;
};

} /* namespace nle */

#endif /* _I_VIDEO_FILE_H_ */
