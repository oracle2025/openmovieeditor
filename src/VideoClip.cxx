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

namespace nle
{
VideoClip::VideoClip( Track* track, int64_t position, IVideoFile* vf, int64_t A, int64_t B, int id )
	: AudioClipBase( track, position, 0, id ), VideoEffectClip()
{
	m_trimA = A;
	m_trimB = B;
	m_audioFile = 0;
	m_videoFile = vf;

	
	m_filmStrip = g_filmStripFactory->get( vf ); //new FilmStrip( vf );
	
	m_audioFile = AudioFileFactory::get( m_videoFile->filename() );
	CLEAR_ERRORS();
	m_artist = new VideoClipArtist( this );
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
	delete m_artist;
	g_filmStripFactory->remove( m_filmStrip );//delete m_filmStrip;
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
frame_struct* VideoClip::getRawFrame( int64_t position, int64_t &position_in_file )
{
	if ( position < m_position || position > m_position + length() )
		return NULL;
	int64_t s_pos = position - m_position + m_trimA;
	position_in_file = s_pos;
	frame_struct *f = m_videoFile->getFrame( s_pos );
	return f;
}
int64_t VideoClip::fileLength()
{
	return m_videoFile->length();
}	
} /* namespace nle */
