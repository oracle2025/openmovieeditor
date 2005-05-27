/*  AudioFileQT.H
 *
 *  Copyright (C) 2003 Richard Spindler <richard.spindler AT gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef _AUDIO_FILE_QT_H_
#define _AUDIO_FILE_QT_H_

#include <stdint.h>

//#include <quicktime/lqt.h>
#include <lqt/lqt.h>

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

