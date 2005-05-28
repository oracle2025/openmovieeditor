/*  Track.H
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

#ifndef _TRACK_H_
#define _TRACK_H_

#include <list>

namespace nle
{
class Clip;

enum Track_Types {
	TRACK_TYPE_VIDEO = 0,
	TRACK_TYPE_AUDIO = 1
};

class Track
{
	public:
		virtual ~Track();
		std::list< Clip* >* getClips() { return &m_clips; }
		virtual int type() = 0;
		virtual float stretchFactor() = 0;
		virtual void reset();
		void add( Clip* clip );
		virtual void add( const char* filename, int64_t position )=0;
		void remove( Clip* clip );
		int num() { return m_num; }
	protected:
		Track( int num );
		std::list< Clip* > m_clips;
	private:
		int m_num;
};
	
} /* namespace nle */

#endif /* _TRACK_H_ */

