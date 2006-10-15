/* Frei0rFactoryPlugin.cxx
 *
 *  Copyright (C) 2006 Richard Spindler <richard.spindler AT gmail.com>
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

#include <dlfcn.h>
#include "Frei0rFactoryPlugin.H"
#include "Frei0rEffect.H"

namespace nle
{

Frei0rFactoryPlugin::Frei0rFactoryPlugin( string filename )
{
	m_handle = dlopen( filename.c_str(), RTLD_NOW );
	assert( m_handle );
	
	f0r_init = (f0r_init_f)dlsym( m_handle, "f0r_init" );
	assert( f0r_init ); //TODO: assert is not a good idea, ok() might be better
	
	f0r_deinit = (f0r_deinit_f)dlsym( m_handle, "f0r_deinit" );
	assert( f0r_deinit );
	
	f0r_get_plugin_info = (f0r_get_plugin_info_f)dlsym( m_handle, "f0r_get_plugin_info");
	
	if ( f0r_init() == 0 ) {
		assert( 0 );
	}
	f0r_get_plugin_info( &m_info );
	
}

Frei0rFactoryPlugin::~Frei0rFactoryPlugin()
{
	f0r_deinit();
	if ( m_handle ) {
		dlclose( m_handle );
	}
}

IVideoEffect* Frei0rFactoryPlugin::get( IVideoReader* reader, int w, int h )
{
	Frei0rEffect* effect = new Frei0rEffect( &m_info, m_handle, reader, w, h );
	return effect;
}

} /* namespace nle */
