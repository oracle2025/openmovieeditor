#ifndef _AUDIO_TRACK_H_
#define _AUDIO_TRACK_H_

#include "Track.h"

namespace nle
{

class AudioClip;

class AudioTrack : public Track
{
	public:
		// constructors and destructors
		AudioTrack( int num );
		~AudioTrack();

		// method declerations
		int fillBuffer( float* output, unsigned long frames, int64_t position );
		void add_audio( int64_t position, const char* filename );
		int type() { return TRACK_TYPE_AUDIO; }
		void reset();
		float stretchFactor() { return ( 48000 / 29.97 ); }
	private:
		// member variable declerations
		std::list< Clip* >::iterator m_current;
};
	
} /* namespace nle */

#endif /* _AUDIO_TRACK_H_ */
