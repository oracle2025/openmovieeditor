/* PlaybackCore.H
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

#ifndef _PLAYBACK_CORE_H_
#define _PLAYBACK_CORE_H_

#include <stdint.h>
#include <pthread.h>
#include <jack/ringbuffer.h>
#include "frame_struct.h"

union semun {
	int              val;    /* Value for SETVAL */
	struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
	unsigned short  *array;  /* Array for GETALL, SETALL */
	struct seminfo  *__buf;  /* Buffer for IPC_INFO
	                            (Linux specific) */
};

namespace nle
{

class IAudioReader;
class IVideoReader;
class IVideoWriter;

class PlaybackCore
{
	public:
		PlaybackCore( IAudioReader* audioReader, IVideoReader* videoReader, IVideoWriter* videoWriter );
		~PlaybackCore();
		void play();
		void stop();
		void flipScreen( int p );
		unsigned long fillBuffer( float* output, unsigned long frames );
		void videoThread();
		void audioThread();
	private:
		bool m_active;
		bool m_stop_playback;
		bool m_stop_playback_gui;
		int64_t m_soundSamples;
		int64_t m_videoFrames;
		int64_t m_videoFrames_protected;
		int m_semid;
		int m_xruns;
		int m_pipe[2];
		pthread_mutex_t m_audio_lock;
		pthread_cond_t m_audio_ready;
		pthread_mutex_t m_video_lock;
		pthread_cond_t m_video_ready;
		jack_ringbuffer_t* m_rb;
		pthread_t m_audio_thread;
		pthread_t m_video_thread;
		frame_struct *m_fs;
		IAudioReader* m_audioReader;
		IVideoReader* m_videoReader;
		IVideoWriter* m_videoWriter;

};

} /* namespace nle */


#endif /* _PLAYBACK_CORE_H_ */
