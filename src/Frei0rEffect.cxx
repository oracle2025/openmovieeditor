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
#include <tinyxml.h>

#include "frei0r.h"
#include "Frei0rEffect.H"
#include "global_includes.H"
#include "globals.H"
#include "Frei0rDialog.H"

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
	delete m_tmpFrame;
	delete m_frame;
	delete m_rows;
}
frame_struct* Frei0rEffect::getFrame( frame_struct* frame, int64_t position )
{
	if ( frame->has_alpha_channel ) {
		f0r_update( m_instance, position / (float)NLE_TIME_BASE, (uint32_t*)frame->RGB, (uint32_t*)m_frame );
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
		f0r_update( m_instance, position / (float)NLE_TIME_BASE, (uint32_t*)m_tmpFrame, (uint32_t*)m_frame );
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
IEffectDialog* Frei0rEffect::dialog()
{
	if ( !m_dialog ) {
		m_dialog = new Frei0rDialog( this );
	}
	return m_dialog;
}
void Frei0rEffect::writeXML( TiXmlElement* xml_node )
{
	TiXmlElement* effect = new TiXmlElement( "effect" );
	TiXmlElement* parameter;
	xml_node->LinkEndChild( effect );
	effect->SetAttribute( "name", name() );
	f0r_plugin_info_t* finfo;
	f0r_param_info_t pinfo;
	finfo = getPluginInfo();
	for ( int i = 0; i < finfo->num_params; i++ ) {
		getParamInfo( &pinfo, i );
		parameter = new TiXmlElement( "parameter" );
		effect->LinkEndChild( parameter );
		parameter->SetAttribute( "name", pinfo.name );
		switch ( pinfo.type ) {
			case F0R_PARAM_DOUBLE: //Seems to be always between 0.0 and 1.0
				{
					f0r_param_double dvalue;
					getValue( &dvalue, i );
					parameter->SetDoubleAttribute( "value", (double)dvalue );
					break;
				}
			case F0R_PARAM_BOOL:
				{
					f0r_param_bool bvalue;
					getValue( &bvalue, i );
					parameter->SetAttribute( "value", (int)bvalue );
					break;
				}
			case F0R_PARAM_COLOR:
				break;
			case F0R_PARAM_POSITION:
				{
					f0r_param_position_t pos;
					getValue( &pos, i );
					parameter->SetDoubleAttribute( "x", pos.x );
					parameter->SetDoubleAttribute( "y", pos.y );
					break;
				}
			default:
				break;

		}
	}



}
void Frei0rEffect::readXML( TiXmlElement* xml_node )
{
	
//	Frei0rEffect* effectObj = dynamic_cast<Frei0rEffect*>( c->appendEffect( ef ) );
	TiXmlElement* parameterXml = TiXmlHandle( xml_node ).FirstChildElement( "parameter" ).Element();
	for ( ; parameterXml; parameterXml = parameterXml->NextSiblingElement( "parameter" ) ) {
		string paramName = parameterXml->Attribute( "name" );
		f0r_plugin_info_t* finfo = getPluginInfo();
		f0r_param_info_t pinfo;
		for ( int i = 0; i < finfo->num_params; i++ ) {
			getParamInfo( &pinfo, i );
			if ( paramName == pinfo.name ) {
				switch ( pinfo.type ) {
					case F0R_PARAM_DOUBLE:
						{
							double dval;
							f0r_param_double dvalue;
							parameterXml->Attribute( "value", &dval );
							dvalue = dval;
							setValue( &dvalue, i );
							break;
						}
					case F0R_PARAM_BOOL:
						{
							//int bval;
							break;
						}
				}
				break;
			}
		}
	}
}

FilterData* Frei0rEffect::getFilterData()
{
	return 0;
}
void Frei0rEffect::setFilterData( FilterData* )
{
}
const char* Frei0rEffect::identifier()
{
	string result = "effect:frei0r:";
	result += name();
	return result.c_str(); //TODO: this is not OK?
}

} /* namespace nle */
