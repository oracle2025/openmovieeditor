/* UniPlaybackCore.H
 *
 *  Copyright (C) 2006 Richard Spindler <richard.spindler AT gmail.com>
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

/*
 * This is a single threaded approach to a playbackCore
 */

#ifndef _UNI_PLAYBACK_CORE_H_
#define _UNI_PLAYBACK_CORE_H_

#include "frame_struct.h"

namespace nle
{
class UniPlaybackCore;
extern UniPlaybackCore* g_uniPlaybackCore;

class IAudioReader;
class IVideoReader;
class IVideoWriter;

class UniPlaybackCore
{
	public:
		UniPlaybackCore( IAudioReader* audioReader, IVideoReader* videoReader, IVideoWriter* videoWriter );
		~UniPlaybackCore();
		void play();
		void stop();
		inline bool active() { return m_active; }
		void writeAudio();
		void flipVideo();
		void readVideo();
	private:
		IAudioReader* m_audioReader;
		IVideoReader* m_videoReader;
		IVideoWriter* m_videoWriter;
		bool m_active;
		int64_t m_audioPosition;
		int64_t m_videoPosition;
		int64_t m_nextVideoFrame;
		frame_struct** m_fs;
		int m_fd;
		void *m_handle;
};


} /* namespace nle */

#endif /* _UNI_PLAYBACK_CORE_H_ */
