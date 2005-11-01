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

class AudioClip : public AudioClipBase
{
	public:
		AudioClip( Track *track, int64_t position, IAudioFile* af );
		~AudioClip();
		int64_t length();
		string filename();
	protected:
		int64_t audioTrimA();
		int64_t audioTrimB();
		int64_t audioPosition();
};


} /* namespace nle */

#endif /* _AUDIO_CLIP_H_ */
