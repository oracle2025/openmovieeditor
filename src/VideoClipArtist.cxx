/*  VideoClipArtist.cxx
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

#include <cassert>

#include <FL/fl_draw.H>

#include "VideoClipArtist.H"
#include "VideoClip.H"
#include "FilmStrip.H"
#include "TimelineView.H"
#include "globals.H"
#include "timeline/Track.H"
#include "mute.xpm"
#include "SwitchBoard.H"

namespace nle
{

VideoClipArtist::VideoClipArtist( VideoClip* clip )
	: m_clip( clip )
{
}
VideoClipArtist::~VideoClipArtist()
{
}
void VideoClipArtist::render( Rect& rect, int64_t, int64_t )
{
	fl_push_clip( rect.x, rect.y, rect.w, rect.h );		
	int _x;
	FilmStrip* fs = m_clip->getFilmStrip();
	int s = m_clip->trimA() / 100;
	int e = ( m_clip->length() / 100 ) + s;
	int off = m_clip->trimA() % 100;
	int64_t inc = 1 + (int64_t)( 0.5 / SwitchBoard::i()->zoom() );
	assert( inc >= 1 );
	for ( int k = s; k < e + 2; k += inc ) {
		// TODO: g_timeline should not be used, rect, start and stop are
		// sufficient

		_x = g_timelineView->get_screen_position( m_clip->position() + (k - s) * 100, m_clip->track()->stretchFactor()  ) - off;
		
		pic_struct* f = fs->get_pic(k);
		if ( f ) {
			fl_draw_image( f->data, _x, rect.y, f->w, f->h );
		}
	}
	if ( m_clip->m_mute ) {
		fl_draw_pixmap( mute_xpm, rect.x + 20, rect.y + 5 );
	}
	fl_pop_clip();
}

} /* namespace nle */
