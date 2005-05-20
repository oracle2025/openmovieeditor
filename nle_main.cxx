/*  nle_main.cxx
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

#include <FL/Fl.H>

#include "nle.h"

int main( int argc, char** argv )
{
	NleUI nui;
	Fl::visual(FL_DOUBLE|FL_RGB);

	Fl::background2( 34, 52, 103 );
	Fl::background( 93, 93, 114 );
	Fl::foreground( 255, 255, 255 );
	Fl::set_color( FL_BLACK, 200, 200, 200 );
	  
	nui.show( argc, argv );
	return Fl::run();
}
