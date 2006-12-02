/*  SpecialClipsBrowser.cxx
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

#include <cstring>

#include <FL/Fl.H>

#include "SpecialClipsBrowser.H"

namespace nle
{
	
SpecialClipsBrowser::SpecialClipsBrowser( int x, int y, int w, int h, const char *l )
	: Fl_Browser( x, y, w, h, l )
{
}

int SpecialClipsBrowser::handle( int event )
{
	if ( event == FL_DRAG ) {
		Fl::copy( "TitleClip", strlen("TitleClip") + 1, 0 );
		Fl::dnd();
		return 1;
	}
	return Fl_Browser::handle( event );
}

} /* namespace nle */
