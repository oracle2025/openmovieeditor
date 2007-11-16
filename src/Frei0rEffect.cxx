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
#include "render_helper.H"

namespace nle
{

Frei0rEffect::Frei0rEffect( f0r_plugin_info_t* info, void* handle, int w, int h )
	: m_info( info )
{
	m_w = w;
	m_h = h;
	m_dialog = 0;
	f0r_construct = (f0r_construct_f)dlsym( handle, "f0r_construct" );
	f0r_destruct = (f0r_destruct_f)dlsym( handle, "f0r_destruct" );
	f0r_update = (f0r_update_f)dlsym( handle, "f0r_update" );
	f0r_get_param_info = (f0r_get_param_info_f)dlsym( handle, "f0r_get_param_info" );
	f0r_set_param_value = (f0r_set_param_value_f)dlsym( handle, "f0r_set_param_value" );
	f0r_get_param_value = (f0r_get_param_value_f)dlsym( handle, "f0r_get_param_value" );
if ( g_INTERLACING ) {
	m_instance = f0r_construct( w, h/2 );
} else {
	m_instance = f0r_construct( w, h );
}
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
	m_framestruct.tilt_x = 0;
	m_framestruct.tilt_y = 0;
	m_framestruct.scale_x = 1.0;
	m_framestruct.scale_y = 1.0;
	m_framestruct.crop_left = 0;
	m_framestruct.crop_right = 0;
	m_framestruct.crop_top = 0;
	m_framestruct.crop_bottom = 0;

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
	//TODO: Check if interlaced and if Filter needs separate fields, then
	//perform conversion
	m_framestruct.pixel_aspect_ratio = frame->pixel_aspect_ratio;
	m_framestruct.interlace_mode = frame->interlace_mode;
	m_framestruct.first_field = frame->first_field;
if ( g_INTERLACING ) {
	//if ( frame->interlace_mode == 1 ) {
	/*	if ( m_h > frame->h / 2 ) {
			m_h = m_h / 2;
			f0r_destruct( m_instance );
			m_instance = f0r_construct( m_w, m_h );
		}*/
	//}
	//This is seriously degrading performance!!
	//frame_to_fields( 1, frame->RGB, m_frame, frame->w, frame->h, frame->has_alpha_channel );
	if ( frame->has_alpha_channel ) {
		f0r_update( m_instance, position / (float)NLE_TIME_BASE, (uint32_t*)frame->RGB, (uint32_t*)m_frame );
		f0r_update( m_instance, position / (float)NLE_TIME_BASE,(uint32_t*)(frame->RGB+m_w*m_h*2), (uint32_t*)(m_frame+m_w*m_h*2) );
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
		f0r_update( m_instance, position / (float)NLE_TIME_BASE, (uint32_t*)(m_tmpFrame+m_w*m_h*2), (uint32_t*)(m_frame+m_w*m_h*2) );
	}
	//fields_to_frames( 1, m_tmpFrame, m_frame, frame->w, frame->h );
} else {
//   vv Old Code without deinterlaceing and interlacing
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
	TiXmlElement* parameter;
	TiXmlElement* effect = xml_node;/*new TiXmlElement( "effect" );
	xml_node->LinkEndChild( effect );
	effect->SetAttribute( "name", name() );*/
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
					parameter->SetAttribute( "value", (int)(bvalue >= 0.5) );
					break;
				}
			case F0R_PARAM_COLOR:
				{
					f0r_param_color_t cvalue;
					f0r_get_param_value( m_instance, &cvalue, i );
					parameter->SetDoubleAttribute( "r", cvalue.r );
					parameter->SetDoubleAttribute( "g", cvalue.g );
					parameter->SetDoubleAttribute( "b", cvalue.b );
					break;
				}
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
							int bval;
							f0r_param_bool bvalue;
							parameterXml->Attribute( "value", &bval );
							bvalue = (double)bval;
							setValue( &bvalue, i );
							break;
						}
					case F0R_PARAM_COLOR:
						{
							double r = 0;
							double g = 0;
							double b = 0;
							parameterXml->Attribute( "r", &r );
							parameterXml->Attribute( "g", &g );
							parameterXml->Attribute( "b", &b );
							f0r_param_color_t cvalue;
							cvalue.r = r;
							cvalue.g = g;
							cvalue.b = b;
							f0r_set_param_value( m_instance, &cvalue, i );
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
