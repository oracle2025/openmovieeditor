/*  AudioVolumeFilter.cxx
 *
 *  Copyright (C) 2007 Richard Spindler <richard.spindler AT gmail.com>
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

#include "AudioVolumeFilter.H"

namespace nle
{

AudioVolumeFilter::AudioVolumeFilter( AudioClipBase* clip )
{
	m_audioClip = audioClip;
	m_automationPoints = 0;
	m_autoCache = 0;
	
	auto_node* r = new auto_node;
	r->next = 0;
	r->y = 1.0;
	r->x = m_audioClip->audioLength();
	m_automationPoints = (auto_node*)sl_push( m_automationPoints, r );
	r = new auto_node;
	r->next = 0;
	r->y = 1.0;
	r->x = 0;
	m_automationPoints = (auto_node*)sl_push( m_automationPoints, r );
}
AudioVolumeFilter::~AudioVolumeFilter()
{
	auto_node* node;
	while ( ( node = (auto_node*)sl_pop( &m_automationPoints ) ) ) {
		delete node;
	}	
}
void AudioVolumeFilter::onDraw( Rect clip_rect )
{
	Track* track;

	float stretchF;
	if ( track->type() == TRACK_TYPE_AUDIO ) {
		stretchF = track->stretchFactor();
	} else {
		stretchF = ( 48000 );
	} 
	fl_push_clip( scr_clip_x, scr_clip_y, scr_clip_w, scr_clip_h );

	auto_node* nodes = m_automationPoints;

	fl_color( FL_RED );
	for ( ; nodes && nodes->next; nodes = nodes->next ) {
		int y = (int)( scr_clip_y + ( ( track->h() - 10 ) * ( 1.0 - nodes->y ) ) + 5 );
		int y_next = (int)( scr_clip_y + ( ( track->h() - 10 ) * ( 1.0 - nodes->next->y ) ) + 5 );
		fl_line( get_screen_position( audioClip->audioPosition() + nodes->x, stretchF ),
				y,
				get_screen_position( audioClip->audioPosition() + nodes->next->x, stretchF ),
				y_next );
	}
	nodes = audioClip->getAutoPoints();
	for ( ; nodes; nodes = nodes->next ) {
		//consider Trimming
		int x;
		int y = (int)( scr_clip_y + ( ( track->h() - 10 ) * ( 1.0 - nodes->y ) ) );
		if ( !nodes->next ) {
			x = get_screen_position( audioClip->audioPosition() + nodes->x, stretchF ) - 10;
		} else if ( nodes == audioClip->getAutoPoints() ) {
			x = get_screen_position( audioClip->audioPosition() + nodes->x, stretchF );
		} else {
			x = get_screen_position( audioClip->audioPosition() + nodes->x, stretchF ) - 5;
		}
		fl_draw_box( FL_UP_BOX, x, y, 10, 10, FL_RED );
	}
	fl_pop_clip();

	
}
DragHandler* AudioVolumeFilter::onMouseDown()
{
	if (  ) {
		return new ShiftAutomationDragHandler( this );
	} else {
		return new AutomationDragHandler( this );
	}
}
int AudioVolumeFilter::fillBuffer( float* input_output, unsigned long frames, int64_t position )
{
	int64_t currentPosition = m_audioClip->audioPosition();
	int64_t aLength = m_audioClip->audioLength();
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
	if ( count < 0 ) { count = 0; }
	count = count * 2;
	for ( int64_t i = (frames * 2) - 1; i >= count; i-- ) {
		output[i] = 0.0;
	}
	return frames;


}
void AudioVolumeFilter::reset()
{
	m_autoCache = m_automationPoints;
}
void AudioVolumeFilter::trimA( int64_t trim )
{
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

}
void AudioVolumeFilter::trimB( int64_t trim )
{
	if ( trim > 0 ) {
		//etwas von den Automations entfernen
		auto_node* n = m_automationPoints;
		while ( n->next && m_audioClip->audioLength() - trim > n->next->x ) {
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
		r->y = ( ( next_y - n->y ) * ( (float)( ( m_audioClip->audioLength() - trim ) - n->x) / (float)(next_x - n->x) ) ) + n->y;
		r->x = m_audioClip->audioLength() - trim;
		r->next = 0;
		n->next = r;
	} else {
		// evtl. etwas hinzufügen
		auto_node* n = m_automationPoints;
		while ( n->next->next ) {
			n = n->next;
		}
		if( n->y == n->next->y ) {
			n->next->x = m_audioClip->audioLength() - trim;
		} else {
			n = n->next;
			auto_node* r = new auto_node;
			r->y = n->y;
			r->x = m_audioClip->audioLength() - trim;
			r->next = 0;
			n->next = r;
		}
	}

}
float AudioVolumeFilter::getEnvelope( int64_t position )
{
	int64_t pPos = position - m_audioClip->audioPosition();
	if ( m_autoCache && m_autoCache->x <= pPos &&  m_autoCache->next && m_autoCache->next->x > pPos ) {
		int64_t diff = m_autoCache->next->x - m_autoCache->x;
		float diff_y = m_autoCache->next->y - m_autoCache->y;
		float inc = diff_y / diff;
		return ( inc * ( pPos - m_autoCache->x ) ) + m_autoCache->y;
	} else {
		if ( m_autoCache ) {
			m_autoCache = m_autoCache->next;
			return getEnvelope( position );
		}
	}
	return 0.0;

}

} /* namespace nle */


#endif /* _AUDIO_VOLUME_FILTER_H_ */

