/*  VideoClip.cxx
 *
 *  Copyright (C) 2005 Richard Spindler <richard.spindler AT gmail.com>
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

#include "sl/sl.h"

#include "VideoClip.H"
#include "IVideoFile.H"
#include "FilmStrip.H"
#include "AudioFileFactory.H"
#include "ErrorDialog/IErrorHandler.H"
#include "VideoClipArtist.H"
#include "FilmStripFactory.H"
#include "IAudioFile.H"
#include "Frei0rFactoryPlugin.H"
#include "Frei0rEffect.H"
namespace nle
{
VideoClip::VideoClip( Track* track, int64_t position, IVideoFile* vf, int64_t A, int64_t B, int id )
	: AudioClipBase( track, position, 0, id )
{
	m_trimA = A;
	m_trimB = B;
	m_audioFile = 0;
	m_videoFile = vf;

//	m_effectReader = vf;
	m_effects = 0;
	
	m_filmStrip = g_filmStripFactory->get( vf ); //new FilmStrip( vf );
	
	m_audioFile = AudioFileFactory::get( m_videoFile->filename() );
	CLEAR_ERRORS();
	m_artist = new VideoClipArtist( this );
}
bool VideoClip::hasAudio()
{
	return ( m_audioFile != 0 );
}
VideoClip::~VideoClip()
{
	delete m_artist;
	g_filmStripFactory->remove( m_filmStrip );//delete m_filmStrip;
	effect_stack* node;
	while ( ( node = (effect_stack*)sl_pop( &m_effects ) ) ) {
		delete node->effect;
		delete node;
	}

	delete m_videoFile;
}
string VideoClip::filename()
{
	return m_videoFile->filename();
}
int64_t VideoClip::maxAudioLength()
{
	return m_videoFile->length() * (int64_t)( 48000 / g_fps );
}
int64_t VideoClip::length()
{
	return m_videoFile->length() - ( m_trimA + m_trimB );
}
int64_t VideoClip::audioTrimA()
{
	return m_trimA * (int64_t)( 48000 / g_fps );
}
int64_t VideoClip::audioTrimB()
{
	int64_t r = m_trimB * (int64_t)( 48000 / g_fps );
	int64_t t = m_audioFile->length() - maxAudioLength() + r;
	return t < 0 ? 0 : t;
}
int64_t VideoClip::audioPosition()
{
	return m_position * (int64_t)( 48000 / g_fps );
}
void VideoClip::reset()
{
	AudioClipBase::reset();
}
frame_struct* VideoClip::getFrame( int64_t position )
{
	if ( position < m_position || position > m_position + length() )
		return NULL;
	int64_t s_pos = position - m_position + m_trimA;
	
	frame_struct *f = m_videoFile->getFrame( s_pos );
	effect_stack *node = m_effects;
	while ( node ) {
		f = node->effect->getFrame( f, s_pos );
		node = node->next;
	}
	return f;
}
int64_t VideoClip::fileLength()
{
	return m_videoFile->length();
}
IVideoEffect* VideoClip::appendEffect( AbstractEffectFactory* factory )
{
	IVideoEffect* e = factory->get( m_videoFile->width(), m_videoFile->height() );
	effect_stack* n = new effect_stack;
	n->next = 0;
	n->effect = e;
	m_effects = (effect_stack*)sl_unshift( m_effects, n );
	return e;
}
void VideoClip::pushEffect( AbstractEffectFactory* factory )
{
	IVideoEffect* e = factory->get( m_videoFile->width(), m_videoFile->height() );
	effect_stack* n = new effect_stack;
	n->next = 0;
	n->effect = e;
	m_effects = (effect_stack*)sl_push( m_effects, n );
}
void VideoClip::popEffect()
{
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

void VideoClip::moveEffectUp( int num )
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
void VideoClip::moveEffectDown( int num )
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
void VideoClip::removeEffect( int num )
{
	int count = num - 1;
	effect_stack* node = (effect_stack*)sl_remove( &m_effects, remove_effect_helper, &count );
	if ( node ) {
		delete node->effect;
		delete node;
	}
}

	
} /* namespace nle */
