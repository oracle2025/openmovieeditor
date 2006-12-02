/* Frei0rEffect.cxx
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

#include "frei0r.h"
#include "Frei0rEffect.H"
#include "global_includes.H"
#include "IDeleteMe.H"

namespace nle
{

Frei0rEffect::Frei0rEffect( f0r_plugin_info_t* info, void* handle, int w, int h )
	: m_info( info )
{
	m_dialog = 0;
	f0r_construct = (f0r_construct_f)dlsym( handle, "f0r_construct" );
	f0r_destruct = (f0r_destruct_f)dlsym( handle, "f0r_destruct" );
	f0r_update = (f0r_update_f)dlsym( handle, "f0r_update" );
	f0r_get_param_info = (f0r_get_param_info_f)dlsym( handle, "f0r_get_param_info" );
	f0r_set_param_value = (f0r_set_param_value_f)dlsym( handle, "f0r_set_param_value" );
	f0r_get_param_value = (f0r_get_param_value_f)dlsym( handle, "f0r_get_param_value" );

	m_instance = f0r_construct( w, h );
	//create frame
	m_tmpFrame = new unsigned char[w * h * 4];
	m_frame = new unsigned char[w * h * 4];
	m_rows = new unsigned char*[h];
	for (int i = 0; i < h; i++) {
                m_rows[i] = m_frame + w * 4 * i;
	}
	m_framestruct.x = 0;
	m_framestruct.y = 0;
	m_framestruct.w = w;
	m_framestruct.h = h;
	m_framestruct.RGB = m_frame;
	m_framestruct.YUV = 0;
	m_framestruct.rows = m_rows;
	m_framestruct.alpha = 1.0;
	m_framestruct.has_alpha_channel = true;
	m_framestruct.cacheable = false;
}
Frei0rEffect::~Frei0rEffect()
{
	f0r_destruct( m_instance );
	if ( m_dialog ) {
		delete m_dialog;
	}
}
frame_struct* Frei0rEffect::getFrame( frame_struct* frame, int64_t position )
{
	if ( frame->has_alpha_channel ) {
		f0r_update( m_instance, position * 0.04, (uint32_t*)frame->RGB, (uint32_t*)m_frame );
	} else {
		int len = frame->w * frame->h * 3;
		unsigned char *src, *dst, *end;
		src = frame->RGB;
		dst = m_tmpFrame;
		end = frame->RGB + len;
		while ( src < end ) {
			dst[0] = src[0];
			dst[1] = src[1];
			dst[2] = src[2];
			dst[3] = 255;
			dst += 4;
			src += 3;
		}
		f0r_update( m_instance, position * 0.04, (uint32_t*)m_tmpFrame, (uint32_t*)m_frame );
	}
	return &m_framestruct;
}

void Frei0rEffect::getParamInfo( f0r_param_info_t *info, int param_index )
{
	f0r_get_param_info( info, param_index );
}

void Frei0rEffect::getValue( f0r_param_t param, int param_index )
{
	f0r_get_param_value( m_instance, param, param_index );
}

void Frei0rEffect::setValue( f0r_param_t param, int param_index )
{
	f0r_set_param_value( m_instance, param, param_index );
}
const char* Frei0rEffect::name()
{
	return m_info->name;
}
int Frei0rEffect::numParams()
{
	return m_info->num_params;
}

} /* namespace nle */
