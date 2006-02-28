/*  AudioClip.H
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

#ifndef _AUDIO_CLIP_H_
#define _AUDIO_CLIP_H_

#include "AudioClipBase.H"

#include "global_includes.H"

namespace nle
{

class IClipArtist;
	
typedef struct _auto_node {
	struct _auto_node *next;
	int64_t x;
	float y;
} auto_node;
	
class AudioClip : public AudioClipBase
{
	public:
		AudioClip( Track *track, int64_t position, IAudioFile* af, int64_t trimA = 0, int64_t trimB = 0 );
		~AudioClip();
		int64_t length();
		string filename();
		inline auto_node* getAutoPoints() { return m_automationPoints; }
		inline float basicLevel() { return m_basicLevel; }
		inline void basicLevel( float o ) { m_basicLevel = o; }
		bool has_automation() { return true; }
		int type() { return CLIP_TYPE_AUDIO; }
		inline IClipArtist* getArtist() { return m_artist; }
		// vv  What's this? I hate C++
		//inline int64_t trimA() { return Clip::trimA(); }
		//inline int64_t trimB() { return Clip::trimB(); }
		// vv "Fixed", yeah right 
		using Clip::trimA;
		using Clip::trimB;
		void trimA( int64_t trim );
		void trimB( int64_t trim );
		int fillBuffer( float* output, unsigned long frames, int64_t position );
		void reset();
		int64_t fileLength();
	protected:
		int64_t audioTrimA();
		int64_t audioTrimB();
		int64_t audioPosition();
	private:
		float getEnvelope( int64_t position );
		float m_basicLevel;
		auto_node* m_automationPoints;
		auto_node* m_autoCache;
		IClipArtist* m_artist;
};


} /* namespace nle */

#endif /* _AUDIO_CLIP_H_ */
