/*  Timeline.H
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

#ifndef _TIMELINE_H_
#define _TIMELINE_H_

#include <list>
#include <stdint.h>

#include "frame_struct.h"

namespace nle
{

class Track;
class VideoTrack;
class AudioTrack;

class Timeline
{
	public:
		// constructors and destructors
		Timeline();
		~Timeline();

		int64_t m_playPosition;
		int64_t m_samplePosition;
		// method declerations
		void reset();
		inline void seek( int64_t position ) { m_seekPosition = position; }
		frame_struct* nextFrame();
		frame_struct* getFrame( int64_t position );
		int fillBuffer( float* output, unsigned long frames );

		void add_video( int track, int64_t position, const char* filename );
		void add_audio( int track, int64_t position, const char* filename );
		std::list< Track* >* getTracks() { return &m_allTracks; }
		std::list< VideoTrack* >* getVideoTracks() { return &m_videoTracks; }
	private:
		// member variable declerations
		std::list< Track* > m_allTracks;
		std::list< VideoTrack* > m_videoTracks;
		std::list< AudioTrack* > m_audioTracks;
		int64_t m_seekPosition;
};
	
} /* namespace nle */

#endif /* _TIMELINE_H_ */
