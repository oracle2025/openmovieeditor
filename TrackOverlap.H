/*  TrackOverlap.H
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

#ifndef _TRACK_OVERLAP_H_
#define _TRACK_OVERLAP_H_
namespace nle
{

class TrackOverlap {
	public:
		TrackOverlap( int64_t start, int64_t end )
			{ m_start = start; m_end = m_end; }
		void get( int64_t& start, int64_t& end )
			{ start = m_start; end = m_end; }
	private:
		int64_t m_start;
		int64_t m_end;
};


} /* namespace nle */

#endif /* _TRACK_OVERLAP_H_ */

