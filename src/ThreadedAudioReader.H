/*  ThreadedAudioReader.H
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

#include <pthread.h>
#include <jack/ringbuffer.h>

namespace nle
{

class IAudioFile;
	
class ThreadedAudioReader
{
	public:
		ThreadedAudioReader( IAudioFile* );
		~ThreadedAudioReader();
		int fillBuffer( float* output, unsigned long frames );
		void seek( int64_t sample );
		void run();
	private:
		bool m_running;
		pthread_mutex_t m_readMutex;
		pthread_cond_t m_readCondition;
		pthread_t m_thread;
		jack_ringbuffer_t* m_ringBuffer;
		IAudioFile* m_file;
		float* m_buffer;
		bool m_fileDone;
		bool m_seek;
		int64_t m_sample;
		bool m_seekPending;
		int64_t m_samplePending;
};

} /* namespace nle */
