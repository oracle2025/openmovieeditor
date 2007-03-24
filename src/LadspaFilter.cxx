/*  LadspaFilter.cxx
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

#include "LadspaFilter.H"

namespace nle
{

LadspaFilter::LadspaFilter( void* handle )
{
	(LADSPA_Descriptor_Function)dlsym( setup->libhandle, "ladspa_descriptor" );

	/*
	 * Ladspa has a plugin descriptor that is some kind of struct that
	 * contains various fields with further information, including the
	 * functions to call.
	 *
	 * Concept: Start with simple implementation for stereo plugins.
	 *
	 * Check MPlayer ladspa loader
	 */
}

} /* namespace nle */
