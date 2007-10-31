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
#include "video_format.H"
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
		void prepareFormat( video_format* fmt );
		void unPrepareFormat();
		frame_struct** getFormattedFrameStack( int64_t position );
		frame_struct** getFrameStack( int64_t position );
		int fillBuffer( float* output, unsigned long frames );
		inline void seek( int64_t position ) { m_seekPosition = position; }
		inline void sampleseek( int mode, int64_t position )
		{
			if (mode) m_samplePosition = position; else m_samplePosition += position;
			/* m_playPosition = m_samplePosition*25/48000; m_seekPosition= m_playPosition;*/
		}
		inline int64_t soundLength() { return m_soundLength; }
		bool changed() { return m_changed; }
		void changing() { m_changed = true; }
		void saving() { m_changed = false; }
		int getTrackId();
		void clear();
		int write( std::string filename, std::string name );
		int read( std::string filename );
		int write_smil( std::string filename, int track );
		bool render_mode() { return m_render_mode; }
		void render_mode( bool b ) { m_render_mode = b; }
	private:
		int read_20061221_and_earlier( std::string filename );
		int64_t m_soundLength;
		bool m_changed;
		int m_trackId;
		bool m_render_mode;
};

extern Timeline* g_timeline;
	
} /* namespace nle */

#endif /* _TIMELINE_H_ */
