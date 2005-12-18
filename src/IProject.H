/*  IProject.H
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

#ifndef _I_PROJECT_H_
#define _I_PROJECT_H_

#include "global_includes.H"

namespace nle
{
	
class IProject
{
	public:
		virtual ~IProject() {}
		// return < 0 on failure
		virtual int write( string filename, string name ) = 0;
		virtual int read( string filename ) = 0;
		virtual void clear() = 0;
};

} /* namespace nle */

#endif /* _I_PROJECT_H_ */
