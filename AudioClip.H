#ifndef _AUDIO_CLIP_H_
#define _AUDIO_CLIP_H_

#include "Clip.h"

namespace nle
{
class AudioFileQT;

class AudioClip : public Clip
{
	public:
		AudioClip( Track *track, int64_t position, AudioFileQT* af );
		~AudioClip();
		int64_t length();
		void reset();
		int fillBuffer( float* output, unsigned long frames, int64_t position );
	private:
		AudioFileQT* m_audioFile;
};


} /* namespace nle */

#endif /* _AUDIO_CLIP_H_ */
