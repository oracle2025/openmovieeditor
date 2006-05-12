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
#include <jack/jack.h>

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
		void jackreadAudio( void *outL, void *outR, jack_nframes_t position, jack_nframes_t nframes );
		void flipFrame();
		void deactivate() { m_active=false; } /* deactivate() use only for error abort - else use stop */
		void checkPlayButton();
	private:
		IAudioReader* m_audioReader;
		IVideoReader* m_videoReader;
		IVideoWriter* m_videoWriter;
		bool m_active;
		int64_t m_audioPosition;
		int64_t m_currentFrame;
		int64_t m_lastFrame;
};

} /* namespace nle */

#endif /* _SIMPLE_PLAYBACK_CORE_H_ */

