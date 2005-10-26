/*  VideoTrack.H
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

#ifndef _VIDEO_TRACK_H_
#define _VIDEO_TRACK_H_

#include "TrackBase.H"
#include "frame_struct.h"

namespace nle
{

class VideoClip;

class VideoTrack : public TrackBase
{
	public:
		// constructors and destructors
		VideoTrack( int num );
		~VideoTrack();

		// method declerations
		void addFile( int64_t position, const char* filename );
//		void add_video( int64_t position, const char* filename );
		int type() { return TRACK_TYPE_VIDEO; }
		float stretchFactor() { return 1.0; }
		frame_struct* getFrame( int64_t position );
		void sort();
	private:
		int64_t m_playPosition;
		// member variable declerations
};
	
} /* namespace nle */

#endif /* _VIDEO_TRACK_H_ */
