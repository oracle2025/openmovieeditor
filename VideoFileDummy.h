#ifndef _VIDEO_FILE_DUMMY_H_
#define _VIDEO_FILE_DUMMY_H_

#include <vector>
#include <string>

#include "IVideoFile.h"

namespace nle
{

class VideoFileDummy : public IVideoFile
{
	public:
		VideoFileDummy( const char* dir );
		int64_t length();
		void size( int &width, int &height );
		int fps();
		unsigned char* read();
		void seek( int64_t frame );
	private:
		int64_t m_length;
		int64_t m_offset;
		int m_width;
		int m_height;
		std::vector< std::string > m_files;
		std::string m_directory;
		unsigned char* m_buffer;
};
	
} /* namespace nle */
#endif /* _VIDEO_FILE_DUMMY_H_ */

