/*  Timeline.H
 *
 *  Copyright (C) 2005 Richard Spindler <richard.spindler AT gmail.com>
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

#ifndef _TIMELINE_H_
#define _TIMELINE_H_

#include <stdint.h>

#include "timeline/TimelineBase.H"
#include "frame_struct.h"
#include "IAudioReader.H"
#include "IVideoReader.H"

namespace nle
{

class Timeline : public TimelineBase, public IAudioReader, public IVideoReader
{
	public:
		Timeline();
		~Timeline();
		void sort();

		int64_t m_playPosition;
		int64_t m_samplePosition;
		int64_t m_seekPosition;
		frame_struct* nextFrame( int64_t position = -1 );
		frame_struct* getFrame( int64_t position );
		void getBlendedFrame( frame_struct* dst );
		void getBlendedFrame( int64_t position, frame_struct* dst );
		frame_struct** getFrameStack( int64_t position );
		int fillBuffer( float* output, unsigned long frames );
		inline void seek( int64_t position ) { m_seekPosition = position; }
		inline int64_t soundLength() { return m_soundLength; }
		bool changed() { return m_changed; }
		void changing() { m_changed = true; }
		void saving() { m_changed = false; }
	private:
		int64_t m_soundLength;
		bool m_changed;
};

} /* namespace nle */

#endif /* _TIMELINE_H_ */
