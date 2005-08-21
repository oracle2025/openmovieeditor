/*  TimelineView.cxx
 *
 *  Copyright (C) 2003 Richard Spindler <richard.spindler AT gmail.com>
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

#include <iostream>

#include <FL/Fl.H>
#include <FL/Fl_Widget.H>
#include <FL/Fl_Window.H>
#include <FL/fl_draw.H>

#include "TimelineView.H"
#include "Timeline.H"
#include "SwitchBoard.H"
#include "Draw.H"
#include "VideoClip.H"
#include "VideoTrack.H"
#include "MoveDragHandler.H"
#include "TrimDragHandler.H"
#include "Rect.H"
#include "events.H"
#include "FilmStrip.H"
#include "Project.H"
#include "helper.H"

#include "audio.xpm"
#include "video.xpm"

using namespace std;

namespace nle
{

bool USING_AUDIO = 0;
	
TimelineView* g_timelineView = 0;

TimelineView::TimelineView( int x, int y, int w, int h, const char *label )
	: Fl_Widget( x, y, w, h, label )//, Flu_DND("DND_Timeline")
{
	m_dragHandler = NULL;
	m_timeline = new Timeline();
	
//	m_timeline->add_video( 0, 30, "/home/oracle/tmp/test3.mov" );
//	m_timeline->add_video( 1, 100, "/home/oracle/tmp/test3.mov" );
//	m_timeline->add_audio( 2, 0, "/home/oracle/tmp/test3.mov" );

	m_scrollPosition = 0;
	m_stylusPosition = 0;
	SwitchBoard::i()->timelineView(this);
	g_timelineView = this;

	Project::write_project();
	
}
TimelineView::~TimelineView()
{
	cout << "Never Called" << endl;
	delete m_timeline;
	m_timeline = NULL;
}
int TimelineView::handle( int event )
{
	int _x = Fl::event_x() - x();
	int _y = Fl::event_y() - y();
	switch ( event ) {
		case FL_PASTE:
			cout << "FL_PASTE" << endl;
			cout << "Text: (" << Fl::event_text() << ") " << Fl::event_length() << endl;
			cout << "X: " << Fl::event_x() << " Y: " << Fl::event_y() << endl;
			{
				int64_t rp = get_real_position(_x);
				cout << "Real Position: " << rp << endl;
				Track* t = get_track( _x, _y );
				if (t && !fl_filename_isdir(Fl::event_text()) ) {
					
					cout << "Track: " << t->num() << endl;
					t->add( Fl::event_text(), int64_t(rp * t->stretchFactor()) );
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
				if (cl) {
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
	fl_draw_box( FL_FLAT_BOX, x(), y(), w(), h(), FL_BACKGROUND_COLOR );
	

	std::list< Track* >* vtl = m_timeline->getTracks();
	std::list< Clip* >* vcl;
	float l, p;
	int cnt = 0;


	for ( std::list< Track* >::iterator i = vtl->begin(); i != vtl->end(); i++ ) {
		vcl = (*i)->getClips();
		USING_AUDIO = (*i)->type() == TRACK_TYPE_AUDIO;
		
		fl_push_matrix();
		
		{
			int box_x = x() + TRACK_SPACING;
			int box_y = y() + TRACK_SPACING + cnt * (TRACK_HEIGHT + TRACK_SPACING);
			
			fl_draw_box( FL_UP_BOX, box_x,
					box_y,
					64, TRACK_HEIGHT + 1, FL_BACKGROUND_COLOR );
			
			fl_color( FL_BLACK );
			fl_font( FL_HELVETICA, 11 );
				
			char *s;
			if ( USING_AUDIO ) {
				s = "Audio";
				fl_draw_pixmap( audio_xpm, box_x + 3,
						box_y + 5 );
			} else {
				s = "Video";
				fl_draw_pixmap( video_xpm, box_x + 6,
						box_y + 8 );
			}
			
			fl_draw( s, box_x + 23, 
					box_y + 18);
		}
		fl_translate( x() + LEFT_TRACK_SPACING,
				y() + TRACK_SPACING + cnt * (TRACK_HEIGHT + TRACK_SPACING) );
		
		Draw::box( 0, 0, w() - TRACK_SPACING - LEFT_TRACK_SPACING, TRACK_HEIGHT, FL_DARK2 );

		fl_push_clip( x() + LEFT_TRACK_SPACING,
				y() + TRACK_SPACING + cnt * (TRACK_HEIGHT + TRACK_SPACING),
				w() - TRACK_SPACING - LEFT_TRACK_SPACING,
				TRACK_HEIGHT );
		fl_scale( SwitchBoard::i()->zoom() / (*i)->stretchFactor(), 1.0 );
		fl_translate( - (m_scrollPosition * (*i)->stretchFactor()), 0.0 );
		//if ( (*i)->type() == TRACK_TYPE_VIDEO ) {
			for ( std::list< Clip* >::iterator j = vcl->begin(); j != vcl->end(); j++ ) {
				p = float( (*j)->position() );
				l = float( (*j)->length() );
				Draw::box( p, 0, l, TRACK_HEIGHT, FL_DARK3 );
				if ( (*i)->type() == TRACK_TYPE_VIDEO ) {
					int _x, _y;
					VideoClip* vc = ((VideoClip*)(*j));
					_y = y() + TRACK_SPACING + cnt * (TRACK_HEIGHT + TRACK_SPACING);
					FilmStrip* fs = vc->getFilmStrip();
					int s = vc->trimA() / 100;
					int e = ( vc->length() / 100 ) + s;
					int off = vc->trimA() % 100;
					for ( int k = s; k < e; k++ ) {
						_x = get_screen_position( (*j)->position() + (k - s) * 100, (*i)->stretchFactor()  ) - off;
						pic_struct* f = fs->get_pic(k);
						fl_draw_image( f->data, _x, _y, f->w, f->h );
					}
				}
				Draw::triangle( p, TRACK_HEIGHT/2, true );
				Draw::triangle( p + l, TRACK_HEIGHT/2, false );
			}
		/*} else {
			for ( std::list< Clip* >::iterator j = vcl->begin(); j != vcl->end(); j++ ) {
				p = float( (*j)->position() );
				l = float( (*j)->length() );
				Draw::box( p / 1601.6, 0, l / 1601.6, TRACK_HEIGHT, FL_DARK3 );
				Draw::triangle( p / 1601.6, TRACK_HEIGHT/2, true );
				Draw::triangle( (p + l) / 1601.6, TRACK_HEIGHT/2, false );
			}
		}*/
		fl_pop_clip();
		fl_pop_matrix();
		cnt++;
	}
	fl_pop_clip();
	fl_overlay_rect( get_screen_position(m_stylusPosition), y(), 1, h() );
	
	/*
	for i in VideoTracks:
		draw VideoTrackBackground
			for j in i(VideoTrack):
				drawClip i
	 */
}
void TimelineView::add_video( int track, int y, const char* filename )
{
	//m_timeline->add_video( track, get_real_position( y ), filename );
}
int64_t TimelineView::get_real_position( int p )
{
	return int64_t( float(p - LEFT_TRACK_SPACING) / SwitchBoard::i()->zoom() ) + m_scrollPosition;
}
int TimelineView::get_screen_position( int64_t p, float stretchFactor )
{
	return int ( float( p - ( m_scrollPosition * stretchFactor ) ) * SwitchBoard::i()->zoom() / stretchFactor ) + LEFT_TRACK_SPACING;

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
	std::list< Track* >* vtl = m_timeline->getTracks();
	for ( std::list< Track* >::iterator i = vtl->begin(); i != vtl->end(); i++ ) {
		if ( !get_track_rect(*i).inside( _x, _y ) )
			continue;
		return *i;
	}
	return NULL;
}
Clip* TimelineView::get_clip( int _x, int _y )
{
	Track *tr = get_track( _x, _y );
	if (!tr)
		return NULL;
	std::list< Clip* >* vcl = tr->getClips();
	for ( std::list< Clip* >::iterator j = vcl->begin(); j != vcl->end(); j++ ) {
		Rect tmp = get_clip_rect(*j);
		if ( !tmp.inside( _x, _y ) )
			continue;
		return *j;
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
		if ( tmp.x < 0 ) {
			tmp.w += tmp.x;
			tmp.x = 0;
		}
		if ( tmp.w > w() - 2 * TRACK_SPACING ) {
			tmp.w = w() - 2 * TRACK_SPACING;
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
	if ( inside_widget( g_trashCan, x() + _x, y() + _y ) ) {
		old_tr->remove( clip );
		delete clip;
		return;
	}
	if (!new_tr || new_tr->type() != old_tr->type() ) {
		return;
	}
	clip->position( int64_t( get_real_position( _x - offset ) * clip->track()->stretchFactor() ) );
	if ( new_tr == old_tr ) {
		return;
	}
	old_tr->remove( clip );
	clip->track( new_tr );
	new_tr->add( clip );
}
void TimelineView::trim_clip( Clip* clip, int _x, bool trimRight )
{
	if (trimRight) {
		clip->trimB( int64_t( ( clip->position() + clip->length() ) - ( get_real_position(_x) * clip->track()->stretchFactor() ) ) );
		return;
	}
	clip->trimA( int64_t( get_real_position(_x) * clip->track()->stretchFactor() - clip->position() ) );
}
void TimelineView::move_cursor( int64_t position )
{
	m_stylusPosition = position;
	//cout << timestamp_to_string( position ) << endl;
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
