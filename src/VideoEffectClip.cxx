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

#include <gavl/gavl.h>
#include "sl/sl.h"
#include "global_includes.H"

#include "VideoEffectClip.H"
#include "AbstractEffectFactory.H"
#include "IVideoEffect.H"
#include "VideoClip.H"
#include "ImageClip.H"
#include "TitleClip.H"

namespace nle
{

VideoEffectClip::~VideoEffectClip()
{
	effect_stack* node;
	while ( ( node = (effect_stack*)sl_pop( &m_effects ) ) ) {
		delete node->effect;
		delete node;
	}
	unPrepareFormat();
}
frame_struct* VideoEffectClip::getFrame( int64_t position )
{
	int64_t position_in_file = 0;
	frame_struct *f = getRawFrame( position, position_in_file );
	if ( !f ) {
		return 0;
	}
	effect_stack *node = m_effects;
	while ( node ) {
		f = node->effect->getFrame( f, position_in_file );
		node = node->next;
	}
	// TODO: Copy pixel aspect and analog_blank
	f->render_strategy = m_render_strategy;
	return f;

}
IVideoEffect* VideoEffectClip::appendEffect( AbstractEffectFactory* factory )
{
	IVideoEffect* e = factory->get( w(), h() );
	effect_stack* n = new effect_stack;
	n->next = 0;
	n->effect = e;
	m_effects = (effect_stack*)sl_unshift( m_effects, n );
	return e;
}
void VideoEffectClip::pushEffect( AbstractEffectFactory* factory )
{
	IVideoEffect* e = factory->get( w(), h() );
	effect_stack* n = new effect_stack;
	n->next = 0;
	n->effect = e;
	m_effects = (effect_stack*)sl_push( m_effects, n );
}
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
}
void VideoEffectClip::moveEffectUp( int num )
{
	if ( !m_effects || num <= 1 ) {
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
	p->next = sl_swap( p->next );
}
void VideoEffectClip::moveEffectDown( int num )
{
	if ( !m_effects ) {
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
	p->next = sl_swap( p->next );
}
static int remove_effect_helper( void*, void* data )
{
	int* num = (int*)data;
	if ( *num ) {
		(*num)--;
		return 0;
	}
	return 1;
}
void VideoEffectClip::removeEffect( int num )
{
	int count = num - 1;
	effect_stack* node = (effect_stack*)sl_remove( &m_effects, remove_effect_helper, &count );
	if ( node ) {
		delete node->effect;
		delete node;
	}
}
void VideoEffectClip::prepareFormat( int ww, int hh )
{
	if ( ww == w() && hh == h() && ( m_render_strategy == RENDER_FIT || m_render_strategy == RENDER_DEFAULT ) ) {
		return;
	}
	if ( m_video_scaler ) {
		gavl_video_scaler_destroy( m_video_scaler );
		m_video_scaler = 0;
	}
	
	gavl_pixelformat_t colorspace = GAVL_RGBA_32;
	m_bits = 4;
	if ( dynamic_cast<VideoClip*>(this) ) {
		if ( m_effects ) {
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
		if ( m_effects || ic->getFirstFrame()->has_alpha_channel ) {
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
		float src_aspect = ( (float)w() / (float)h() );
		float dst_aspect = ( (float)ww / (float)hh );
		if ( src_aspect < dst_aspect ) { // Skyscraper
			src_rect.x = 0.0;
			src_rect.w = w();
			src_rect.h = h() / dst_aspect;
			src_rect.y = ( h() - src_rect.h ) / 2 ;
		} else { // Banner
			src_rect.y = 0.0;
			src_rect.w = w() / dst_aspect;
			src_rect.x = ( w() - src_rect.w ) / 2;
			src_rect.h = h();
		}
		dst_rect.x = 0;
		dst_rect.y = 0;
		dst_rect.w = ww;
		dst_rect.h = hh;
		gavl_video_options_set_rectangles( options, &src_rect, &dst_rect );
	} else if ( m_render_strategy == RENDER_FIT ) {
		gavl_rectangle_f_t src_rect;
		gavl_rectangle_i_t dst_rect;
		float src_aspect = ( (float)w() / (float)h() );
		float dst_aspect = ( (float)ww / (float)hh );
		if ( src_aspect > dst_aspect ) { // Banner
			src_rect.x = 0.0;
			src_rect.w = w();
			src_rect.h = h() * dst_aspect;
			src_rect.y = ( h() - src_rect.h ) / 2 ;
		} else { // Skyscraper
			src_rect.y = 0.0;
			src_rect.w = w() * dst_aspect;
			src_rect.x = ( w() - src_rect.w ) / 2;
			src_rect.h = h();
		}
		dst_rect.x = 0;
		dst_rect.y = 0;
		dst_rect.w = ww;
		dst_rect.h = hh;
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
	


} /* namespace nle */
