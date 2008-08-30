/*  MasterEffectClip.cxx
 *
 *  Copyright (C) 2008 Richard Spindler <richard.spindler AT gmail.com>
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

#include "MasterEffectClip.H"

namespace nle
{
MasterEffectClip::MasterEffectClip()
	: FilterClip( 0, 0, -1 ), VideoEffectClip( this )
{
	m_frame = 0;
}
MasterEffectClip::~MasterEffectClip()
{
}
LazyFrame* MasterEffectClip::processFrame( LazyFrame* frame )
{
	m_frame = frame;
	LazyFrame* f = getFrame( -1 );
	m_frame = 0;
	return f;
}

} /* namespace nle */
