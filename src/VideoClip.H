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

class VideoClip : public AudioClipBase, public IVideoReader
{
	public:
		VideoClip( Track* track, int64_t position, IVideoFile* vf );
		~VideoClip();
		int64_t length();
		void reset();
		frame_struct* getFrame( int64_t position );
		inline FilmStrip* getFilmStrip() { return m_filmStrip; }
		string filename();
		inline IVideoFile* file() { return m_videoFile; }
		int type() { return CLIP_TYPE_VIDEO; }
		inline IClipArtist* getArtist() { return m_artist; }
	protected:
		int64_t audioTrimA();
		int64_t audioTrimB();
		int64_t audioPosition();
	private:
		IVideoFile* m_videoFile;
		FilmStrip* m_filmStrip;
		frame_struct m_previewFrame;
		int64_t m_lastFramePosition;

		int64_t m_audioPosition;
		bool m_noAudio;
		IClipArtist* m_artist;
};

} /* namespace nle */

#endif /* _VIDEO_CLIP_H_ */
