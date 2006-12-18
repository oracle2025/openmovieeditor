/*  AudioClipArtist.cxx
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

#include <FL/fl_draw.H>
#include <FL/filename.H>

#include "AudioClipArtist.H"
#include "AudioClip.H"
#include "WavArtist.H"
#include "TimelineView.H"
#include "timeline/Track.H"

namespace nle
{

AudioClipArtist::AudioClipArtist( AudioClip* clip )
	: m_clip( clip )
{
}
AudioClipArtist::~AudioClipArtist()
{
}
void AudioClipArtist::render( Rect& rect, int64_t start, int64_t stop )
{
	int64_t start_, stop_;
	if ( m_clip->position() < start ) {
		start_ = m_clip->trimA() + ( start - m_clip->position() );
	} else {
		start_ = m_clip->trimA();
	}
	if ( m_clip->position() + m_clip->length() > stop ) {
		stop_ = m_clip->trimA() + ( stop - m_clip->position() );
	} else {
		stop_ = m_clip->trimA() + m_clip->length();
	}
	g_wavArtist->render( m_clip->filename(), rect, start_, stop_ );
	int _x = g_timelineView->get_screen_position( m_clip->position(), m_clip->track()->stretchFactor() );
	fl_color( FL_DARK3 );
	fl_font( FL_HELVETICA, 11 );
	fl_draw( fl_filename_name( m_clip->filename().c_str() ), _x + 6, rect.y + rect.h - 4 );
	fl_color( FL_WHITE );
	fl_draw( fl_filename_name( m_clip->filename().c_str() ), _x + 5, rect.y + rect.h - 5 );
}

	
} /* namespace nle */
