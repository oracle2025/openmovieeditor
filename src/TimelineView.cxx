/*  TimelineView.cxx
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

#include <math.h>

#include <FL/Fl.H>
#include <FL/Fl_Widget.H>
#include <FL/Fl_Window.H>
#include <FL/fl_draw.H>

#include "nle.h"
#include "TimelineView.H"
#include "Timeline.H"
#include "SwitchBoard.H"
#include "VideoClip.H"
#include "VideoTrack.H"
#include "MoveDragHandler.H"
#include "TrimDragHandler.H"
#include "Rect.H"
#include "events.H"
#include "FilmStrip.H"
#include "helper.H"
#include "ErrorDialog/IErrorHandler.H"
#include "AudioClip.H"
#include "AutomationDragHandler.H"
#include "IClipArtist.H"

#include "audio.xpm"
#include "video.xpm"
#include "titles.xpm"

namespace nle
{

bool USING_AUDIO = 0;
	
TimelineView* g_timelineView = 0;

TimelineView::TimelineView( int x, int y, int w, int h, const char *label )
	: Fl_Widget( x, y, w, h, label )
{
	g_timelineView = this;
	m_dragHandler = NULL;
	

	m_scrollPosition = 0;
	m_stylusPosition = 0;
	SwitchBoard::i()->timelineView(this);

	
}
TimelineView::~TimelineView()
{
	cout << "Never Called" << endl;
}
int TimelineView::handle( int event )
{
	int _x = Fl::event_x();
	int _y = Fl::event_y() - y();
	static bool automationLine = false;
	switch ( event ) {
		case FL_PASTE:
			{
				Track* t = get_track( _x, _y );
				int64_t rp = get_real_position( _x, t->stretchFactor() );
				if (t && !fl_filename_isdir(Fl::event_text()) ) {
					
					t->addFile( rp, Fl::event_text() );
					adjustScrollbar();
					redraw();
				}
			}
			return 1;
		case FL_DND_DRAG:
		case FL_DND_RELEASE:
		case FL_DND_ENTER:
		case FL_DND_LEAVE:
			return 1;
		case FL_PUSH: {
				Clip* cl = get_clip( _x, _y );
				if ( cl && g_ui->automationsMode() && cl->has_automation() ) {
					m_dragHandler = new AutomationDragHandler( cl, get_clip_rect( cl, true ), 0, _x, _y );
					return 1;
				} else if ( cl ) {
					if ( _x < get_screen_position( cl->position(), cl->track()->stretchFactor() ) + 8 ) {
						m_dragHandler = new TrimDragHandler(
								this, cl, cl->track()->num(),
								0, 0, false );
					} else if ( _x > get_screen_position( cl->position() + cl->length(), cl->track()->stretchFactor() ) - 8) {
						m_dragHandler = new TrimDragHandler(
								this, cl, cl->track()->num(),
								0, 0, true );
					} else {
						m_dragHandler = new MoveDragHandler(
								this, cl, _x, _y, get_clip_rect( cl, false )
								);
					}
					return 1;
				}
			}// vv-- Fall Through
			return Fl_Widget::handle( event );	
		case FL_DRAG:
			if ( m_dragHandler ) {
				m_dragHandler->OnDrag( _x, _y );
				return 1;
			}
		case FL_RELEASE:
			if ( m_dragHandler ) {
				m_dragHandler->OnDrop( _x, _y );
				delete m_dragHandler;
				m_dragHandler = NULL;
				redraw();
				return 1;
			}
		default:
			return Fl_Widget::handle( event );	
	}
	
}
void TimelineView::resize(int x, int y, int w, int h)
{
	Fl_Widget::resize( x, y, w, h );
	long num = long( this->w() / SwitchBoard::i()->zoom() );
	e_scroll_position( m_scrollPosition, num, 1024 );

}
void TimelineView::draw()
{
	fl_overlay_clear();
	fl_push_clip( x(), y(), w(), h() );


//     - Draw Background
	fl_draw_box( FL_FLAT_BOX, x(), y(), w(), h(), FL_BACKGROUND_COLOR );
// END - Draw Background

	int track_count = -1;
	for ( track_node* i = g_timeline->getTracks(); i; i = i->next ) {
		Track* track = i->track;
		track_count++;
		int y_coord = y() + TRACK_SPACING + track_count * TRACK_OFFSET;
		int x_coord = x() + LEFT_TRACK_SPACING;
		int w_size = w() - TRACK_SPACING - LEFT_TRACK_SPACING;
		
		USING_AUDIO = track->type() == TRACK_TYPE_AUDIO;
		
	//     - Draw Button
		fl_draw_box( FL_UP_BOX, x() + TRACK_SPACING, y_coord, 64, TRACK_HEIGHT + 1, FL_BACKGROUND_COLOR );
		fl_color( FL_BLACK );
		fl_font( FL_HELVETICA, 11 );
		if ( track_count == 0 ) {
			fl_draw( "Titles", x() + TRACK_SPACING + 23, y_coord + 18);
			fl_draw_pixmap( titles_xpm, x() + TRACK_SPACING + 3, y_coord + 5 );
		} else if ( USING_AUDIO ) {
			fl_draw( "Audio", x() + TRACK_SPACING + 23, y_coord + 18);
			fl_draw_pixmap( audio_xpm, x() + TRACK_SPACING + 3, y_coord + 5 );
		} else {
			fl_draw( "Video", x() + TRACK_SPACING + 23, y_coord + 18);
			fl_draw_pixmap( video_xpm, x() + TRACK_SPACING + 6, y_coord + 8 );
		}
	// END - Draw Button

	//     - Draw Track Background
		fl_draw_box( FL_BORDER_BOX , x_coord, y_coord, w_size, TRACK_HEIGHT, FL_DARK2 );
	// END - Draw Track Background

		fl_push_clip( x_coord, y_coord, w_size, TRACK_HEIGHT );
		
		for ( clip_node* j = track->getClips(); j; j = j->next ) {
			Clip* clip = j->clip;
			int scr_clip_x = get_screen_position( clip->position(), track->stretchFactor() );
			int scr_clip_y = y_coord;
			int scr_clip_w = (int)( clip->length() * SwitchBoard::i()->zoom() / track->stretchFactor() );
			int scr_clip_h = TRACK_HEIGHT;
			
			fl_draw_box( FL_BORDER_BOX , scr_clip_x, scr_clip_y, scr_clip_w, scr_clip_h, FL_DARK3 );
			
			Rect r( scr_clip_x, scr_clip_y, scr_clip_w, scr_clip_h );
			IClipArtist* artist = clip->getArtist();
			if ( artist ) {
				artist->render( r, 0, 0 );
			}

			fl_draw_box( FL_BORDER_FRAME, scr_clip_x, scr_clip_y, scr_clip_w, scr_clip_h, FL_DARK3 );
		//     - Draw Trim Triangles
		    /* 	fl_color( FL_BLACK );
			fl_begin_polygon();
			fl_vertex( scr_clip_x, scr_clip_y + TRACK_HEIGHT / 2 );
			fl_vertex( scr_clip_x + 8, scr_clip_y + TRACK_HEIGHT / 2 - 5 );
			fl_vertex( scr_clip_x + 8, scr_clip_y + TRACK_HEIGHT / 2 + 5 );
			fl_end_polygon();
			fl_begin_polygon();
			fl_vertex( scr_clip_x + scr_clip_w, scr_clip_y + TRACK_HEIGHT / 2 );
			fl_vertex( scr_clip_x + scr_clip_w - 8, scr_clip_y + TRACK_HEIGHT / 2 - 5 );
			fl_vertex( scr_clip_x + scr_clip_w - 8, scr_clip_y + TRACK_HEIGHT / 2 + 5 );
			fl_end_polygon();*/
		// END - Draw Trim Triangles
			if ( track->type() == TRACK_TYPE_AUDIO ) {
				//Draw Automations
				fl_push_clip( scr_clip_x, scr_clip_y, scr_clip_w, scr_clip_h );
				AudioClip* audioClip = dynamic_cast<AudioClip*>(clip);
				
				auto_node* nodes = audioClip->getAutoPoints();

				fl_color( FL_RED );
				for ( ; nodes && nodes->next; nodes = nodes->next ) {
					int y = (int)( scr_clip_y + ( ( TRACK_HEIGHT - 10 ) * ( 1.0 - nodes->y ) ) + 5 );
					int y_next = (int)( scr_clip_y + ( ( TRACK_HEIGHT - 10 ) * ( 1.0 - nodes->next->y ) ) + 5 );
					fl_line( get_screen_position( audioClip->position() + nodes->x, track->stretchFactor() ),
							y,
							get_screen_position( audioClip->position() + nodes->next->x, track->stretchFactor() ),
							y_next );
				}
				nodes = audioClip->getAutoPoints();
				for ( ; nodes; nodes = nodes->next ) {
					//consider Trimming
					int x;
					int y = (int)( scr_clip_y + ( ( TRACK_HEIGHT - 10 ) * ( 1.0 - nodes->y ) ) );
					if ( !nodes->next ) {
						x = get_screen_position( audioClip->position() + nodes->x, track->stretchFactor() ) - 10;
					} else if ( nodes == audioClip->getAutoPoints() ) {
						x = get_screen_position( audioClip->position() + nodes->x, track->stretchFactor() );
					} else {
						x = get_screen_position( audioClip->position() + nodes->x, track->stretchFactor() ) - 5;
					}
					fl_draw_box( FL_UP_BOX, x, y, 10, 10, FL_RED );
				}
				fl_pop_clip();
			}

		}
		for ( clip_node* j = track->getClips(); j; j = j->next ) {
			Clip* clip = j->clip;
			for ( clip_node* k = j->next; k; k = k->next ) {
				Clip* cclip = k->clip;
				if ( clip->A() < cclip->A() && clip->B() > cclip->A() ) {
					int x = get_screen_position( cclip->A(), track->stretchFactor() );
					int w = get_screen_position( clip->B(), track->stretchFactor() ) - x;
					fl_draw_box( FL_FLAT_BOX, x, y_coord, w, TRACK_HEIGHT, FL_DARK_BLUE );
					fl_draw_box( FL_BORDER_FRAME, x, y_coord, w, TRACK_HEIGHT, FL_RED );
					fl_color( FL_RED );
					fl_line( x, y_coord, x + w, y_coord + TRACK_HEIGHT );
					fl_line( x + w, y_coord, x, y_coord + TRACK_HEIGHT );
				} else if ( cclip->A() < clip->A() && cclip->B() > clip->A() ) {
					int x = get_screen_position( clip->A(), track->stretchFactor() );
					int w = get_screen_position( cclip->B(), track->stretchFactor() ) - x;
					fl_draw_box( FL_FLAT_BOX, x, y_coord, w, TRACK_HEIGHT, FL_DARK_BLUE );
					fl_draw_box( FL_BORDER_FRAME, x, y_coord, w, TRACK_HEIGHT, FL_RED );
					fl_color( FL_RED );
					fl_line( x, y_coord, x + w, y_coord + TRACK_HEIGHT );
					fl_line( x + w, y_coord, x, y_coord + TRACK_HEIGHT );
				}

			}
		}
		for ( clip_node* j = track->getClips(); j; j = j->next ) {
			Clip* clip = j->clip;
			int scr_clip_x = get_screen_position( clip->position(), track->stretchFactor() );
			int scr_clip_y = y_coord;
			int scr_clip_w = (int)( clip->length() * SwitchBoard::i()->zoom() / track->stretchFactor() );
			int scr_clip_h = TRACK_HEIGHT;
		     	fl_color( FL_BLACK );
			fl_begin_polygon();
			fl_vertex( scr_clip_x, scr_clip_y + TRACK_HEIGHT / 2 );
			fl_vertex( scr_clip_x + 8, scr_clip_y + TRACK_HEIGHT / 2 - 5 );
			fl_vertex( scr_clip_x + 8, scr_clip_y + TRACK_HEIGHT / 2 + 5 );
			fl_end_polygon();
			fl_begin_polygon();
			fl_vertex( scr_clip_x + scr_clip_w, scr_clip_y + TRACK_HEIGHT / 2 );
			fl_vertex( scr_clip_x + scr_clip_w - 8, scr_clip_y + TRACK_HEIGHT / 2 - 5 );
			fl_vertex( scr_clip_x + scr_clip_w - 8, scr_clip_y + TRACK_HEIGHT / 2 + 5 );
			fl_end_polygon();
		}

		fl_pop_clip();
	}


