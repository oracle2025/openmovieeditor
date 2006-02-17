/*  AudioClipBase.H
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

#ifndef _AUDIO_CLIP_BASE_H_
#define _AUDIO_CLIP_BASE_H_

#include "timeline/Clip.H"

#include "global_includes.H"

namespace nle
{

class IAudioFile;

/*
 * This is the base class for both, Audio and Video Clips,
 * because Video Clips can handle Audio as well, since most
 * Video Files contain Audio as well, and this should be played
 * back seamlessly.
 */
class AudioClipBase : public Clip
{
	public:
		virtual ~AudioClipBase();
		virtual int fillBuffer( float* output, unsigned long frames, int64_t position );
		virtual void reset();
	protected:
		AudioClipBase( Track *track, int64_t position, IAudioFile* af = 0 );
		virtual int64_t audioTrimA() = 0;
		virtual int64_t audioTrimB() = 0;
		int64_t audioLength();
		virtual int64_t audioPosition() = 0;
		string audioFilename();
		IAudioFile* m_audioFile;
	private:
		int64_t m_lastSamplePosition;
};

} /* namespace nle */


#endif /* _AUDIO_CLIP_BASE_H_ */
