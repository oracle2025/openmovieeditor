#include "VideoTrack.h"
#include "VideoClip.h"
#include "VideoFileQT.h"

namespace nle
{

VideoTrack::VideoTrack( int num )
	: Track( num )
{
}
VideoTrack::~VideoTrack()
{
}
void VideoTrack::add_video( int64_t position, const char* filename )
{
	VideoFileQT *vf = new VideoFileQT( filename );
	
	if ( !vf->ok() ) {
		delete vf;
		return;
	}
	m_clips.push_back( new VideoClip( this, position, vf ) );
}
	
} /* namespace nle */
