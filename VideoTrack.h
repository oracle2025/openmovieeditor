#ifndef _VIDEO_TRACK_H_
#define _VIDEO_TRACK_H_

#include "Track.h"

namespace nle
{

class VideoClip;

class VideoTrack : public Track
{
	public:
		// constructors and destructors
		VideoTrack( int num );
		~VideoTrack();

		// method declerations
		void add_video( int64_t position, const char* filename );
		int type() { return TRACK_TYPE_VIDEO; }
	private:
		// member variable declerations
};
	
} /* namespace nle */

#endif /* _VIDEO_TRACK_H_ */
