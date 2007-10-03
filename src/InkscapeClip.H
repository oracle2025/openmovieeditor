/*  InkscapeClip.H
 *
 *  Copyright (C) 2007 Richard Spindler <richard.spindler AT gmail.com>
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

#ifndef _INKSCAPE_CLIP_H_
#define _INKSCAPE_CLIP_H_
#include "FilterClip.H"
#include "IVideoReader.H"
#include "global_includes.H"
#include "VideoEffectClip.H"
class Fl_Shared_Image;
namespace nle
{

class InkscapeClip : public FilterClip, public VideoEffectClip
{
	public:
		InkscapeClip( Track* track, int64_t position, int64_t length = -1, int id = -1, ClipData* data = 0 );
		//InkscapeClip( Track* track, int64_t position, int64_t length = -1, int id = -1, xml_node );
		~InkscapeClip();
		int64_t length();
		frame_struct* getRawFrame( int64_t position, int64_t &position_in_file );
		bool ok() { return m_ok; }
		string filename() { return "src:builtin:InkscapeClip"; }
		void reset() {}
		int type() { return CLIP_TYPE_IMAGE; }
		inline IClipArtist* getArtist() { return m_artist; }
		using Clip::trimA;
		using Clip::trimB;
		int64_t trimA( int64_t trim );
		int64_t trimB( int64_t trim );
		int64_t fileLength();
		int w();
		int h();
		int aspectHeight() { return m_aspectHeight; }
		int aspectWidth() { return m_aspectWidth; }
		float aspectRatio() { return m_aspectRatio; }
		int analogBlank() { return 0; }
		ClipData* getClipData() { return vec_getClipData(); }
		int getActionCount() { return 1; }
		// vv- Index starts at 0, good old C-Style. ;-)
		const char* getActionName( int index )
			{ if ( index == 0 ) { return "Edit with Inkscape" } return 0; }
		void doAction( int index );
	private:
		Fl_Shared_Image* m_image;
		int64_t m_length;
		frame_struct m_frame;
		bool m_ok;
		string m_filename;
		IClipArtist* m_artist;
		int m_aspectHeight;
		int m_aspectWidth;
		float m_aspectRatio;
		string m_unique_id;
};

} /* namespace nle */

#endif /* _INKSCAPE_CLIP_H_ */
