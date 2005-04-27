#ifndef _SOUND_H_
#define _SOUND_H_

namespace nle
{

class NextFrameListener;
class Timeline;

class Sound
{
	public:
		Sound( NextFrameListener *nfl );
		~Sound();
		void Play();
		void Stop();
		int  fillBuffer( float* output, unsigned long frames );
	private:
		NextFrameListener *m_nfl;
		Timeline *m_tl;
		int64_t m_soundSamples;
		int64_t m_videoFrames;
};


} /* namespace nle */

#endif /* _SOUND_H_ */

