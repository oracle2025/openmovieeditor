/*  Draw.H
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

#ifndef _DRAW_H_
#define _DRAW_H_

namespace nle
{

class Draw
{
	public:
		static void box( float x, float y, float w, float h, Fl_Color c );
		static void triangle( float x, float y, bool direction );
/*		static void VideoClip();
		static void VideoTrack();
		static void VideoTrackBg();*/
};

} /* namespace nle */

#endif /* _DRAW_H_ */
