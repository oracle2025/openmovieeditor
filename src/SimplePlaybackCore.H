/* SimplePlaybackCore.H
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

#ifndef _SIMPLE_PLAYBACK_CORE_H_
#define _SIMPLE_PLAYBACK_CORE_H_

#include <stdint.h>
#include <jack/ringbuffer.h>

namespace nle
{
class SimplePlaybackCore;
extern SimplePlaybackCore* g_simplePlaybackCore;
class IAudioReader;
class IVideoReader;
class IVideoWriter;


class SimplePlaybackCore
{
	public:
		SimplePlaybackCore( IAudioReader* audioReader, IVideoReader* videoReader, IVideoWriter* videoWriter );
		~SimplePlaybackCore();
		void play();
		void stop();
		inline bool active() { return m_active; }
		int readAudio( float* output, unsigned long frames );
		void fillBuffer( int p );
		void flipFrame();
		void checkPlayButton();
	private:
		IAudioReader* m_audioReader;
		IVideoReader* m_videoReader;
		IVideoWriter* m_videoWriter;
		bool m_active;
		int m_pipe[2];
		int64_t m_audioPosition;
		int64_t m_videoPosition;
		int64_t m_nextVideoFrame;
		jack_ringbuffer_t* m_rb;
};

} /* namespace nle */

#endif /* _SIMPLE_PLAYBACK_CORE_H_ */

