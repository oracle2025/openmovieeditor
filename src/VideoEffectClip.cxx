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

#include "sl/sl.h"
#include "global_includes.H"

#include "VideoEffectClip.H"
#include "AbstractEffectFactory.H"
#include "IVideoEffect.H"

namespace nle
{

VideoEffectClip::~VideoEffectClip()
{
	effect_stack* node;
	while ( ( node = (effect_stack*)sl_pop( &m_effects ) ) ) {
		delete node->effect;
		delete node;
	}
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


} /* namespace nle */
