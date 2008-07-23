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

#include <string>
		
namespace nle
{	

int read_video_gmerlin( void* data, gavl_video_frame_t * frame, int stream )
{
	GmerlinEffect* effect = (GmerlinEffect*)data;
	gavl_video_frame_copy( &effect->input_format, frame, effect->m_input_lazy_frame->RGBA() );
	return 1;
}
		
GmerlinEffect::GmerlinEffect( const bg_plugin_info_t* plugin_info, bg_plugin_handle_t* plugin_handle, VideoEffectClip* clip )
{
	m_plugin_info = plugin_info;
	m_plugin_handle = plugin_handle;
	m_filter = (const bg_fv_plugin_t*)m_plugin_handle->plugin;
	m_filter_instance = m_filter->common.create();
	m_parameters = m_filter->common.get_parameters( m_filter_instance );

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
LazyFrame* GmerlinEffect::getFrame( LazyFrame* frame, int64_t position )
{
	m_input_lazy_frame = frame;
	m_filter->read_video( m_filter_instance, m_gavl_frame, 0 );
	return m_lazy_frame;
}
const char* GmerlinEffect::name()
{
	return m_plugin_info->long_name;
}
IEffectDialog* GmerlinEffect::dialog()
{
	return 0;
}
const char* GmerlinEffect::identifier()
{
	std::string result = "effect:gmerlin:";
	result += m_plugin_info->name;
	return result.c_str(); //TODO: this is not OK?
}
void GmerlinEffect::writeXML( TiXmlElement* )
{
}
void GmerlinEffect::readXML( TiXmlElement* )
{
}


} /* namespace nle */
