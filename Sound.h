#ifndef _SOUND_H_
#define _SOUND_H_

namespace nle
{


class Sound
{
	public:
		Sound();
		~Sound();
		void Play();
		void Stop();
		int  fillBuffer( float* output, unsigned long frames );
	private:
		int m_pipe[2];
};


} /* namespace nle */

#endif /* _SOUND_H_ */