//Überlappungen malen

	fl_pop_clip();
	fl_overlay_rect( get_screen_position( m_stylusPosition ), y(), 1, h() );
}
void TimelineView::add_video( int track, int y, string filename )
{
	//m_timeline->add_video( track, get_real_position( y ), filename );
}
int64_t TimelineView::get_real_position( int p, float stretchFactor )
{
	return int64_t( ( ( float(p - LEFT_TRACK_SPACING - x() ) / SwitchBoard::i()->zoom() ) + m_scrollPosition ) * stretchFactor );
}
int TimelineView::get_screen_position( int64_t p, float stretchFactor )
{
	return int ( float( p - ( m_scrollPosition * stretchFactor ) ) * SwitchBoard::i()->zoom() / stretchFactor ) + LEFT_TRACK_SPACING + x();

}
void TimelineView::scroll( int64_t position )
{
	m_scrollPosition = position;
	//redraw();
}
void TimelineView::zoom( float zoom )
{
	if ( isinf(zoom) || isnan(zoom) || zoom >= 10.0 ) {
		redraw();
		e_stylus_position( get_screen_position(m_stylusPosition) );
		return;
	}
	SwitchBoard::i()->zoom( zoom );
	redraw();
	e_stylus_position( get_screen_position(m_stylusPosition) );
}
Track* TimelineView::get_track( int _x, int _y )
{
	for ( track_node* o = g_timeline->getTracks(); o; o = o->next ) {
		if ( !get_track_rect( o->track ).inside( _x, _y ) )
			continue;
		return o->track;
	}
	return NULL;
}
Clip* TimelineView::get_clip( int _x, int _y )
{
	Track *tr = get_track( _x, _y );
	if (!tr)
		return NULL;
	for ( clip_node* p = tr->getClips(); p; p = p->next ) {
		Rect tmp = get_clip_rect( p->clip, true );
		if ( !tmp.inside( _x, _y ) )
			continue;
		return p->clip;
	}
	return NULL;
}
Rect TimelineView::get_track_rect( Track* track )
{
	Rect tmp(
			TRACK_SPACING,
			TRACK_SPACING + (TRACK_SPACING + TRACK_HEIGHT) * track->num(),
			w() - 2 * TRACK_SPACING,
			TRACK_HEIGHT
		);
	return tmp;
}
Rect TimelineView::get_clip_rect( Clip* clip, bool clipping )
{
	
	Rect tmp(
			get_screen_position( clip->position(), clip->track()->stretchFactor() ),
			int( TRACK_SPACING + (TRACK_SPACING + TRACK_HEIGHT) * clip->track()->num() ),
			int( clip->length() * SwitchBoard::i()->zoom() / clip->track()->stretchFactor() ),
			TRACK_HEIGHT
		);
	if ( clipping ) {
		if ( tmp.x < LEFT_TRACK_SPACING + x() ) {
			tmp.w += tmp.x - ( LEFT_TRACK_SPACING + x() );
			tmp.x = LEFT_TRACK_SPACING + x();
		}
		if ( tmp.w + tmp.x > w() - TRACK_SPACING + x() ) {
			tmp.w = ( w() - TRACK_SPACING + x() ) - tmp.x;
		}
	}
	return tmp;
}

