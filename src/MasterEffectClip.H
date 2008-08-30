/*  MasterEffectClip.H
 *
 *  Copyright (C) 2008 Richard Spindler <richard.spindler AT gmail.com>
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

#ifndef _MASTER_EFFECT_CLIP_H_
#define _MASTER_EFFECT_CLIP_H_

#include "FilterClip.H"
#include "VideoEffectClip.H"

namespace nle
{

class MasterEffectClip : public FilterClip, public VideoEffectClip
{
	public:
		MasterEffectClip();
		~MasterEffectClip();
		int64_t length() { return 0; }
		int64_t fileLength() { return 0; }
		std::string filename() { return ""; }
		int type() { return CLIP_TYPE_VIDEO; }
		nle::ClipData* getClipData() { return 0; }
		int w() { return 720; }
		int h() { return 576; }
		LazyFrame* getRawFrame( int64_t position, int64_t &position_in_file ) { return m_frame; }
		LazyFrame* processFrame( LazyFrame* frame );
	private:
		LazyFrame* m_frame;
};


} /* namespace nle */

#endif /* _MASTER_EFFECT_CLIP_H_ */

