#include "Timeline.h"
#include "VideoTrack.h"
#include "VideoClip.h"
#include "Track.h"
#include "SwitchBoard.h"
#include <string>


namespace nle
{

Timeline::Timeline()
{
	m_tracks.push_back( new VideoTrack( 0 ) );
	m_tracks.push_back( new VideoTrack( 1 ) );
	SwitchBoard::i()->timeline( this );
	m_playPosition = 0;
}
Timeline::~Timeline()
{
}

void Timeline::add_video( int track, int64_t position, const char* filename )
{
	std::list< Track* >::iterator i = m_tracks.begin();
	for ( int j = 0; j < track; j++ )
		i++;
	((VideoTrack*)(*i))->add_video( position, filename );
}
void reset_helper( Track* track ) { track->reset(); }
void Timeline::reset()
{
	m_playPosition = 0;
	for_each( m_tracks.begin(), m_tracks.end(), reset_helper );
}
unsigned char* Timeline::nextFrame()
{
	unsigned char* res = NULL;
	for ( std::list< Track* >::iterator i = m_tracks.begin(); i != m_tracks.end(); i++ ) {
		Track* current = *i;
		std::list< Clip* >* clips = current->getClips();
		for ( std::list< Clip* >::iterator j = clips->begin(); j != clips->end(); j++ ) {
			res = ((VideoClip*)(*j))->getFrame(m_playPosition);
			if ( res ) {
				m_playPosition++;
				return res;
			}
		}
	}
	m_playPosition++;
	return res;
}

int Timeline::fillBuffer( float* output, unsigned long frames )
{
	float buffer1[frames]; //frames * 2??
	float buffer2[frames];
}



} /* namespace nle */
