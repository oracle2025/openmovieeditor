#ifndef _VIDEO_CLIP_H_
#define _VIDEO_CLIP_H_

#include <stdint.h>
#include "Clip.h"
#include "frame_struct.h"
namespace nle
{
class IVideoFile;

class VideoClip : public Clip
{
	public:
//		VideoClip( Track *track, int64_t position, const char* filename );
		VideoClip( Track *track, int64_t position, IVideoFile *vf );
		~VideoClip();
		int64_t length();
		void reset();
		frame_struct* getFrame( int64_t position );
		/*VideoFrame *getFrame( VideoFrame* frame, int64_t position ) */
	private:
//		int64_t m_length;
		IVideoFile *m_videoFile;
};

} /* namespace nle */

#endif /* _VIDEO_CLIP_H_ */
