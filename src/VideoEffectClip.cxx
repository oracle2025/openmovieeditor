/*  VideoEffectClip.cxx
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

#include <cassert>
#include <gavl/gavl.h>
#include "sl/sl.h"
#include "global_includes.H"

#include "VideoEffectClip.H"
#include "FilterFactory.H"
#include "IVideoEffect.H"
#include "VideoClip.H"
#include "ImageClip.H"
#include "TitleClip.H"
#include "render_helper.H"
#include "frei0r.h"
#include "Frei0rEffect.H"
#include "Frei0rFactory.H"
#include "globals.H"
#include "IVideoFile.H"
#include "XmlClipData.H"
#include "MainFilterFactory.H"
#include <tinyxml.h>

namespace nle
{
extern Frei0rFactory* g_frei0rFactory;
struct effect_parameter {
	string name;
	f0r_param_bool param_bool;
	f0r_param_double param_double;
	f0r_param_color_t param_color;
	f0r_param_position_t param_position;
	f0r_param_string* param_string;
};
struct effect_data {
	string name;
	struct effect_parameter* parameters;
};

class EffectClipData : public ClipData
{
	public:
		~EffectClipData() {
			for ( int i = 0; i < effect_count; i++ ) {
				delete [] effects[i].parameters;
			}
			if ( effects ) {
				delete [] effects;
			}
		}
		struct effect_data* effects;
		int effect_count;
};

VideoEffectClip::VideoEffectClip( FilterClip* filterClip )
{
	m_filterClip = filterClip;
	m_render_strategy = RENDER_DEFAULT;
	//m_effects = 0;
	m_crop = m_fit = m_stretch = false;
	m_default = true;
	m_video_converter = 0;
	m_frame_src = 0;
	m_frame_dst = 0;
}
void VideoEffectClip::setEffects( ClipData* clip_data )
{
	if ( XmlClipData* xml = dynamic_cast<XmlClipData*>(clip_data) ) {
		TiXmlElement* filterXml = TiXmlHandle( xml->m_xml ).FirstChildElement( "filter" ).Element();
		for ( ; filterXml; filterXml = filterXml->NextSiblingElement( "filter" ) ) {
			FilterFactory* ff = g_mainFilterFactory->get( filterXml->Attribute( "identifier" ) );
			if ( ff ) {
				FilterBase* filter = m_filterClip->appendFilter( ff );
				filter->readXML( filterXml );
			}
		}
	}

}
VideoEffectClip::~VideoEffectClip()
{
	unPrepareFormat();
}
frame_struct* VideoEffectClip::getFrame( int64_t position )
{
	int64_t position_in_file = 0;
	frame_struct *f = getRawFrame( position, position_in_file );
	if ( !f ) {
		return 0;
	}
	filter_stack *node = m_filterClip->getFilters();
	IVideoEffect* effect;
	while ( node ) {
		effect = dynamic_cast<IVideoEffect*>( node->filter );
		node = node->next;
		if ( !effect ) { continue; }
		f = effect->getFrame( f, position_in_file );
	}
	// TODO: Copy pixel aspect
	f->render_strategy = m_render_strategy;
	return f;

}

void VideoEffectClip::prepareFormat( video_format* fmt )
{
	VideoClip* videoclip = dynamic_cast<VideoClip*>(this);
	if ( fmt->w == w() && fmt->h == h() ) {
		if ( !videoclip ) {
			return;
		} else {
			IVideoFile* videofile = videoclip->file();
			if ( videofile->interlacing() == fmt->interlacing ) {
				return;
			}
		}
	}
	if ( m_video_converter ) {
		gavl_video_converter_destroy( m_video_converter );
		m_video_converter = 0;
	}
	gavl_pixelformat_t colorspace = GAVL_RGBA_32;
	m_bits = 4;
	
	{
		frame_struct* f = getFrame( dynamic_cast<Clip*>(this)->position() );
		if ( f->has_alpha_channel ) {
			colorspace = GAVL_RGBA_32;
			m_bits = 4;
		} else {
			colorspace = GAVL_RGB_24;
			m_bits = 3;
		}
	}
 
	m_video_converter = gavl_video_converter_create();
	gavl_video_options_t* options = gavl_video_converter_get_options( m_video_converter );

	gavl_video_format_t format_src;

	m_format_dst.frame_width  = fmt->w;
	m_format_dst.frame_height = fmt->h;
	m_format_dst.image_width  = fmt->w;
	m_format_dst.image_height = fmt->h;
	m_format_dst.pixel_width = 1;
	m_format_dst.pixel_height = 1;
	m_format_dst.pixelformat = colorspace;
	m_format_dst.interlace_mode = GAVL_INTERLACE_NONE;

	if ( m_format_dst.frame_width == 720 && m_format_dst.frame_height == 576 ) {
		m_format_dst.pixel_width = 197;
		m_format_dst.pixel_height = 180;
	}
	
	format_src.frame_width  = w();
	format_src.frame_height = h();
	format_src.image_width  = w();
	format_src.image_height = h();
	format_src.pixel_width = 1;
	format_src.pixel_height = 1;
	format_src.pixelformat = colorspace;
	format_src.interlace_mode = GAVL_INTERLACE_NONE;

	if ( format_src.frame_width == 720 && format_src.frame_height == 576 ) {
		format_src.pixel_width = 128;
		format_src.pixel_height = 117;
	}
	if ( videoclip ) {
		IVideoFile* videofile = videoclip->file();
		switch ( videofile->interlacing() ) {
			case INTERLACE_TOP_FIELD_FIRST:
				format_src.interlace_mode = GAVL_INTERLACE_TOP_FIRST;
				break;
			case INTERLACE_BOTTOM_FIELD_FIRST:
				format_src.interlace_mode = GAVL_INTERLACE_BOTTOM_FIRST;
				break;
		}
	}
	switch ( fmt->interlacing ) {
		case INTERLACE_TOP_FIELD_FIRST:
			m_format_dst.interlace_mode = GAVL_INTERLACE_TOP_FIRST;
			break;
		case INTERLACE_BOTTOM_FIELD_FIRST:
			m_format_dst.interlace_mode = GAVL_INTERLACE_BOTTOM_FIRST;
			break;
	}

	gavl_rectangle_f_t src_rect;
	gavl_rectangle_i_t dst_rect;
	gavl_video_options_set_deinterlace_mode( options, GAVL_DEINTERLACE_SCALE );

	gavl_rectangle_f_set_all( &src_rect, &format_src );
	gavl_rectangle_i_set_all( &dst_rect, &m_format_dst );
	if ( m_render_strategy == RENDER_CROP ) {
		rectangle_crop_aspect( &dst_rect,
				&format_src,
				&src_rect,
				&m_format_dst
				);
	} else if ( m_render_strategy == RENDER_FIT ) {
		gavl_rectangle_fit_aspect( &dst_rect,
				&format_src,
				&src_rect,
				&m_format_dst,
				1.0,
				0.0
				);
	}


	gavl_video_options_set_rectangles( options, &src_rect, &dst_rect );
	if ( gavl_video_converter_init( m_video_converter, &format_src, &m_format_dst ) == -1 ) {
		cerr << "Video Scaler Init failed" << endl;
		return;
	}
	
	if ( m_frame_src ) {
		gavl_video_frame_null( m_frame_src );
		gavl_video_frame_destroy( m_frame_src );
		m_frame_src = 0;
	}
	if ( m_frame_dst ) {
		gavl_video_frame_null( m_frame_dst );
		gavl_video_frame_destroy( m_frame_dst );
		m_frame_dst = 0;
	}
	m_frame_src = gavl_video_frame_create( 0 );
	m_frame_src->strides[0] = w() * m_bits;
	m_frame_dst = gavl_video_frame_create( 0 );
	m_frame_dst->strides[0] = fmt->w * m_bits;

}
void VideoEffectClip::unPrepareFormat()
{
	if ( m_video_converter ) {
		gavl_video_converter_destroy( m_video_converter );
		m_video_converter = 0;
	}
	if ( m_frame_src ) {
		gavl_video_frame_null( m_frame_src );
		gavl_video_frame_destroy( m_frame_src );
		m_frame_src = 0;
	}
	if ( m_frame_dst ) {
		gavl_video_frame_null( m_frame_dst );
		gavl_video_frame_destroy( m_frame_dst );
		m_frame_dst = 0;
	}


}

frame_struct* VideoEffectClip::getFormattedFrame( frame_struct* tmp_frame, int64_t position )
{
	frame_struct* f = getFrame( position );
	if ( !f ) {
		return 0;
	}
	if ( m_video_converter ) {
		m_frame_src->planes[0] = f->RGB;
		m_frame_dst->planes[0] = tmp_frame->RGB;
		tmp_frame->has_alpha_channel = ( m_bits == 4 );
		tmp_frame->alpha = 1.0;
		gavl_video_frame_clear( m_frame_dst, &m_format_dst );
		gavl_video_convert( m_video_converter, m_frame_src, m_frame_dst );
		return tmp_frame;
	} else {
		return f;
	}
}
	
ClipData* VideoEffectClip::vec_getClipData()
{
	XmlClipData* xml = new XmlClipData;
	xml->m_xml = new TiXmlElement( "clip_data" );
	filter_stack* filters;
	for ( filters = m_filterClip->getFilters(); filters; filters = filters->next ) {
		TiXmlElement* filter_xml = new TiXmlElement( "filter" );
		xml->m_xml->LinkEndChild( filter_xml );
		filter_xml->SetAttribute( "name", filters->filter->name() );
		filter_xml->SetAttribute( "identifier", filters->filter->identifier() );
		filters->filter->writeXML( filter_xml );
	}
	return xml;
}


} /* namespace nle */
