#include <string>
#include "VideoClip.h"
#include "IVideoFile.h"

namespace nle
{
	
VideoClip::VideoClip( Track *track, int64_t position, IVideoFile *vf )
//VideoClip::VideoClip( Track *track, int64_t position, const char* filename )
	: Clip( track, position )
{
//	m_length = 500;
	m_videoFile = vf; //new VideoFileQT("/home/oracle/test3.mov");
}
int64_t VideoClip::length()
{
	return m_videoFile->length() - ( m_trimA + m_trimB );
//	return 500 - ( m_trimA + m_trimB );
}
void VideoClip::reset()
{
	m_videoFile->seek( m_trimA );
}
unsigned char* VideoClip::getFrame( int64_t position )
{
	if ( position < m_position || position > m_position + length() )
		return NULL;
	return m_videoFile->read();
}
	
} /* namespace nle */
