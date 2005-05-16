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
void VideoTrack::reset()
{
	Track::reset();
	m_playPosition = 0;
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
frame_struct* VideoTrack::nextFrame()
{
	frame_struct* res = NULL;
	m_playPosition++;
	for ( std::list< Clip* >::iterator j = m_clips.begin(); j != m_clips.end(); j++ ) {
		res = ((VideoClip*)(*j))->getFrame( m_playPosition - 1 );
		if ( res ) {
			return res;
		}
	}
	return NULL;
}
	
} /* namespace nle */
