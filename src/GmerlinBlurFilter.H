/* GmerlinBlurFilter.H
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


#ifndef _GMERLIN_BLUR_FILTER_H_
#define _GMERLIN_BLUR_FILTER_H_

#include "IVideoEffect.H"
#include "FilterFactory.H"

#include <gavl/gavl.h>
extern "C" {
#include <gmerlin/pluginregistry.h>
}
#define GMERLIN_BLUR_IDENTIFIER "effect:gmerlin:fv_blur"

namespace nle
{

class GmerlinBlurFactory;
extern GmerlinBlurFactory* g_gmerlinBlurFactory;

class GmerlinBlurFactory : public FilterFactory
{
	public:
		GmerlinBlurFactory();
		~GmerlinBlurFactory();
		FilterBase* get( Clip* clip );
		const char* name();
		const char* identifier() { return GMERLIN_BLUR_IDENTIFIER; }
};

class GmerlinBlurFilter : public IVideoEffect
{
	public:
		GmerlinBlurFilter( int w, int h );
		~GmerlinBlurFilter();
		LazyFrame* getFrame( LazyFrame* frame, int64_t position = -1 );
		const char* name();
		int numParams() { return 6; }
		IEffectDialog* dialog();
		IEffectDialog* m_dialog;
		const char* identifier() { return GMERLIN_BLUR_IDENTIFIER; }
		FilterData* getFilterData() { return 0; }
		void setFilterData( FilterData* ) {}
		void writeXML( TiXmlElement* );
		void readXML( TiXmlElement* );
		LazyFrame* m_input_lazy_frame;
		gavl_video_format_t input_format;
	private:
		bg_cfg_registry_t* m_cfg_registry;
		bg_cfg_section_t* m_plugin_cfg;
		bg_plugin_registry_t* m_plugin_registry;
		const bg_plugin_info_t* m_blur_plugin_info;
		bg_plugin_handle_t* m_blur_plugin_handle;
		const bg_fv_plugin_t* m_blur_filter;
		void* m_filter_instance;
		const bg_parameter_info_t* m_parameters;
		LazyFrame* m_lazy_frame;
		gavl_video_frame_t* m_gavl_frame;
};

} /* namespace nle */

#endif /* _GMERLIN_BLUR_FILTER_H_ */
