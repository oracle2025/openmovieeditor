/*  AudioClip.cxx
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

#include <sl.h>

#include "AudioClip.H"
#include "IAudioFile.H"
#include "WavArtist.H"

namespace nle
{

AudioClip::AudioClip( Track *track, int64_t position, IAudioFile* af )
	: AudioClipBase( track, position, af )
{
	g_wavArtist->add( af );
	m_basicLevel = 1.0;
	m_automationPoints = 0;
	//Add Start and End Node
	auto_node* r = new auto_node;
	r->next = 0;
	r->y = 1.0;
	r->x = af->length();
	m_automationPoints = (auto_node*)sl_push( m_automationPoints, r );
	r = new auto_node;
	r->next = 0;
	r->y = 0.5;
	r->x = 100000;
	m_automationPoints = (auto_node*)sl_push( m_automationPoints, r );
	r = new auto_node;
	r->next = 0;
	r->y = 1.0;
	r->x = 0;
	m_automationPoints = (auto_node*)sl_push( m_automationPoints, r );
}
AudioClip::~AudioClip()
{
	g_wavArtist->remove( m_audioFile->filename() );
}
int64_t AudioClip::length()
{
	return audioLength();
}
string AudioClip::filename()
{
	return audioFilename();
}
int64_t AudioClip::audioTrimA()
{
	return m_trimA;
}
int64_t AudioClip::audioTrimB()
{
	return m_trimB;
}
int64_t AudioClip::audioPosition()
{
	return position();
}

} /* namespace nle */
