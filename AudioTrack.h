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
		void add_audio( int64_t position, const char* filename );
		int type() { return TRACK_TYPE_AUDIO; }
	private:
		// member variable declerations
};
	
} /* namespace nle */

#endif /* _AUDIO_TRACK_H_ */
