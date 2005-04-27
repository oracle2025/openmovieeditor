#ifndef _TIMELINE_H_
#define _TIMELINE_H_

#include <stdint.h>
#include <list>

namespace nle
{

class Track;
class VideoTrack;
class AudioTrack;

class Timeline
{
	public:
		// constructors and destructors
		Timeline();
		~Timeline();

		int64_t m_playPosition;
		int64_t m_samplePosition;
		// method declerations
		void reset();
		unsigned char* nextFrame();
		int fillBuffer( float* output, unsigned long frames );

		void add_video( int track, int64_t position, const char* filename );
		void add_audio( int track, int64_t position, const char* filename );
		std::list< Track* >* getTracks() { return &m_allTracks; }
	private:
		// member variable declerations
		std::list< Track* > m_allTracks;
		std::list< VideoTrack* > m_videoTracks;
		std::list< AudioTrack* > m_audioTracks;
};
	
} /* namespace nle */

#endif /* _TIMELINE_H_ */
