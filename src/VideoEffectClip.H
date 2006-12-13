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
		void prepareFormat( int w, int h ); // might add fps here
		frame_struct* getFormattedFrame( int64_t position );
		bool crop() { return m_crop; }
		bool fit() { return m_fit; }
		bool stretch() { return m_stretch; }
		bool def() { return m_default; }
		void crop( bool v ) { if ( v ) { m_fit = m_stretch = m_default = !(m_crop = true); m_render_strategy = RENDER_CROP;} }
		void fit( bool v ) { if ( v ) { m_crop = m_stretch = m_default = !(m_fit = true); m_render_strategy = RENDER_FIT;} }
		void stretch( bool v ) { if ( v ) { m_fit = m_crop = m_default = !(m_stretch = true); m_render_strategy = RENDER_STRETCH;} }
		void def( bool v ) { if ( v ) { m_fit = m_crop = m_stretch = !(m_default = true); m_render_strategy = RENDER_DEFAULT;} }
	protected:
		VideoEffectClip() { m_render_strategy = RENDER_DEFAULT; m_effects = 0; m_crop = m_fit = m_stretch = false; m_default = true; }
		bool m_crop;
		bool m_fit;
		bool m_stretch;
		bool m_default;
		render_strategy_t m_render_strategy;
	private:
		effect_stack* m_effects;
};

} /* namespace nle */

#endif /* _VIDEO_EFFECT_CLIP_H_ */
