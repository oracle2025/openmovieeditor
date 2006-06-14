/*  ImageClip.H
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

#ifndef _IMAGE_CLIP_H_
#define _IMAGE_CLIP_H_

#include "timeline/Clip.H"
#include "IVideoReader.H"
#include "global_includes.H"

class Fl_Shared_Image;

namespace nle
{

class ImageClip : public Clip, public IVideoReader
{
	public:
		ImageClip( Track* track, int64_t position, string filename, int64_t length = -1, int id = -1 );
		~ImageClip();
		int64_t length();
		frame_struct* getFrame( int64_t position );
		bool ok() { return m_ok; }
		string filename() { return m_filename; }
		void reset() {}
		int type() { return CLIP_TYPE_IMAGE; }
		inline IClipArtist* getArtist() { return m_artist; }
		using Clip::trimA;
		using Clip::trimB;
		void trimA( int64_t trim );
		void trimB( int64_t trim );
		int64_t fileLength();
	private:
		Fl_Shared_Image* m_image;
		int64_t m_length;
		frame_struct m_frame;
		bool m_ok;
		string m_filename;
		IClipArtist* m_artist;
};

} /* namespace nle */

#endif /* _IMAGE_CLIP_H_ */

