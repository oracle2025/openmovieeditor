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

#include "VideoClip.H"
#include "IVideoFile.H"
#include "FilmStrip.H"
#include "AudioFileFactory.H"
#include "ErrorDialog/IErrorHandler.H"
#include "VideoClipArtist.H"
#include "FilmStripFactory.H"
#include "IAudioFile.H"
#include "helper.H"
#include "WavArtist.H"
#include "EnvelopeClip.H"

namespace nle
{
VideoClip::VideoClip( Track* track, int64_t position, IVideoFile* vf, int64_t A, int64_t B, int id, ClipData* data )
	: AudioClipBase( track, position, 0, id ), VideoEffectClip()
{
	m_trimA = A;
	m_trimB = B;
	m_audioFile = 0;
	m_envelopeClip = 0;
	m_videoFile = vf;
	m_frame = 0;
	m_lastFrame = -1;

	m_filmStrip = g_filmStripFactory->get( vf ); //new FilmStrip( vf );
	
	m_audioFile = AudioFileFactory::get( m_videoFile->filename() );
	CLEAR_ERRORS();
	m_artist = new VideoClipArtist( this );


	guess_aspect( w(), h(), &m_aspectHeight, &m_aspectWidth, &m_aspectRatio, &m_analogBlank, 0, 0 );
	setEffects( data );
	if ( m_audioFile ) {
		g_wavArtist->add( m_audioFile );
		m_envelopeClip = new EnvelopeClip( this );
	}
}
int VideoClip::w()
{
	return m_videoFile->width();
}
int VideoClip::h()
{
	return m_videoFile->height();
}
bool VideoClip::hasAudio()
{
	return ( m_audioFile != 0 );
}
VideoClip::~VideoClip()
{
	if ( m_audioFile ) {
		g_wavArtist->remove( m_audioFile->filename() );
	}
	delete m_artist;
	g_filmStripFactory->remove( m_filmStrip );//delete m_filmStrip;
	delete m_videoFile;
	if ( m_envelopeClip ) {
		delete m_envelopeClip;
	}
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
	m_envelopeClip->reset();
	m_lastFrame = -1;
}
frame_struct* VideoClip::getRawFrame( int64_t position, int64_t &position_in_file )
{
	if ( position < m_position || position > m_position + length() )
		return NULL;
	int64_t s_pos = position - m_position + m_trimA;
	position_in_file = s_pos;
	if ( !m_freezeFrame ) {
		m_frame = m_videoFile->getFrame( s_pos );
	} else {
		if ( m_lastFrame == m_trimA ) {
			return m_frame;
		} else {
			m_lastFrame = m_trimA;
			m_frame = m_videoFile->getFrame( m_trimA );
		}
	}
	return m_frame;
}
int64_t VideoClip::fileLength()
{
	return m_videoFile->length();
}	
auto_node* VideoClip::getAutoPoints()
{
	if ( !m_envelopeClip ) {
		return 0;
	}
	return m_envelopeClip->getAutoPoints();
}
void VideoClip::setAutoPoints( auto_node* a )
{
	if ( !m_envelopeClip ) {
		return;
	}
	m_envelopeClip->setAutoPoints( a );
}

bool VideoClip::has_automation()
{
	return (bool)m_envelopeClip;
}
void VideoClip::trimA( int64_t trim )
{
	if ( trim + m_trimA < 0 ) {
		trim = -m_trimA;
	}
	if ( length() - trim <= 0 || trim == 0 ) {
		return;
	}
	m_envelopeClip->trimA( trim * (int64_t)( 48000 / g_fps ) );
	Clip::trimA( trim );
}
void VideoClip::trimB( int64_t trim )
{
	if ( trim + m_trimB < 0 ) {
		trim = -m_trimB;
	}
	if ( length() - trim <= 0 ) {
		return;
	}
	m_envelopeClip->trimB( trim * (int64_t)( 48000 / g_fps ) );
	Clip::trimB( trim );
}
int VideoClip::fillBuffer( float* output, unsigned long frames, int64_t position )
{
	return m_envelopeClip->fillBuffer( output, frames, position );
}

} /* namespace nle */
