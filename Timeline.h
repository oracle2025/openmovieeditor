#ifndef _TIMELINE_H_
#define _TIMELINE_H_

#include <stdint.h>
#include <list>

namespace nle
{

class Track;

class Timeline
{
	public:
		// constructors and destructors
		Timeline();
		~Timeline();

		int64_t m_playPosition;
		// method declerations
		void reset();
		unsigned char* nextFrame();
		int fillBuffer( float* output, unsigned long frames );

		void add_video( int track, int64_t position, const char* filename );
		std::list< Track* >* getTracks() { return &m_tracks; }
	private:
		// member variable declerations
		std::list< Track* > m_tracks;
};
	
} /* namespace nle */

#endif /* _TIMELINE_H_ */
