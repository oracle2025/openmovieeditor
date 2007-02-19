/* FilterBase.H
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

#ifndef _FILTER_BASE_
#define _FILTER_BASE_

#include <stdint.h>

namespace nle
{

class DragHandler;

class FilterBase
{
	public:
		virtual ~FilterBase() {}
		virtual void reset() {}
		virtual int64_t trimA( int64_t trim ) { return trim; }
		virtual int64_t trimB( int64_t trim ) { return trim; }
		virtual void onDraw() {}
		virtual DragHandler* onMouseDown() { return 0; }
		//writeXML
		//readXML
		//trimA
		//trimB
};
	
} /* namespace nle */

#endif /* _FILTER_BASE_ */
