/*  Automation.H
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

#ifndef _AUTOMATION_POINT_H_
#define _AUTOMATION_POINT_H_

#include <stdint.h>

#include "Rect.H"

namespace nle
{

class Clip;

class AutomationPoint
{
	public:
		AutomationPoint( int64_t _x, unsigned int _y ) { x = _x; y = _y; }
		int64_t x;
		unsigned int y; // 0 - 100, 100: opaque, 0: transparent
		Rect getScreenRect( Clip* clip );
};

} /* namespace nle */
#endif /* _AUTOMATION_POINT_H_ */