bool inside_widget( Fl_Widget* widget, int x, int y )
{
	int w_x = widget->x();
	int w_y = widget->y();
	int w_w = widget->w();
	int w_h = widget->h();
	return ( x >= w_x && x <= w_x + w_w && y >= w_y && y <= w_y + w_h );
}

void TimelineView::move_clip( Clip* clip, int _x, int _y, int offset )
{
	Track *new_tr = get_track( _x, _y );
	Track *old_tr = clip->track();
	if ( inside_widget( g_trashCan, _x, y() + _y ) ) {
		old_tr->removeClip( clip );
		delete clip;
		return;
	}
	if (!new_tr || new_tr->type() != old_tr->type() ) {
		return;
	}
	int64_t new_position = get_real_position( _x - offset, clip->track()->stretchFactor() );
	new_position = new_tr->getSnapA( clip, new_position );
	new_position = new_tr->getSnapB( clip, new_position );
	if ( new_position < 0 ) {
		new_position = 0;
	}
	clip->position( new_position );
	adjustScrollbar();
	if ( new_tr == old_tr ) {
		VideoTrack* t = dynamic_cast<VideoTrack*>(new_tr);
		if ( t ) {
			t->reconsiderFadeOver();
		}
		return;
	}
	old_tr->removeClip( clip );
	clip->track( new_tr );
	new_tr->addClip( clip );
	VideoTrack* t = dynamic_cast<VideoTrack*>(new_tr);
	if ( t ) {
		t->reconsiderFadeOver();
	}
	t = dynamic_cast<VideoTrack*>(old_tr);
	if ( t ) {
		t->reconsiderFadeOver();
	}

}
void TimelineView::adjustScrollbar()
{
	int64_t l = g_timeline->length();
	if ( l > g_scrollBar->slider_size_i() ) {
		g_scrollBar->slider_size_i( l );
	}
}
void TimelineView::trim_clip( Clip* clip, int _x, bool trimRight )
{
	if (trimRight) {
		int64_t trimv = int64_t( ( clip->position() + clip->length() ) - ( get_real_position(_x, clip->track()->stretchFactor()) ) );
		clip->trimB( trimv );
		return;
	}
	clip->trimA( int64_t( get_real_position(_x, clip->track()->stretchFactor())  - clip->position() ) );
}
void TimelineView::move_cursor( int64_t position )
{
	m_stylusPosition = position;
	if ( m_stylusPosition < 0 ) {
		m_stylusPosition = 0;
	} else if ( m_stylusPosition > 1024 ) {
		m_stylusPosition = 1024;
	}
	window()->make_current();
	long screen_pos = get_screen_position(m_stylusPosition);
	if ( screen_pos < LEFT_TRACK_SPACING ) {
		m_scrollPosition = get_real_position( screen_pos );
		e_scroll_position( m_scrollPosition, long( w() / SwitchBoard::i()->zoom() ), 1024 );
		redraw();
	} else if ( screen_pos > w() - TRACK_SPACING ) {
		m_scrollPosition = get_real_position( screen_pos - ( w() - TRACK_SPACING - LEFT_TRACK_SPACING )  ); //(25 = stylus_width)
		e_scroll_position( m_scrollPosition, long( w() / SwitchBoard::i()->zoom() ), 1024  );
		redraw();
	} else {
		window()->make_current();
		fl_overlay_rect( get_screen_position(m_stylusPosition), y(), 1, h() );
	}
	e_stylus_position( get_screen_position(m_stylusPosition) );
	e_seek_position( m_stylusPosition );
}
void TimelineView::stylus( long stylus_pos )
{
	move_cursor( get_real_position( stylus_pos ) );
}

} /* namespace nle */
