/* Prefs.H
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

#ifndef _PREFS_H_
#define _PREFS_H_

#define KEY_STRING_LEN   50
#define VALUE_STRING_LEN 256


namespace nle
{


class Prefs
{
	public:
		Prefs();
		~Prefs();
		const char* getBrowserFolder();
		void setBrowserFolder( const char* filename );
	private:
		char m_browserFolder[1024];
		
};

} /* namespace nle */

#endif /* _PREFS_H_ */


