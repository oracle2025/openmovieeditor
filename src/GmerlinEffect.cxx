/*  GmerlinEffect.cxx
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

#include "GmerlinEffect.H"
#include "LazyFrame.H"
#include "VideoEffectClip.H"
#include "GmerlinWidget.H"

#include <string>
#include <iostream>
		
namespace nle
{	

int read_video_gmerlin( void* data, gavl_video_frame_t * frame, int stream )
{
	GmerlinEffect* effect = (GmerlinEffect*)data;
	gavl_video_frame_copy( &effect->input_format, frame, effect->m_input_lazy_frame->RGBA() );
	return 1;
}

const char* gmerlin_parameter_type_string( int v )
{
	switch ( v ) {
		case BG_PARAMETER_SECTION:
			return "BG_PARAMETER_SECTION";
		case BG_PARAMETER_CHECKBUTTON:
			return "BG_PARAMETER_CHECKBUTTON";
		case BG_PARAMETER_INT :
			return "BG_PARAMETER_INT";
		case BG_PARAMETER_FLOAT:
			return "BG_PARAMETER_FLOAT";
		case BG_PARAMETER_SLIDER_INT :
			return "BG_PARAMETER_SLIDER_INT";
		case BG_PARAMETER_SLIDER_FLOAT :
			return "BG_PARAMETER_SLIDER_FLOAT";
		case BG_PARAMETER_STRING :
			return "BG_PARAMETER_STRING";
		case BG_PARAMETER_STRING_HIDDEN:
			return "BG_PARAMETER_STRING_HIDDEN";
		case BG_PARAMETER_STRINGLIST :
			return "BG_PARAMETER_STRINGLIST";
		case BG_PARAMETER_COLOR_RGB :
			return "BG_PARAMETER_COLOR_RGB";
		case BG_PARAMETER_COLOR_RGBA:
			return "BG_PARAMETER_COLOR_RGBA";
		case BG_PARAMETER_FONT :
			return "BG_PARAMETER_FONT";
		case BG_PARAMETER_DEVICE:
			return "BG_PARAMETER_DEVICE";
		case BG_PARAMETER_FILE :
			return "BG_PARAMETER_FILE";
		case BG_PARAMETER_DIRECTORY:
			return "BG_PARAMETER_DIRECTORY";
		case BG_PARAMETER_MULTI_MENU:
			return "BG_PARAMETER_MULTI_MENU";
		case BG_PARAMETER_MULTI_LIST:
			return "BG_PARAMETER_MULTI_LIST";
		case BG_PARAMETER_MULTI_CHAIN:
			return "BG_PARAMETER_MULTI_CHAIN";
		case BG_PARAMETER_TIME :
			return "BG_PARAMETER_TIME";
		case BG_PARAMETER_POSITION:
			return "BG_PARAMETER_POSITION";
	}
	return "";
}
		
GmerlinEffect::GmerlinEffect( const char* identifier, bg_plugin_handle_t* plugin_handle, VideoEffectClip* clip )
{
	m_plugin_handle = plugin_handle;
	m_identifier = identifier;
	m_filter = (const bg_fv_plugin_t*)m_plugin_handle->plugin;
	m_filter_instance = m_filter->common.create();
	m_parameters = m_filter->common.get_parameters( m_filter_instance );
	for ( int i = 0; m_parameters[i].name; i++ ) {
		m_filter->common.set_parameter( m_filter_instance, m_parameters[i].name, &m_parameters[i].val_default );
		std::cout << m_parameters[i].name << " : " << m_parameters[i].long_name << " : " << gmerlin_parameter_type_string( m_parameters[i].type ) << std::endl;
	}
	m_filter->common.set_parameter( m_filter_instance, 0, 0 );

	m_filter->connect_input_port( m_filter_instance, read_video_gmerlin, this, 0, 0 );

	input_format.frame_width  = clip->w();
	input_format.frame_height = clip->h();
	input_format.image_width  = clip->w();
	input_format.image_height = clip->h();
	input_format.pixel_width = 1;
	input_format.pixel_height = 1;
	input_format.pixelformat = GAVL_RGBA_32;
	input_format.interlace_mode = GAVL_INTERLACE_NONE;
	m_filter->set_input_format( m_filter_instance, &input_format, 0 );
	
	m_lazy_frame = new LazyFrame( clip->w(), clip->h() );
	m_gavl_frame = gavl_video_frame_create( m_lazy_frame->format() );
	m_lazy_frame->put_data( m_gavl_frame );

}
GmerlinEffect::~GmerlinEffect()
{
	m_filter->common.destroy( m_filter_instance );
	delete m_lazy_frame;
	gavl_video_frame_destroy( m_gavl_frame );
}
int GmerlinEffect::numberOfParams()
{
	m_parameters = m_filter->common.get_parameters( m_filter_instance );
	int i;
	for ( i = 0; m_parameters[i].name; i++ )
		; // empty
	return i;
}
LazyFrame* GmerlinEffect::getFrame( LazyFrame* frame, int64_t position )
{
	m_input_lazy_frame = frame;
	m_filter->read_video( m_filter_instance, m_gavl_frame, 0 );
	return m_lazy_frame;
}
const char* GmerlinEffect::name()
{
	return m_plugin_handle->info->long_name;
}
IEffectDialog* GmerlinEffect::dialog()
{
	return 0;
}
const char* GmerlinEffect::identifier()
{
	static char buffer[256];
	strncpy( buffer, m_identifier.c_str(), sizeof(buffer) - 1 );
	return buffer;
}
IEffectWidget* GmerlinEffect::widget()
{
	return new GmerlinWidget( this );
}
void GmerlinEffect::writeXML( TiXmlElement* xml_noden )
{
	/*
	TiXmlElement* parameter;
	TiXmlElement* effect = xml_node;

	for ( int i = 0; m_parameters[i].name; i++ ) {
		parameter = new TiXmlElement( "parameter" );
		effect->LinkEndChild( parameter );
		parameter->SetAttribute( "name", m_parameters[i].name );
		switch ( parameters[i].type ) {
			case BG_PARAMETER_SECTION:
			break;
			case BG_PARAMETER_CHECKBUTTON:
			case BG_PARAMETER_INT :
			case BG_PARAMETER_SLIDER_INT :
				{
					//get_paramater still unimplemented
					//ARGH!
				}
			case BG_PARAMETER_FLOAT:
			case BG_PARAMETER_SLIDER_FLOAT :
			case BG_PARAMETER_STRING :
			case BG_PARAMETER_STRING_HIDDEN:
			case BG_PARAMETER_STRINGLIST :
			case BG_PARAMETER_COLOR_RGB :
			case BG_PARAMETER_COLOR_RGBA:
			case BG_PARAMETER_FONT :
			case BG_PARAMETER_DEVICE:
			case BG_PARAMETER_FILE :
			case BG_PARAMETER_DIRECTORY:
			case BG_PARAMETER_MULTI_MENU:
			case BG_PARAMETER_MULTI_LIST:
			case BG_PARAMETER_MULTI_CHAIN:
			case BG_PARAMETER_TIME :
			case BG_PARAMETER_POSITION:

		}
	}
	*/

}
void GmerlinEffect::readXML( TiXmlElement* )
{
}


} /* namespace nle */
