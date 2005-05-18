#ifndef _AUDIO_FILE_QT_H_
#define _AUDIO_FILE_QT_H_

#include <quicktime/lqt.h>
#include <stdint.h>

namespace nle
{

class AudioFileQT
{
	public:
		AudioFileQT( const char* filename );
		~AudioFileQT();
		bool ok();
		inline int64_t length() { return m_length; } // number of floats: 2 * length (left, right)
		void seek( int64_t sample );
		int fillBuffer( float* output, unsigned long frames );
			//sizof(output) = frames * 2
			//File Format: 44100 stereo interleaved
	private:
		quicktime_t* m_qt;
		int64_t m_length;
		bool m_ok;
		bool m_oneShot;
};

}

#endif /* _AUDIO_FILE_QT_H_ */

