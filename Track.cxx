#include "Track.h"
#include "Clip.h"

#include <algorithm>

namespace nle
{


Track::Track( int num )
{
	m_num = num;
}
Track::~Track()
{
	Clip *clip;
	while(m_clips.size()) {
		clip = m_clips.pop_back();
		delete clip;
	}
}
void Track::add( Clip* clip )
{
	m_clips.push_back( clip );
}
void Track::remove( Clip* clip )
{
	std::list< Clip* >::iterator j;
	for ( j = m_clips.begin(); j != m_clips.end(); j++ ) {
		if ( *j == clip ) {
			m_clips.erase(j);
			return;
		}
	}
}
void reset_helper( Clip* clip ) { clip->reset(); }
void Track::reset()
{
	for_each( m_clips.begin(), m_clips.end(), reset_helper );
}
	
} /* namespace nle */
