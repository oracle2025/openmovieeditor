/*  AudioFileMpeg3.H
 *
 *  Copyright (C) 2007 Richard Spindler <richard.spindler AT gmail.com>
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

#ifndef _AUDIO_FILE_MPEG3_H_
#define _AUDIO_FILE_MPEG3_H_

#include <stdint.h>

#include <libmpeg3.h>

#include "global_includes.H"
#include "IAudioFile.H"
#include "globals.H"

namespace nle
{

class AudioFileMpeg3 : public IAudioFile
{
	public:
		AudioFileMpeg3( string filename );
		~AudioFileMpeg3();
		void seek( int64_t position );
		int fillBuffer( float* output, unsigned long frames );
			// sizof(output) == frames * 2
			// File Format: 48000 stereo interleaved
	private:
		mpeg3_t* m_file;
		bool m_oneShot;
		float l_buffer[4096];
		float r_buffer[4096];
};

}

#endif /* _AUDIO_FILE_MPEG3_H_ */

