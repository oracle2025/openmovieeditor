/*  SwitchBoard.H
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

#ifndef _SWITCH_BOARD_H_
#define _SWITCH_BOARD_H_

namespace nle
{
class Timeline;
class TimelineView;

class SwitchBoard
{
	public:
		inline static SwitchBoard* i() //Singleton sucks, should be fixed,
		{
			static SwitchBoard inst;
			return &inst;
		}
		void zoom( float zoom );
		float zoom();
		void timeline( Timeline* tl );
		void timelineView( TimelineView* tlv );
		Timeline* timeline();
		void move_cursor();
	private:
		SwitchBoard();
		float m_zoom; // = 1.0;
		Timeline *m_timeline;
		TimelineView *m_tlv;
};

} /* namespace nle */


#endif /* _SWITCH_BOARD_H_ */
