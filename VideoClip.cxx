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

#include <string>
#include <iostream>

#include "VideoClip.H"
#include "IVideoFile.H"
#include "FilmStrip.H"
#include "AudioFileQT.H"

using namespace std;

namespace nle
{
	
VideoClip::VideoClip( Track *track, int64_t position, IVideoFile *vf )
	: AudioClipBase( track, position )
{
	m_videoFile = vf;
	m_filmStrip = new FilmStrip( vf );
	m_lastFramePosition = -1;
	IAudioFile* af = new AudioFileQT( m_videoFile->filename() );
	if ( af->ok() ) {
		m_audioFile = af;
	} else {
		delete af;
		af = 0;
	}
}
VideoClip::~VideoClip()
{
	delete m_filmStrip;
	delete m_videoFile;
}
string VideoClip::filename()
{
	return m_videoFile->filename();
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
	return m_trimB * (int64_t)( 48000 / g_fps );
}
int64_t VideoClip::audioPosition()
{
	return m_position * (int64_t)( 48000 / g_fps );
}
void VideoClip::reset()
{
	AudioClipBase::reset();
	m_videoFile->seek( m_trimA );//FIXME noch nötig??
	/*evtl. damit nicht bei jedem Abspiel vorgang für jeden Clip geseekt werden muss*/
}
frame_struct* VideoClip::getFrame( int64_t position )
{
//	m_lastFramePosition = position;
	if ( position < m_position || position > m_position + length() )
		return NULL;
	if ( m_lastFramePosition + 1 != position ) {
		int64_t s_pos = position - m_position + m_trimA;
		m_videoFile->seek( s_pos );
	} 
	m_lastFramePosition = position;
	return m_videoFile->read();
}
	
} /* namespace nle */
