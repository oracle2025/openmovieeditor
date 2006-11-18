/*  VideoEffectClip.H
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

#ifndef _VIDEO_EFFECT_CLIP_H_
#define _VIDEO_EFFECT_CLIP_H_

#include <stdint.h>

#include "frame_struct.h"
#include "IVideoReader.H"

namespace nle
{

class AbstractEffectFactory;
class IVideoEffect;

typedef struct _effect_stack {
	struct _effect_stack* next;
	IVideoEffect* effect;
} effect_stack;

class VideoEffectClip : public IVideoReader
{
	public:
		~VideoEffectClip();
		frame_struct* getFrame( int64_t position );
		virtual frame_struct* getRawFrame( int64_t position, int64_t &position_in_file ) = 0;
		void pushEffect( AbstractEffectFactory* factory );
		IVideoEffect* appendEffect( AbstractEffectFactory* factory );
		void moveEffectUp( int num );
		void moveEffectDown( int num );
		void removeEffect( int num );
		effect_stack* getEffects() { return m_effects; }
		virtual int w() = 0;
		virtual int h() = 0;
	protected:
		VideoEffectClip() { m_effects = 0; }
	private:
		effect_stack* m_effects;
};

} /* namespace nle */

#endif /* _VIDEO_EFFECT_CLIP_H_ */
