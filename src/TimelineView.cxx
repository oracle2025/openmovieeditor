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

#include "sl/sl.h"

#include "nle.h"
#include "TimelineView.H"
#include "Timeline.H"
#include "SwitchBoard.H"
#include "VideoClip.H"
#include "VideoTrack.H"
#include "MoveDragHandler.H"
#include "SelectDragHandler.H"
#include "TrimDragHandler.H"
#include "Rect.H"
#include "events.H"
#include "FilmStrip.H"
#include "helper.H"
#include "ErrorDialog/IErrorHandler.H"
#include "AudioClip.H"
#include "AutomationDragHandler.H"
#include "ShiftAutomationDragHandler.H"
#include "IClipArtist.H"
#include "SimplePlaybackCore.H"
#include "DocManager.H"
#include "Commands.H"

#include "audio.xpm"
#include "video.xpm"
//#include "titles.xpm"

namespace nle
{

bool USING_AUDIO = 0;
	
TimelineView* g_timelineView = 0;

static Fl_Cursor current_cursor;
TimelineView::TimelineView( int x, int y, int w, int, const char *label )
	: Fl_Widget( x, y, w, 200, label )
{
	g_timelineView = this;
	m_dragHandler = NULL;
	

	m_scrollPosition = 0;
	m_stylusPosition = 0;
	SwitchBoard::i()->timelineView(this);
	current_cursor = FL_CURSOR_DEFAULT;
	m_vscroll = 0;
	m_selectedClips = 0;
}
TimelineView::~TimelineView()
{
}
#define MENU_ITEM_INIT 0, 0, 0, 0, 0, 0, 0, 0
int TimelineView::handle( int event )
{
  Clip* cl;
	if ( g_simplePlaybackCore->active() ) {
		return Fl_Widget::handle( event );
	}
	int _x = Fl::event_x();
	int _y = Fl::event_y() - y();
	switch ( event ) {
		case FL_PASTE:
			{
				Track* t = get_track( _x, _y );
				char *fn,*filename=strdup(Fl::event_text());
				int i=strlen(filename);
				while (i>0 && (iscntrl(filename[i]) || isspace(filename[i])) ) filename[i--]=0;
				if (!strncmp(filename,"file://",7)) fn=&(filename[7]); 
				else fn=filename;
				if (t && !fl_filename_isdir(fn)) {
					int64_t rp = get_real_position( _x, t->stretchFactor() );
					
					//t->addFile( rp, fn );

					Command* cmd = new AddCommand( fn, t, rp );
					g_docManager->submit( cmd );
					
					adjustScrollbar();
					redraw();
					g_videoView->seek( m_stylusPosition );
					g_timeline->changing();
				}
				free(filename);
			}
			return 1;
		case FL_DND_DRAG:
		case FL_DND_RELEASE:
		case FL_DND_ENTER:
		case FL_DND_LEAVE:
		case FL_ENTER:
			return 1;
	/*	case FL_ENTER:
			window()->cursor( FL_CURSOR_WE );
			return 1;*/
		case FL_LEAVE:
			window()->cursor( FL_CURSOR_DEFAULT );
			return 1;
		case FL_MOVE:
			if ( g_ui->automationsMode() != 0 ) {
				return 1;
			}
			cl = get_clip( _x, _y );
			if ( cl && ( _x < get_screen_position( cl->position(), cl->track()->stretchFactor() ) + 8 
					|| _x > get_screen_position( cl->position() + (cl->length()+1), cl->track()->stretchFactor() ) - 8 ) ) {
				if ( current_cursor != FL_CURSOR_WE ) {
					window()->cursor( FL_CURSOR_WE, fl_rgb_color(254,254,254), fl_rgb_color(1,1,1) );
					current_cursor = FL_CURSOR_WE;
				}
			} else {
				if ( current_cursor != FL_CURSOR_DEFAULT ) {
					window()->cursor( FL_CURSOR_DEFAULT );
					current_cursor = FL_CURSOR_DEFAULT;
				}
			}
			return 1;
		case FL_PUSH: {
				if ( _x > TRACK_SPACING + x() && _x < TRACK_SPACING + x() + 64 ) {
					int track_count = -1;
					for ( track_node* i = g_timeline->getTracks(); i; i = i->next ) {
						track_count++;
						int yy = TRACK_SPACING + track_count * TRACK_OFFSET;
						if ( _y > yy && _y < yy + TRACK_HEIGHT ) {
							Fl_Menu_Item menuitem[] = { { "Remove Track", MENU_ITEM_INIT }, { "Move Up", MENU_ITEM_INIT }, { "Move Down", MENU_ITEM_INIT }, { 0L, MENU_ITEM_INIT } };
							Fl_Menu_Item* r = (Fl_Menu_Item*)menuitem->popup( TRACK_SPACING + x(), yy + TRACK_HEIGHT + y() + 1 );
							if ( r == &menuitem[0] ) {
								clear_selection();
								Command* cmd = new RemoveTrackCommand( i->track );
								submit( cmd );
							} else if ( r == &menuitem[1] ) {
								g_timeline->trackUp( i->track );
								redraw();
							} else if ( r == &menuitem[2] ) {
								g_timeline->trackDown( i->track );
								redraw();
							}
							return 1;
						}
					}
				}
				cl = get_clip( _x, _y );
				VideoClip* vcl = 0;
				if ( cl ) {
					vcl = dynamic_cast<VideoClip*>(cl);
				}
				if ( cl && Fl::event_ctrl() ) {
					toggle_selection( cl );
					return 1;
				}
				if ( cl && g_ui->automationsMode() == 2 ) {
					return 1;
				}
				if ( cl && vcl && vcl->hasAudio() && ( Fl::event_button() == FL_RIGHT_MOUSE ) ) {
					if ( vcl->m_mute ) {
						Fl_Menu_Item menuitem[] = { { "Unmute Original Sound", MENU_ITEM_INIT }, { "Select all Clips after Cursor", MENU_ITEM_INIT }, { 0L, MENU_ITEM_INIT } };
						Fl_Menu_Item* r = (Fl_Menu_Item*)menuitem->popup( Fl::event_x(), Fl::event_y() );
						if ( r == &menuitem[0] ) {
							vcl->m_mute = false;
							redraw();
							g_timeline->changing();
						} else if ( r == &menuitem[1] ) {
							select_all_after_cursor();	
						}
					} else {
						Fl_Menu_Item menuitem[] = { { "Mute Original Sound", MENU_ITEM_INIT }, { "Select all Clips after Cursor", MENU_ITEM_INIT }, { 0L, MENU_ITEM_INIT } };
						Fl_Menu_Item* r = (Fl_Menu_Item*)menuitem->popup( Fl::event_x(), Fl::event_y() );
						if ( r == &menuitem[0] ) {
							vcl->m_mute = true;
							redraw();
							g_timeline->changing();
						} else if ( r == &menuitem[1] ) {
							select_all_after_cursor();	
						}
					}
					return 1;
				}
				if ( Fl::event_button() == FL_RIGHT_MOUSE ) {
					Fl_Menu_Item menuitem[] = { { "Select all Clips after Cursor", MENU_ITEM_INIT }, { 0L, MENU_ITEM_INIT } };
					if ( menuitem->popup( Fl::event_x(), Fl::event_y() ) ) {
						select_all_after_cursor();
					}
					return 1;
				}
				if ( cl && g_ui->automationsMode() == 1 && cl->has_automation() ) {
					if ( FL_SHIFT & Fl::event_state() ) {
						m_dragHandler = new ShiftAutomationDragHandler( cl, get_clip_rect( cl, true ), _x, _y );
					} else {
						m_dragHandler = new AutomationDragHandler( cl, get_clip_rect( cl, true ), 0, _x, _y );
					}
					return 1;
				} else if ( cl ) {
					if ( _x < get_screen_position( cl->position(), cl->track()->stretchFactor() ) + 8 ) {
						m_dragHandler = new TrimDragHandler(
								this, cl, cl->track()->num(),
								0, 0, false );
					} else if ( _x > get_screen_position( cl->position() + (cl->length()+1), cl->track()->stretchFactor() ) - 8) {
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
			}
			m_dragHandler = new SelectDragHandler( _x, _y );
			return 1;
		case FL_DRAG:
			if ( m_dragHandler ) {
				m_dragHandler->OnDrag( _x, _y );
				return 1;
			}
			return Fl_Widget::handle( event );
		case FL_RELEASE:
			if ( m_dragHandler ) {
				m_dragHandler->OnDrop( _x, _y );
				delete m_dragHandler;
				m_dragHandler = NULL;
				redraw();
				g_videoView->seek( m_stylusPosition );
				g_timeline->changing();
				return 1;
			}
			if ( g_ui->automationsMode() == 2 ) {
				cl = get_clip( _x, _y );
				if ( cl ) {
					split_clip( cl, _x );
					redraw();
					g_timeline->changing();
					return 1;
				}
			}
		default:
			return Fl_Widget::handle( event );	
	}
	
}
void TimelineView::resize(int x, int y, int w, int h)
{
	int track_count = 0;
	for ( track_node* i = g_timeline->getTracks(); i; i = i->next ) {
		track_count++;
	}
	int h_t = ( 2 * TRACK_SPACING ) + ( TRACK_OFFSET * track_count );
	int h_r = h_t > h ? h_t : h;
	Fl_Widget::resize( x, y, w, h_r );
	int a = g_v_scrollbar->value();
	int b = h_r - parent()->h();
	int v = a <= b ? a : b;
	if ( v < 0 ) { v = 0; }
	g_v_scrollbar->value( v, parent()->h(), 0, h_r );
	adjustScrollbar();
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
/*		if ( track_count == 0 ) {
			fl_draw( "Titles", x() + TRACK_SPACING + 23, y_coord + 18);
			fl_draw_pixmap( titles_xpm, x() + TRACK_SPACING + 3, y_coord + 5 );
		} else*/ if ( USING_AUDIO ) {
			fl_draw( "Audio", x() + TRACK_SPACING + 23, y_coord + 18);
			fl_draw_pixmap( audio_xpm, x() + TRACK_SPACING + 3, y_coord + 5 );
		} else {
			fl_draw( "Video", x() + TRACK_SPACING + 23, y_coord + 18);
			fl_draw_pixmap( video_xpm, x() + TRACK_SPACING + 6, y_coord + 8 );
		}
	// END - Draw Button

	//     - Draw Track Background
		fl_draw_box( FL_BORDER_BOX, x_coord, y_coord, w_size, TRACK_HEIGHT, FL_DARK2 );
	// END - Draw Track Background

		fl_push_clip( x_coord, y_coord, w_size, TRACK_HEIGHT );
		
		for ( clip_node* j = track->getClips(); j; j = j->next ) {
			Clip* clip = j->clip;
			int64_t scr_clip_x = get_screen_position( clip->position(), track->stretchFactor() );
			int64_t scr_clip_y = y_coord;
			int64_t scr_clip_w = (int)( (clip->length() + 1) * SwitchBoard::i()->zoom() / track->stretchFactor() );
			int64_t scr_clip_h = TRACK_HEIGHT;

			if ( scr_clip_x + scr_clip_w < 0 )
				continue;
			if ( scr_clip_x > (int64_t)w() + (int64_t)x() )
				continue;

			if ( scr_clip_x < (int64_t)x() ) {
				scr_clip_w = scr_clip_w + scr_clip_x - x() + 5;
				scr_clip_x = x() - 5;
			}

			if ( scr_clip_x + scr_clip_w > (int64_t)w() + (int64_t)x() ) {
				scr_clip_w = (int64_t)(x() + w()) - scr_clip_x;
			}

			
			
			fl_draw_box( FL_BORDER_BOX , scr_clip_x, scr_clip_y, scr_clip_w, scr_clip_h, FL_DARK3 );
			
			Rect r( scr_clip_x, scr_clip_y, scr_clip_w, scr_clip_h );
			IClipArtist* artist = clip->getArtist();
			if ( artist ) {
				artist->render( r, get_real_position( scr_clip_x, track->stretchFactor() ),
						get_real_position( scr_clip_x + scr_clip_w, track->stretchFactor() ) );
			}
			if ( clip->selected() ) {
				fl_draw_box( FL_BORDER_FRAME, scr_clip_x, scr_clip_y, scr_clip_w, scr_clip_h, FL_RED );
			} else {
				fl_draw_box( FL_BORDER_FRAME, scr_clip_x, scr_clip_y, scr_clip_w, scr_clip_h, FL_DARK3 );
			}
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
		if ( dynamic_cast<VideoTrack*>(track) ) {
			for ( clip_node* j = track->getClips(); j; j = j->next ) {
				Clip* clip = j->clip;
				for ( clip_node* k = j->next; k; k = k->next ) {
					Clip* cclip = k->clip;
					if ( clip->A() < cclip->A() && clip->B() > cclip->A() ) {
						int x = get_screen_position( cclip->A(), track->stretchFactor() );
						int w = get_screen_position( clip->B()+1, track->stretchFactor() ) - x;
						fl_draw_box( FL_FLAT_BOX, x, y_coord, w, TRACK_HEIGHT, FL_DARK_BLUE );
						fl_draw_box( FL_BORDER_FRAME, x, y_coord, w, TRACK_HEIGHT, FL_RED );
						fl_color( FL_RED );
						fl_line( x, y_coord, x + w, y_coord + TRACK_HEIGHT );
						fl_line( x + w, y_coord, x, y_coord + TRACK_HEIGHT );
					} else if ( cclip->A() < clip->A() && cclip->B() > clip->A() ) {
						int x = get_screen_position( clip->A(), track->stretchFactor() );
						int w = get_screen_position( cclip->B()+1, track->stretchFactor() ) - x;
						fl_draw_box( FL_FLAT_BOX, x, y_coord, w, TRACK_HEIGHT, FL_DARK_BLUE );
						fl_draw_box( FL_BORDER_FRAME, x, y_coord, w, TRACK_HEIGHT, FL_RED );
						fl_color( FL_RED );
						fl_line( x, y_coord, x + w, y_coord + TRACK_HEIGHT );
						fl_line( x + w, y_coord, x, y_coord + TRACK_HEIGHT );
					}

				}
			}
		}
		for ( clip_node* j = track->getClips(); j; j = j->next ) {
			Clip* clip = j->clip;
			int scr_clip_x = get_screen_position( clip->position(), track->stretchFactor() );
			int scr_clip_y = y_coord;
			int scr_clip_w = (int)( (clip->length()+1) * SwitchBoard::i()->zoom() / track->stretchFactor() );
			//int scr_clip_h = TRACK_HEIGHT;
			for ( int k = 0; k < 2; k++ ) {
				if ( !k ) {
					fl_color( FL_BLACK );
					fl_begin_polygon();
				} else {
					fl_color( FL_DARK2 );
					fl_begin_loop();
				}
				fl_vertex( scr_clip_x, scr_clip_y + TRACK_HEIGHT / 2 );
				fl_vertex( scr_clip_x + 8, scr_clip_y + TRACK_HEIGHT / 2 - 5 );
				fl_vertex( scr_clip_x + 8, scr_clip_y + TRACK_HEIGHT / 2 + 5 );
				if ( !k ) {
					fl_end_polygon();
					fl_begin_polygon();
				} else {
					fl_end_loop();
					fl_begin_loop();
				}
				fl_vertex( scr_clip_x + scr_clip_w, scr_clip_y + TRACK_HEIGHT / 2 );
				fl_vertex( scr_clip_x + scr_clip_w - 8, scr_clip_y + TRACK_HEIGHT / 2 - 5 );
				fl_vertex( scr_clip_x + scr_clip_w - 8, scr_clip_y + TRACK_HEIGHT / 2 + 5 );
				if ( !k ) {
					fl_end_polygon();
				} else {
					fl_end_loop();
				}
			}
		}

		fl_pop_clip();
	}
	fl_pop_clip();
	fl_overlay_rect( get_screen_position( m_stylusPosition ), parent()->y(), 1, parent()->h() );
}
/*void TimelineView::add_video( int track, int y, string filename )
{
	//m_timeline->add_video( track, get_real_position( y ), filename );
}*/
int64_t TimelineView::get_real_position( int p, float stretchFactor )
{
	return int64_t( ( ( float(p - LEFT_TRACK_SPACING - x() ) / SwitchBoard::i()->zoom() ) + m_scrollPosition ) * stretchFactor );
}
int64_t TimelineView::get_screen_position( int64_t p, float stretchFactor )
{
	return int64_t ( float( p - ( m_scrollPosition * stretchFactor ) ) * SwitchBoard::i()->zoom() / stretchFactor ) + LEFT_TRACK_SPACING + x();

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
	int i = -1;
	for ( track_node* o = g_timeline->getTracks(); o; o = o->next ) {
		i++;
		if ( !get_track_rect( i ).inside( _x, _y ) ) {
			continue;
		}
		return o->track;
	}
	return NULL;
}
Clip* TimelineView::get_clip( int _x, int _y )
{
	Track *tr = get_track( _x, _y );
	if ( !tr ) {
		return NULL;
	}
	for ( clip_node* p = tr->getClips(); p; p = p->next ) {
		Rect tmp = get_clip_rect( p->clip, true );
		if ( !tmp.inside( _x, _y ) )
			continue;
		return p->clip;
	}
	return NULL;
}
int get_track_order( Track* track ) {
	int t = -1;
	for ( track_node* o = g_timeline->getTracks(); o; o = o->next ) {
		t++;
		if ( track == o->track ) {
			return t;
		}
	}
	return 0;
}
Rect TimelineView::get_track_rect( Track* track )
{
	Rect tmp(
			x() + LEFT_TRACK_SPACING,
			TRACK_SPACING + (TRACK_SPACING + TRACK_HEIGHT) * get_track_order( track ),
			w() - ( TRACK_SPACING + LEFT_TRACK_SPACING ),
			TRACK_HEIGHT
		);
	return tmp;
}
Rect TimelineView::get_track_rect( int track )
{
	Rect tmp(
			x() + LEFT_TRACK_SPACING,
			TRACK_SPACING + (TRACK_SPACING + TRACK_HEIGHT) * track,
			w() - ( TRACK_SPACING + LEFT_TRACK_SPACING ),
			TRACK_HEIGHT
		);
	return tmp;
}
Rect TimelineView::get_clip_rect( Clip* clip, bool clipping )
{
	
	Rect tmp(
			get_screen_position( clip->position(), clip->track()->stretchFactor() ),
			int( TRACK_SPACING + (TRACK_SPACING + TRACK_HEIGHT) * get_track_order( clip->track() ) ),
			int( (clip->length()+1) * SwitchBoard::i()->zoom() / clip->track()->stretchFactor() ),
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
		Command* cmd;
		if ( m_selectedClips ) {
			cmd = new RemoveSelectionCommand( m_selectedClips );
			submit( cmd );
			adjustScrollbar();
			clear_selection();
			return;
		} else {
			cmd = new RemoveCommand( clip );
			g_docManager->submit( cmd );
			adjustScrollbar();
			return;
		}
	}
	if (!new_tr || new_tr->type() != old_tr->type() ) {
		return;
	}
	int64_t new_position = get_real_position( _x - offset, clip->track()->stretchFactor() );
	if ( g_snap ) {
		new_position = new_tr->getSnapA( clip, new_position );
		new_position = new_tr->getSnapB( clip, new_position );
	}
	if ( new_position < 0 ) {
		new_position = 0;
	}
	Command* cmd;
	if ( m_selectedClips && new_tr == old_tr ) {
		cmd = new MoveSelectionCommand( clip, new_position, m_selectedClips );
	} else {
		clear_selection();
		cmd = new MoveCommand( clip, new_tr, new_position );
	}
	submit( cmd );
	adjustScrollbar();
}
void TimelineView::add_track( int type )
{
	Command* cmd = new AddTrackCommand( type );
	submit( cmd );
}
void TimelineView::adjustScrollbar()
{
	g_scrollBar->value( m_scrollPosition, long( w() / SwitchBoard::i()->zoom() ),0, g_timeline->length() + long( w() / SwitchBoard::i()->zoom() ) );
}
void TimelineView::split_clip( Clip* clip, int _x )
{
	int64_t split_position = get_real_position(_x, clip->track()->stretchFactor() );
	Command* cmd = new SplitCommand( clip, split_position );
	g_docManager->submit( cmd );
}
void TimelineView::clear_selection()
{
	clip_node* node;
	while ( ( node = (clip_node*)sl_pop( &m_selectedClips ) ) ) {
		node->clip->selected( false );
		delete node;
	}
}
void TimelineView::select_clips( int _x1, int _y1, int _x2, int _y2 )
{
	clear_selection();
	if ( _x1 > _x2 ) { int k; k = _x1; _x1 = _x2; _x2 = k; }
	if ( _y1 > _y2 ) { int k; k = _y1; _y1 = _y2; _y2 = k; }
	int i = -1;
	Rect r;
	for ( track_node* o = g_timeline->getTracks(); o; o = o->next ) {
		i++;
		r = get_track_rect( i );
		if ( !( _y1 <= y() + r.y && _y2 >= y() + r.y + r.h ) ) {
			continue;
		}
		for ( clip_node* c = o->track->getClips(); c; c = c->next ) {
			r = get_clip_rect( c->clip, false );
			if ( !(_x1 <= r.x && _x2 >= r.x + r.w) ) {
				continue;
			}
			// Add Clip to Selection and mark them
			clip_node* n = new clip_node;
			n->next = 0;
			n->clip = c->clip;
			c->clip->selected( true );
			m_selectedClips = (clip_node*)sl_push( m_selectedClips, n );
		}
	}
}
void TimelineView::select_all_after_cursor()
{
	clear_selection();
	for ( track_node* o = g_timeline->getTracks(); o; o = o->next ) {
		for ( clip_node* c = o->track->getClips(); c; c = c->next ) {
			if ( c->clip->position() / c->clip->track()->stretchFactor() >= m_stylusPosition ) {
				clip_node* n = new clip_node;
				n->next = 0;
				n->clip = c->clip;
				c->clip->selected( true );
				m_selectedClips = (clip_node*)sl_push( m_selectedClips, n );
			}
		}
	}
	redraw();
}
static int remove_clip_helper( void* p, void* data )
{
	Clip* clip = (Clip*)data;
	clip_node* node = (clip_node*)p;
	if ( node->clip == clip ) {
		return 1;
	} else {
		return 0;
	}
}
void TimelineView::toggle_selection( Clip* clip )
{
	if ( clip->selected() ) {
		clip->selected( false );
		clip_node* n = (clip_node*)sl_remove( &m_selectedClips, remove_clip_helper, clip );
		if ( n ) {
			delete n;
		}
	} else {
		clip->selected( true );
		clip_node* n = new clip_node;
		n->next = 0;
		n->clip = clip;
		clip->selected( true );
		m_selectedClips = (clip_node*)sl_push( m_selectedClips, n );
	}
	redraw();
}
void TimelineView::trim_clip( Clip* clip, int _x, bool trimRight )
{
	Command* cmd = new TrimCommand( clip, get_real_position( _x, clip->track()->stretchFactor() ), trimRight );
	g_docManager->submit( cmd );
	/*
	if (trimRight) {
		int64_t trimv = int64_t( ( clip->position() + clip->length() ) - ( get_real_position(_x, clip->track()->stretchFactor()) ) );
		clip->trimB( trimv );
		adjustScrollbar();
		return;
	}
	clip->trimA( int64_t( get_real_position(_x, clip->track()->stretchFactor())  - clip->position() ) );*/
	adjustScrollbar();
}
void TimelineView::move_cursor( int64_t position )
{
	m_stylusPosition = position;
	if ( m_stylusPosition < 0 ) {
		m_stylusPosition = 0;
	}
	window()->make_current();
	long screen_pos = get_screen_position(m_stylusPosition);
	if ( screen_pos < ( LEFT_TRACK_SPACING + x() ) ) {
		m_scrollPosition = get_real_position( screen_pos );
		adjustScrollbar();
		redraw();
	} else if ( screen_pos > w() + x() - TRACK_SPACING - 50 ) {
		m_scrollPosition = get_real_position( screen_pos );
		adjustScrollbar();
		redraw();
	} else {
		window()->make_current();
		fl_overlay_rect( get_screen_position(m_stylusPosition), parent()->y(), 1, parent()->h() );
	}
	e_stylus_position( get_screen_position(m_stylusPosition) );
	e_seek_position( m_stylusPosition );
}
void TimelineView::stylus( long stylus_pos )
{
	move_cursor( get_real_position( stylus_pos ) );
}

} /* namespace nle */
