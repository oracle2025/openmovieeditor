/*  GmerlinFactoryPlugin.cxx
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

#include "GmerlinFactoryPlugin.H"
#include "VideoEffectClip.H"
#include "GmerlinEffect.H"
#include "timeline/Clip.H"

#include <string>

namespace nle
{

GmerlinFactoryPlugin::GmerlinFactoryPlugin( const bg_plugin_info_t* plugin_info, bg_plugin_handle_t* plugin_handle )
{
	m_plugin_info = plugin_info;
	m_plugin_handle = plugin_handle;
}

GmerlinFactoryPlugin::~GmerlinFactoryPlugin()
{
	bg_plugin_unref( m_plugin_handle );
}

FilterBase* GmerlinFactoryPlugin::get( Clip* clip )
{
	VideoEffectClip* effectClip = dynamic_cast<VideoEffectClip*>(clip);
	if ( !effectClip ) {
		return 0;
	}
	GmerlinEffect* effect = new GmerlinEffect( m_plugin_info, m_plugin_handle, effectClip );
	return effect;
}

const char* GmerlinFactoryPlugin::name()
{
	return m_plugin_info->long_name;
}

const char* GmerlinFactoryPlugin::identifier()
{
	std::string result = "effect:gmerlin:";
	result += m_plugin_info->name;
	return result.c_str(); //TODO: this is not OK?
}

} /* namespace nle */

