/*  Draw.cxx
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

#include <FL/Fl.H>
#include <FL/fl_draw.H>

#include "Draw.H"
#include "SwitchBoard.H"
#include "globals.H"

namespace nle
{

void Draw::box( float x, float y, float w, float h, Fl_Color c )
{
	fl_color( c );
	fl_begin_polygon();
	fl_vertex( x, y );
	fl_vertex( x, y + h);
	fl_vertex( x + w, y + h );
	fl_vertex( x + w, y );
	fl_end_polygon();
	fl_color( FL_BLACK );
	fl_begin_loop();
	fl_vertex( x, y );
	fl_vertex( x, y + h);
	fl_vertex( x + w, y + h );
	fl_vertex( x + w, y );
	fl_end_loop();

}
extern bool USING_AUDIO;
void Draw::triangle( float x, float y, bool direction )
{
	float _x;
	float zoom = SwitchBoard::i()->zoom();
	if ( USING_AUDIO ) {
		_x = direction ? 8 * ( 48000 / g_fps ) : - 8 * ( 48000 / g_fps );
	} else {
		_x = direction ? 8 : -8;
	}
	_x = _x / zoom;
	fl_color( FL_BLACK );
	fl_begin_polygon();
	fl_vertex( x, y );
	fl_vertex( x + _x, y - 5 );
	fl_vertex( x + _x, y + 5 );
	fl_end_polygon();
	
}


} /* namespace nle */
