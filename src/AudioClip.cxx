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
#include "AudioClipArtist.H"

namespace nle
{

AudioClip::AudioClip( Track *track, int64_t position, IAudioFile* af, int64_t trimA, int64_t trimB )
	: AudioClipBase( track, position, af )
{
	m_trimA = trimA;
	m_trimB = trimB;
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
	r->y = 1.0;
	r->x = 0;
	m_automationPoints = (auto_node*)sl_push( m_automationPoints, r );
	m_artist = new AudioClipArtist( this );
	m_autoCache = 0;
}
AudioClip::~AudioClip()
{
	delete m_artist;
	g_wavArtist->remove( m_audioFile->filename() );
	auto_node* node;
	while ( ( node = (auto_node*)sl_pop( &m_automationPoints ) ) ) {
		delete node;
	}
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
void AudioClip::trimA( int64_t trim )
{
	if ( trim + m_trimA < 0 ) {
		trim = -m_trimA;
	}
	if ( length() - trim <= 0 || trim == 0 ) {
		return;
	}
	float last_y = m_automationPoints->y;
	int64_t last_x = 0;
	if ( trim > 0 ) {
		auto_node* n = m_automationPoints;
		while ( 1 ) {
			if ( n->x < trim ) {
				if ( n != m_automationPoints ) {
					cerr << "Fatal, this shouldn't happen (AudioClip::trimA)" << endl;
					return;
				}
				last_y = n->y;
				last_x = n->x;
				n = n->next;
				delete (auto_node*)sl_pop( &m_automationPoints );
			} else {
				auto_node* r = new auto_node;
				r->y = ( ( n->y - last_y ) * ( (float)(trim - last_x) / (float)(n->x - last_x) ) ) + last_y;
				r->x = 0;
				r->next = 0;
				m_automationPoints = (auto_node*)sl_push( m_automationPoints, r );
				auto_node* q = r->next;
				for ( ; q; q = q->next ) {
					q->x -= trim;
				}
				break;
			}
			if ( !n ) {
				break;
			}
		}
	} else {
		auto_node* n = m_automationPoints;
		if ( n->y == n->next->y ) {
			n->x = 0;
			n = n->next;
			for ( ; n; n = n->next ) {
				n->x -= trim;
			}
		} else {
			auto_node* r = new auto_node;
			r->y = n->y;
			r->x = 0;
			r->next = 0;
			m_automationPoints = (auto_node*)sl_push( m_automationPoints, r );
			for ( ; n; n = n->next ) {
				n->x -= trim;
			}
		}
	}
	Clip::trimA( trim );
}
void AudioClip::trimB( int64_t trim )
{
	if ( trim + m_trimB < 0 ) {
		trim = -m_trimB;
	}
	if ( length() - trim <= 0 ) {
		return;
	}

	if ( trim > 0 ) {
		//etwas von den Automations entfernen
		auto_node* n = m_automationPoints;
		while ( length() - trim > n->next->x ) {
			n = n->next;
		}
		int64_t next_x = n->next->x;
		float next_y = n->next->y;
		auto_node* r = n->next;
		while ( r ) {
			auto_node* q = r;
			r = r->next;
			delete q;
		}
		r = new auto_node;
		r->y = ( ( next_y - n->y ) * ( (float)( ( length() - trim ) - n->x) / (float)(next_x - n->x) ) ) + n->y;
		r->x = length() - trim;
		r->next = 0;
		n->next = r;
	} else {
		// evtl. etwas hinzufügen
		auto_node* n = m_automationPoints;
		while ( n->next->next ) {
			n = n->next;
		}
		if( n->y == n->next->y ) {
			n->next->x = length() - trim;
		} else {
			n = n->next;
			auto_node* r = new auto_node;
			r->y = n->y;
			r->x = length() - trim;
			r->next = 0;
			n->next = r;
		}
	}


	Clip::trimB( trim );
}
void AudioClip::reset()
{
	AudioClipBase::reset();
	m_autoCache = m_automationPoints;
}
float AudioClip::getEnvelope( int64_t position )
{
	/*if ( position < audioPosition() || position > audioPosition() + audioLength() ) {
		cout << "shouldn't happen" << endl;
	}*/
	int64_t pPos = position - audioPosition();
	if ( m_autoCache && m_autoCache->x <= pPos &&  m_autoCache->next && m_autoCache->next->x > pPos ) {
		int64_t diff = m_autoCache->next->x - m_autoCache->x;
		float diff_y = m_autoCache->next->y - m_autoCache->y;
		float inc = diff_y / diff;
		return ( inc * ( pPos - m_autoCache->x ) ) + m_autoCache->y;
	} else {
		if ( m_autoCache ) {
			m_autoCache = m_autoCache->next;
			cout << "Entering Recursion in getEnvelope" << endl;
			return getEnvelope( position );
		}
	}
	return 1.0;
}
int AudioClip::fillBuffer( float* output, unsigned long frames, int64_t position )
{
	int result = AudioClipBase::fillBuffer( output, frames, position );
	int64_t currentPosition = audioPosition();
	int64_t aLength = audioLength();
	//Manipulate output

	int64_t start_output = currentPosition > position ? currentPosition - position : 0;
	int64_t start_clip = currentPosition > position ? currentPosition : position ;
	int64_t count = currentPosition + aLength - position - start_output;
	if ( count + start_output > frames ) {
		count = frames - start_output;
	}
	float envelope;
	for ( int64_t i = 0; i < count; i++ ) {
		envelope = getEnvelope( start_clip + i );
		output[(i*2)] = output[(i*2)] * envelope;
		output[(i*2) + 1] = output[(i*2) + 1] * envelope;
	}
	return result;
}

} /* namespace nle */
