/*  VideoClip.H
 *
 *  Copyright (C) 2003 Richard Spindler <richard.spindler AT gmail.com>
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

#include "Clip.H"
#include "frame_struct.h"

namespace nle
{
class IVideoFile;
class FilmStrip;

class VideoClip : public Clip
{
	public:
		VideoClip( Track *track, int64_t position, IVideoFile *vf );
		~VideoClip();
		int64_t length();
		void reset();
		frame_struct* getFrame( int64_t position );
		inline FilmStrip* getFilmStrip() { return m_filmStrip; }
	private:
		IVideoFile *m_videoFile;
		FilmStrip *m_filmStrip;
		frame_struct m_previewFrame;
		int64_t m_lastFramePosition;
};

} /* namespace nle */

#endif /* _VIDEO_CLIP_H_ */
