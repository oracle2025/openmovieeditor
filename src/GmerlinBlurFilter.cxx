/* GmerlinBlurFilter.cxx
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

#include "GmerlinBlurFilter.H"
#include "VideoEffectClip.H"
#include "timeline/Clip.H"
#include "LazyFrame.H"

namespace nle
{

GmerlinBlurFactory* g_gmerlinBlurFactory;

GmerlinBlurFactory::GmerlinBlurFactory()
{
	g_gmerlinBlurFactory = this;
}
GmerlinBlurFactory::~GmerlinBlurFactory()
{
}
FilterBase* GmerlinBlurFactory::get( Clip* clip )
{
	VideoEffectClip* effectClip = dynamic_cast<VideoEffectClip*>(clip);
	if ( !effectClip ) {
		return 0;
	}
	return new GmerlinBlurFilter( effectClip->w(), effectClip->h() );
}
const char* GmerlinBlurFactory::name()
{
	return "Blur";
}

//===========================================

int read_video_gmerlin_blur( void *data, gavl_video_frame_t *frame, int stream )
{
	GmerlinBlurFilter* filter = (GmerlinBlurFilter*)data;
	gavl_video_frame_copy( &filter->input_format, frame, filter->m_input_lazy_frame->RGBA() );
	return 1;
}

GmerlinBlurFilter::GmerlinBlurFilter( int w, int h )
{
	m_cfg_registry = bg_cfg_registry_create();
	m_plugin_cfg = bg_cfg_registry_find_section( m_cfg_registry, "plugins" );
	m_plugin_registry = bg_plugin_registry_create( m_plugin_cfg );
	m_blur_plugin_info = bg_plugin_find_by_name( m_plugin_registry, "fv_blur" );
	m_blur_plugin_handle = bg_plugin_load( m_plugin_registry, m_blur_plugin_info );

	m_blur_filter = (const bg_fv_plugin_t*)m_blur_plugin_handle->plugin;
	m_filter_instance = m_blur_filter->common.create();
	m_parameters = m_blur_filter->common.get_parameters( m_filter_instance );

	m_blur_filter->connect_input_port( m_filter_instance, read_video_gmerlin_blur, this, 0, 0 );
	
	input_format.frame_width  = w;
	input_format.frame_height = h;
	input_format.image_width  = w;
	input_format.image_height = h;
	input_format.pixel_width = 1;
	input_format.pixel_height = 1;
	input_format.pixelformat = GAVL_RGBA_32;
	input_format.interlace_mode = GAVL_INTERLACE_NONE;
	m_blur_filter->set_input_format( m_filter_instance, &input_format, 0 );
	
	m_lazy_frame = new LazyFrame( w, h );
	m_gavl_frame = gavl_video_frame_create( m_lazy_frame->format() );
	m_lazy_frame->put_data( m_gavl_frame );

	bg_parameter_value_t val;
	val.val_f = 5.0;
	m_blur_filter->common.set_parameter( m_filter_instance, "radius_h", &val );
	m_blur_filter->common.set_parameter( m_filter_instance, "radius_v", &val );

}
GmerlinBlurFilter::~GmerlinBlurFilter()
{
	m_blur_filter->common.destroy( m_filter_instance );
	bg_plugin_unref( m_blur_plugin_handle );
	bg_plugin_registry_destroy( m_plugin_registry );
	bg_cfg_registry_destroy( m_cfg_registry );
	delete m_lazy_frame;
	gavl_video_frame_destroy( m_gavl_frame );
}
LazyFrame* GmerlinBlurFilter::getFrame( LazyFrame* frame, int64_t position )
{
	m_input_lazy_frame = frame;
	m_blur_filter->read_video( m_filter_instance, m_gavl_frame, 0 );
	return m_lazy_frame;
}
const char* GmerlinBlurFilter::name()
{
	return "Blur";
}
IEffectDialog* GmerlinBlurFilter::dialog()
{
	return 0;
}
void GmerlinBlurFilter::writeXML( TiXmlElement* )
{
}
void GmerlinBlurFilter::readXML( TiXmlElement* )
{
}


} /* namespace nle */
