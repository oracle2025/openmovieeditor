/*  VideoClip.H
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

#ifndef _VIDEO_CLIP_H_
#define _VIDEO_CLIP_H_

#include <stdint.h>

#include "AudioClipBase.H"
#include "frame_struct.h"
#include "IVideoReader.H"

namespace nle
{
class IVideoFile;
class FilmStrip;
class IClipArtist;
class IVideoEffect;
class AbstractEffectFactory;

typedef struct _effect_stack {
	struct _effect_stack* next;
	IVideoEffect* effect;
} effect_stack;

class VideoClip : public AudioClipBase, public IVideoReader
{
	public:
		VideoClip( Track* track, int64_t position, IVideoFile* vf, int64_t A = 0, int64_t B = 0, int id = -1 );
		~VideoClip();
		int64_t length();
		void reset();
		frame_struct* getFrame( int64_t position );
		inline FilmStrip* getFilmStrip() { return m_filmStrip; }
		string filename();
		inline IVideoFile* file() { return m_videoFile; }
		int type() { return CLIP_TYPE_VIDEO; }
		inline IClipArtist* getArtist() { return m_artist; }
		bool hasAudio();
		int64_t fileLength();
		IVideoEffect* appendEffect( AbstractEffectFactory* factory );
		void pushEffect( AbstractEffectFactory* factory );
		void popEffect();
		void moveEffectUp( int num );/* Counting starts with 1 */
		void moveEffectDown( int num );
		void removeEffect( int num );
		effect_stack* getEffects() { return m_effects; }
//		IVideoReader* getEffect() { return m_effectReader; }
	protected:
		int64_t audioTrimA();
		int64_t audioTrimB();
		int64_t audioPosition();
		int64_t maxAudioLength();
	private:
		IVideoFile* m_videoFile;
		FilmStrip* m_filmStrip;
		frame_struct m_previewFrame;

		int64_t m_audioPosition;
		bool m_noAudio;
		IClipArtist* m_artist;
		IVideoReader* m_effectReader;
		effect_stack* m_effects;
};

} /* namespace nle */

#endif /* _VIDEO_CLIP_H_ */
