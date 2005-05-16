#include <string>
#include <iostream>
#include "VideoClip.h"
#include "IVideoFile.h"

using namespace std;

namespace nle
{
	
VideoClip::VideoClip( Track *track, int64_t position, IVideoFile *vf )
	: Clip( track, position )
{
	m_videoFile = vf;
}
VideoClip::~VideoClip(){}
int64_t VideoClip::length()
{
	return m_videoFile->length() - ( m_trimA + m_trimB );
}
void VideoClip::reset()
{
	m_videoFile->seek( m_trimA );
}
frame_struct* VideoClip::getFrame( int64_t position )
{
	if ( position < m_position || position > m_position + length() )
		return NULL;
	return m_videoFile->read();
}
	
} /* namespace nle */
