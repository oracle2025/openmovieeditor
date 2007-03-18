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
	m_video_scaler = 0;
	m_frame_src = 0;
	m_frame_dst = 0;
}
void VideoEffectClip::setEffects( ClipData* )
{
	return;
/*	EffectClipData* edata = dynamic_cast<EffectClipData*>(data);
	if ( edata ) {
		FilterFactory* ef;
		for ( int i = 0; i < edata->effect_count; i++ ) {
			ef = g_frei0rFactory->get( edata->effects[i].name );
			if ( ef ) {
				Frei0rEffect* effectObj = dynamic_cast<Frei0rEffect*>( appendEffect( ef ) );
				f0r_plugin_info_t* finfo = effectObj->getPluginInfo();
				f0r_param_info_t pinfo;
				for ( int j = 0; j < finfo->num_params; j++ ) {
					effectObj->getParamInfo( &pinfo, j );
					switch ( pinfo.type ) {
						case F0R_PARAM_DOUBLE:
							{
								f0r_param_double dvalue;
								dvalue = edata->effects[i].parameters[j].param_double;
								effectObj->setValue( &dvalue, j );
								break;
							}
						default:
							break;
					}
				}
			}
			
		}
	}*/
}
VideoEffectClip::~VideoEffectClip()
{
/*	effect_stack* node;
	while ( ( node = (effect_stack*)sl_pop( &m_effects ) ) ) {
		delete node->effect;
		delete node;
	}*/
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
	// TODO: Copy pixel aspect and analog_blank
	f->aspect = aspectRatio();
	f->analog_blank = analogBlank();
	f->render_strategy = m_render_strategy;
	return f;

}
IVideoEffect* VideoEffectClip::appendEffect( FilterFactory* factory )
{
	return dynamic_cast<IVideoEffect*>( m_filterClip->appendFilter( factory ) );
/*	FilterBase* fb;
	VideoClip* c1;
	ImageClip* c2;
	TitleClip* c3;
	c1 = dynamic_cast<VideoClip*>(this);
	if ( c1 ) {
		fb = factory->get( c1 );
	} else if ( (c2 = dynamic_cast<ImageClip*>(this)) ) {
		fb = factory->get( c2 );
	} else if ( (c3 = dynamic_cast<TitleClip*>(this)) ) {
		fb = factory->get( c3 );
	}
	assert(fb);
	
	IVideoEffect* e = dynamic_cast<IVideoEffect*>(fb);
	if ( !e ) {
		delete fb;
		return 0;
	}
	effect_stack* n = new effect_stack;
	n->next = 0;
	n->effect = e;
	m_effects = (effect_stack*)sl_unshift( m_effects, n );
	return e;*/
}
void VideoEffectClip::pushEffect( FilterFactory* factory )
{
	m_filterClip->pushFilter( factory );
/*	FilterBase* fb;
	VideoClip* c1;
	ImageClip* c2;
	TitleClip* c3;
	c1 = dynamic_cast<VideoClip*>(this);
	if ( c1 ) {
		fb = factory->get( c1 );
	} else if ( (c2 = dynamic_cast<ImageClip*>(this)) ) {
		fb = factory->get( c2 );
	} else if ( (c3 = dynamic_cast<TitleClip*>(this)) ) {
		fb = factory->get( c3 );
	}
	assert(fb);
	IVideoEffect* e = dynamic_cast<IVideoEffect*>(fb);
	if ( !e ) {
		delete fb;
		return;
	}
	effect_stack* n = new effect_stack;
	n->next = 0;
	n->effect = e;
	m_effects = (effect_stack*)sl_push( m_effects, n );*/
}
/*
static effect_stack* sl_swap( effect_stack* root )
{
	effect_stack* q = root;
	effect_stack* r;
	if ( !q || !q->next ) {
		return q;
	}
	r = q->next;
	q->next = r->next;
	r->next = q;
	return r;
}*/
void VideoEffectClip::moveEffectUp( int num )
{
	m_filterClip->moveFilterUp( num );
/*	if ( !m_effects || num <= 1 ) {
		return;
	}
	if ( num == 2 ) {
		m_effects = sl_swap( m_effects );
		return;
	}
	effect_stack* p = m_effects;
	for ( int i = 3; i < num; i++ ) {
		p = p->next;
	}
	p->next = sl_swap( p->next );*/
}
void VideoEffectClip::moveEffectDown( int num )
{
	m_filterClip->moveFilterDown( num );
/*	if ( !m_effects ) {
		return;
	}
	if ( num == 1 ) {
		m_effects = sl_swap( m_effects );
		return;
	}
	effect_stack* p = m_effects;
	for ( int i = 2; i < num; i++ ) {
		p = p->next;
	}
	p->next = sl_swap( p->next );*/
}
/*static int remove_effect_helper( void*, void* data )
{
	int* num = (int*)data;
	if ( *num ) {
		(*num)--;
		return 0;
	}
	return 1;
}*/
void VideoEffectClip::removeEffect( int num )
{
	m_filterClip->removeFilter( num );
/*	int count = num - 1;
	effect_stack* node = (effect_stack*)sl_remove( &m_effects, remove_effect_helper, &count );
	if ( node ) {
		delete node->effect;
		delete node;
	}*/
}
//void VideoEffectClip::prepareFormat( int ww, int hh, int aspect_w, int aspect_h, float aspect, int analog_blank )
void VideoEffectClip::prepareFormat( int ww, int hh, int , int , float aspect, int analog_blank )
{
	if ( ww == w() && hh == h() ) {
		return;
	}
	if ( m_video_scaler ) {
		gavl_video_scaler_destroy( m_video_scaler );
		m_video_scaler = 0;
	}
	
	gavl_pixelformat_t colorspace = GAVL_RGBA_32;
	m_bits = 4;
	if ( dynamic_cast<VideoClip*>(this) ) {
		if ( m_filterClip->getFilters() ) { //TODO has Video Effects?
			colorspace = GAVL_RGBA_32;
			m_bits = 4;
		} else {
			colorspace = GAVL_RGB_24;
			m_bits = 3;
		}
	} else if ( dynamic_cast<TitleClip*>(this) ) {
		colorspace = GAVL_RGBA_32;
		m_bits = 4;
	} else if ( ImageClip* ic = dynamic_cast<ImageClip*>(this) ) {
		if ( m_filterClip->getFilters() || ic->getFirstFrame()->has_alpha_channel ) {
			colorspace = GAVL_RGBA_32;
			m_bits = 4;
		} else {
			colorspace = GAVL_RGB_24;
			m_bits = 3;
		}
	}
 
	m_video_scaler = gavl_video_scaler_create();
	gavl_video_options_t* options = gavl_video_scaler_get_options( m_video_scaler );

	gavl_video_format_t format_src;
	gavl_video_format_t format_dst;

	format_dst.frame_width  = ww;
	format_dst.frame_height = hh;
	format_dst.image_width  = ww;
	format_dst.image_height = hh;
	format_dst.pixel_width = 1;
	format_dst.pixel_height = 1;
	format_dst.pixelformat = colorspace;
	
	format_src.frame_width  = w();
	format_src.frame_height = h();
	format_src.image_width  = w();
	format_src.image_height = h();
	format_src.pixel_width = 1;
	format_src.pixel_height = 1;
	format_src.pixelformat = colorspace;
	
	if ( m_render_strategy == RENDER_CROP ) {
		gavl_rectangle_f_t src_rect;
		gavl_rectangle_i_t dst_rect;
		crop_format( w(), h(), aspectRatio(), analogBlank(), ww, hh, aspect, analog_blank,
				src_rect.x, src_rect.y, src_rect.w, src_rect.h,
				dst_rect.x, dst_rect.y, dst_rect.w, dst_rect.h );
		gavl_video_options_set_rectangles( options, &src_rect, &dst_rect );
	} else if ( m_render_strategy == RENDER_FIT ) {
		gavl_rectangle_f_t src_rect;
		gavl_rectangle_i_t dst_rect;
		fit_format( w(), h(), aspectRatio(), analogBlank(), ww, hh, aspect, analog_blank,
				src_rect.x, src_rect.y, src_rect.w, src_rect.h,
				dst_rect.x, dst_rect.y, dst_rect.w, dst_rect.h );
		gavl_video_options_set_rectangles( options, &src_rect, &dst_rect );
	} else if ( analogBlank() != analog_blank ) {
		gavl_rectangle_f_t src_rect;
		gavl_rectangle_i_t dst_rect;
		stretch_format( w(), h(), aspectRatio(), analogBlank(), ww, hh, aspect, analog_blank,
				src_rect.x, src_rect.y, src_rect.w, src_rect.h,
				dst_rect.x, dst_rect.y, dst_rect.w, dst_rect.h );
		gavl_video_options_set_rectangles( options, &src_rect, &dst_rect );
	}
	
	if ( gavl_video_scaler_init( m_video_scaler, &format_src, &format_dst ) == -1 ) {
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
	m_frame_dst->strides[0] = ww * m_bits;

}
void VideoEffectClip::unPrepareFormat()
{
	if ( m_video_scaler ) {
		gavl_video_scaler_destroy( m_video_scaler );
		m_video_scaler = 0;
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
	if ( m_video_scaler ) {
		m_frame_src->planes[0] = f->RGB;
		m_frame_dst->planes[0] = tmp_frame->RGB;
		tmp_frame->has_alpha_channel = ( m_bits == 4 );
		tmp_frame->alpha = 1.0;
		gavl_video_scaler_scale( m_video_scaler, m_frame_src, m_frame_dst );
		return tmp_frame;
	} else {
		return f;
	}
}
	
ClipData* VideoEffectClip::vec_getClipData()
{
	return 0;
	/*
	int effect_count = 0;
	filter_stack *node = m_filterClip->getFilters();
	while ( node ) {
		effect_count++;
		node = node->next;
	}
	if ( effect_count == 0 ) {
		return 0;
	}
	EffectClipData* data = new EffectClipData;
	data->effects = new struct effect_data[effect_count];
	data->effect_count = effect_count;
	node = m_filterClip->getFilters();
	int i = 0;
	while ( node ) {
		IVideoEffect* effect = dynamic_cast<IVideoEffect*>(node->filter);
		node = node->next;
		if ( !effect ) {
			i++;
			continue;
		}
		data->effects[i].name = effect->name();
		Frei0rEffect* fe = dynamic_cast<Frei0rEffect*>( effect );
		f0r_plugin_info_t* finfo;
		f0r_param_info_t pinfo;
		finfo = fe->getPluginInfo();
		data->effects[i].parameters = new struct effect_parameter[finfo->num_params];
		for ( int j = 0; j < finfo->num_params; j++ ) {
			fe->getParamInfo( &pinfo, j );
			data->effects[i].parameters[j].name = pinfo.name;
			switch ( pinfo.type ) {
				case F0R_PARAM_DOUBLE:
					{
						f0r_param_double dvalue;
						fe->getValue( &dvalue, j );
						data->effects[i].parameters[j].param_double = (double)dvalue;
						break;
					}
				case F0R_PARAM_BOOL:
					{
						f0r_param_bool bvalue;
						fe->getValue( &bvalue, j );
						data->effects[i].parameters[j].param_bool = bvalue;
						break;
					}
				case F0R_PARAM_COLOR:
					break;
				case F0R_PARAM_POSITION:
					{
						f0r_param_position_t pos;
						fe->getValue( &pos, j );
						data->effects[i].parameters[j].param_position = pos;
						break;
					}
				default:
					break;
			}
		}
		

		
		i++;
	}
	return data;
	*/
}


} /* namespace nle */
