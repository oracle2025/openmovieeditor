/*  AudioTrack.H
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

#ifndef _AUDIO_TRACK_H_
#define _AUDIO_TRACK_H_

#include "timeline/Track.H"
#include "globals.H"

namespace nle
{

class AudioClip;

class AudioTrack : public Track
{
	public:
		// constructors and destructors
		AudioTrack( int num );
		~AudioTrack();

		// method declerations
		int fillBuffer( float* output, unsigned long frames, int64_t position );
		void addFile( const char* filename, int64_t position );
		int type() { return TRACK_TYPE_AUDIO; }
		void reset();
		float stretchFactor() { return ( 48000 / g_fps ); }
	private:
		// member variable declerations
		clip_node* m_current;
};
	
} /* namespace nle */

#endif /* _AUDIO_TRACK_H_ */
