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
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


#include <FL/Fl.H>
#include <FL/Fl_Widget.H>
#include <FL/Fl_Window.H>
#include <FL/fl_draw.H>
#include <FL/Fl_Help_Dialog.H>

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
#include "IPlaybackCore.H"
#include "DocManager.H"
#include "Commands.H"
#include "Frei0rFactoryPlugin.H"
#include "Frei0rEffect.H"
#include "Frei0rDialog.H"
#include "PasteSelectionCommand.H"
#include "TitleClip.H"

#include "audio.xpm"
#include "video.xpm"

namespace nle
{
int get_track_top( Track* track );
bool USING_AUDIO = 0;
	
TimelineView* g_timelineView = 0;

static Fl_Cursor current_cursor;
TimelineView::TimelineView( int x, int y, int w, int, const char *label )
	: Fl_Widget( x, y, w, 200, label )
{
	g_timelineView = this;
	m_dragHandler = NULL;
	m_pasteCommand = 0;

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
	if ( g_playbackCore->active() ) {
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
						int yy = get_track_top( i->track );
						if ( _y > yy && _y < yy + i->track->h() ) {
							Fl_Menu_Item menuitem[] = { { "Remove Track", MENU_ITEM_INIT }, { "Move Up", MENU_ITEM_INIT }, { "Move Down", MENU_ITEM_INIT }, { "Rename", 0,0,0,FL_MENU_DIVIDER,0,0,0,0 }, { "1x", MENU_ITEM_INIT }, { "2x", MENU_ITEM_INIT }, { "4x", MENU_ITEM_INIT }, { 0L, MENU_ITEM_INIT } };
							Fl_Menu_Item* r = (Fl_Menu_Item*)menuitem->popup( TRACK_SPACING + x(), yy + i->track->h() + y() + 1 );
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
							} else if ( r == &menuitem[3] ) {
								const char* name = fl_input( "Please enter the track name.", i->track->name().c_str() );
								if ( name ) {
									i->track->name( name );
									redraw();
								}
							} else if ( r == &menuitem[4] ) {
								i->track->h( 30 );
								redraw();
							} else if ( i->track->type() == TRACK_TYPE_AUDIO && r == &menuitem[5] ) {
								i->track->h( 60 );
								redraw();
							} else if ( r == &menuitem[6] ) {
								i->track->h( 120 );
								redraw();
							} else if ( i->track->type() == TRACK_TYPE_VIDEO && r == &menuitem[5] ) {
								i->track->h( 65 );
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
						Fl_Menu_Item menuitem[] = { { "Crop", 0,0,0,FL_MENU_RADIO|( vcl->crop() ? FL_MENU_VALUE : 0 ),0,0,0,0 },
							{ "Fit", 0,0,0,FL_MENU_RADIO|( vcl->fit() ? FL_MENU_VALUE : 0 ),0,0,0,0 },
							{ "Stretch", 0,0,0,FL_MENU_RADIO |( vcl->stretch() ? FL_MENU_VALUE : 0 ),0,0,0,0 },
							{ "Default", 0,0,0,FL_MENU_RADIO | FL_MENU_DIVIDER|( vcl->def() ? FL_MENU_VALUE : 0 ),0,0,0,0 }, 
							{ "Unmute Original Sound", MENU_ITEM_INIT },
							{ "Freeze Frame", 0,0,0,FL_MENU_TOGGLE|( vcl->freezeFrame() ? FL_MENU_VALUE : 0 ),0,0,0,0 },
							{ "Select all Clips after Cursor", MENU_ITEM_INIT }, 
							{ 0L, MENU_ITEM_INIT } };
						//menuitem[2].setonly();
						Fl_Menu_Item* r = (Fl_Menu_Item*)menuitem->popup( Fl::event_x(), Fl::event_y() );
						if ( r == &menuitem[0] ) {
							vcl->crop( true );
							g_videoView->redraw();
						} else if ( r == &menuitem[1] ) {
							vcl->fit( true );
							g_videoView->redraw();
						} else if ( r == &menuitem[2] ) {
							vcl->stretch( true );
							g_videoView->redraw();
						} else if ( r == &menuitem[3] ) {
							vcl->def( true );
							g_videoView->redraw();
						} else if ( r == &menuitem[4] ) {
							vcl->m_mute = false;
							redraw();
							g_timeline->changing();
						} else if ( r == &menuitem[5] ) {
							vcl->freezeFrame( !vcl->freezeFrame() );
							g_videoView->redraw();
						} else if ( r == &menuitem[6] ) {
							select_all_after_cursor();	
						}
					} else {
						Fl_Menu_Item menuitem[] = { { "Crop", 0,0,0,FL_MENU_RADIO|( vcl->crop() ? FL_MENU_VALUE : 0 ),0,0,0,0 },
							{ "Fit", 0,0,0,FL_MENU_RADIO|( vcl->fit() ? FL_MENU_VALUE : 0 ),0,0,0,0 },
							{ "Stretch", 0,0,0,FL_MENU_RADIO | ( vcl->stretch() ? FL_MENU_VALUE : 0 ),0,0,0,0 }, 
							{ "Default", 0,0,0,FL_MENU_RADIO | FL_MENU_DIVIDER|( vcl->def() ? FL_MENU_VALUE : 0 ),0,0,0,0 }, 
							{ "Mute Original Sound", MENU_ITEM_INIT }, 
							{ "Freeze Frame", 0,0,0,FL_MENU_TOGGLE|( vcl->freezeFrame() ? FL_MENU_VALUE : 0 ),0,0,0,0 },
							{ "Select all Clips after Cursor", MENU_ITEM_INIT }, 
							{ 0L, MENU_ITEM_INIT } };
						//menuitem[2].setonly();
						Fl_Menu_Item* r = (Fl_Menu_Item*)menuitem->popup( Fl::event_x(), Fl::event_y() );
						if ( r == &menuitem[0] ) {
							vcl->crop( true );
							g_videoView->redraw();
						} else if ( r == &menuitem[1] ) {
							vcl->fit( true );
							g_videoView->redraw();
						} else if ( r == &menuitem[2] ) {
							vcl->stretch( true );
							g_videoView->redraw();
						} else if ( r == &menuitem[3] ) {
							vcl->def( true );
							g_videoView->redraw();
						} else if ( r == &menuitem[4] ) {
							vcl->m_mute = true;
							redraw();
							g_timeline->changing();
						} else if ( r == &menuitem[5] ) {
							vcl->freezeFrame( !vcl->freezeFrame() );
							g_videoView->redraw();
						} else if ( r == &menuitem[6] ) {
							select_all_after_cursor();	
						}
					}
					return 1;
				}
				VideoEffectClip* vec;
				if ( ( vec = dynamic_cast<VideoEffectClip*>(cl) ) && ( Fl::event_button() == FL_RIGHT_MOUSE ) ) {
						Fl_Menu_Item menuitem[] = { { "Crop", 0,0,0,FL_MENU_RADIO|( vec->crop() ? FL_MENU_VALUE : 0 ),0,0,0,0 },
							{ "Fit", 0,0,0,FL_MENU_RADIO|( vec->fit() ? FL_MENU_VALUE : 0 ),0,0,0,0 },
							{ "Stretch", 0,0,0,FL_MENU_RADIO |( vec->stretch() ? FL_MENU_VALUE : 0 ),0,0,0,0 },
							{ "Default", 0,0,0,FL_MENU_RADIO | FL_MENU_DIVIDER|( vec->def() ? FL_MENU_VALUE : 0 ),0,0,0,0 }, 
							{ "Select all Clips after Cursor", MENU_ITEM_INIT }, 
							{ "Freeze Frame", 0,0,0,FL_MENU_TOGGLE|( vec->freezeFrame() ? FL_MENU_VALUE : 0 ),0,0,0,0 },
							{ 0L, MENU_ITEM_INIT } };
						//menuitem[2].setonly();
						Fl_Menu_Item* r = (Fl_Menu_Item*)menuitem->popup( Fl::event_x(), Fl::event_y() );
						if ( r == &menuitem[0] ) {
							vec->crop( true );
							g_videoView->redraw();
						} else if ( r == &menuitem[1] ) {
							vec->fit( true );
							g_videoView->redraw();
						} else if ( r == &menuitem[2] ) {
							vec->stretch( true );
							g_videoView->redraw();
						} else if ( r == &menuitem[3] ) {
							vec->def( true );
							g_videoView->redraw();
						} else if ( r == &menuitem[4] ) {
							select_all_after_cursor();	
						} else if ( r == &menuitem[5] ) {
							vec->freezeFrame( !vec->freezeFrame() );
							g_videoView->redraw();
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
	int h_t = 2 * TRACK_SPACING;
	for ( track_node* i = g_timeline->getTracks(); i; i = i->next ) {
		h_t += i->track->h() + TRACK_SPACING;
	}
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
	int y_coord = y() + TRACK_SPACING;
	for ( track_node* i = g_timeline->getTracks(); i; i = i->next ) {
		Track* track = i->track;
		track_count++;
		int x_coord = x() + LEFT_TRACK_SPACING;
		int w_size = w() - TRACK_SPACING - LEFT_TRACK_SPACING;
		
		USING_AUDIO = track->type() == TRACK_TYPE_AUDIO;
		
	//     - Draw Button
		fl_draw_box( FL_UP_BOX, x() + TRACK_SPACING, y_coord, 64, track->h() + 1, FL_BACKGROUND_COLOR );
		fl_color( FL_BLACK );
		fl_font( FL_HELVETICA, 11 );
		if ( USING_AUDIO ) {
			fl_draw( track->name().c_str(), x() + TRACK_SPACING + 23, y_coord + 18);
			fl_draw_pixmap( audio_xpm, x() + TRACK_SPACING + 3, y_coord + 5 );
		} else {
			fl_draw( track->name().c_str(), x() + TRACK_SPACING + 23, y_coord + 18);
			fl_draw_pixmap( video_xpm, x() + TRACK_SPACING + 6, y_coord + 8 );
		}
	// END - Draw Button

	//     - Draw Track Background
		fl_draw_box( FL_BORDER_BOX, x_coord, y_coord, w_size, track->h(), FL_DARK2 );
	// END - Draw Track Background

		fl_push_clip( x_coord, y_coord, w_size, track->h() );
		
		for ( clip_node* j = track->getClips(); j; j = j->next ) {
			Clip* clip = j->clip;
			int64_t scr_clip_x = get_screen_position( clip->position(), track->stretchFactor() );
			int64_t scr_clip_y = y_coord;
			int64_t scr_clip_w = llrint( (clip->length() + 1) * SwitchBoard::i()->zoom() / track->stretchFactor() );
			int64_t scr_clip_h = track->h();

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
			if ( clip->has_automation() /*track->type() == TRACK_TYPE_AUDIO*/ ) {
				//Draw Automations
				AudioClipBase* audioClip = dynamic_cast<AudioClipBase*>(clip);

				if ( !audioClip ) {
					continue;
				}
				float stretchF;
				if ( track->type() == TRACK_TYPE_AUDIO ) {
					stretchF = track->stretchFactor();
				} else {
					stretchF = ( 48000 );
				} 
				fl_push_clip( scr_clip_x, scr_clip_y, scr_clip_w, scr_clip_h );
				
				auto_node* nodes = audioClip->getAutoPoints();

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

		}
		if ( dynamic_cast<VideoTrack*>(track) ) {
			for ( clip_node* j = track->getClips(); j; j = j->next ) {
				Clip* clip = j->clip;
				for ( clip_node* k = j->next; k; k = k->next ) {
					Clip* cclip = k->clip;
					if ( clip->A() < cclip->A() && clip->B() > cclip->A() ) {
						int x = get_screen_position( cclip->A(), track->stretchFactor() );
						int w = get_screen_position( clip->B()+1, track->stretchFactor() ) - x;
						fl_draw_box( FL_FLAT_BOX, x, y_coord, w, track->h(), FL_DARK_BLUE );
						fl_draw_box( FL_BORDER_FRAME, x, y_coord, w, track->h(), FL_RED );
						fl_color( FL_RED );
						fl_line( x, y_coord, x + w, y_coord + track->h());
						fl_line( x + w, y_coord, x, y_coord + track->h());
					} else if ( cclip->A() < clip->A() && cclip->B() > clip->A() ) {
						int x = get_screen_position( clip->A(), track->stretchFactor() );
						int w = get_screen_position( cclip->B()+1, track->stretchFactor() ) - x;
						fl_draw_box( FL_FLAT_BOX, x, y_coord, w, track->h(), FL_DARK_BLUE );
						fl_draw_box( FL_BORDER_FRAME, x, y_coord, w, track->h(), FL_RED );
						fl_color( FL_RED );
						fl_line( x, y_coord, x + w, y_coord + track->h());
						fl_line( x + w, y_coord, x, y_coord + track->h());
					}

				}
			}
		}
		for ( clip_node* j = track->getClips(); j; j = j->next ) {
			Clip* clip = j->clip;
			int scr_clip_x = get_screen_position( clip->position(), track->stretchFactor() );
			int scr_clip_y = y_coord;
			int scr_clip_w = (int)( (clip->length()+1) * SwitchBoard::i()->zoom() / track->stretchFactor() );
			for ( int k = 0; k < 2; k++ ) {
				if ( !k ) {
					fl_color( FL_BLACK );
					fl_begin_polygon();
				} else {
					fl_color( FL_DARK2 );
					fl_begin_loop();
				}
				fl_vertex( scr_clip_x, scr_clip_y + track->h()/ 2 );
				fl_vertex( scr_clip_x + 8, scr_clip_y + track->h()/ 2 - 5 );
				fl_vertex( scr_clip_x + 8, scr_clip_y + track->h()/ 2 + 5 );
				if ( !k ) {
					fl_end_polygon();
					fl_begin_polygon();
				} else {
					fl_end_loop();
					fl_begin_loop();
				}
				fl_vertex( scr_clip_x + scr_clip_w, scr_clip_y + track->h()/ 2 );
				fl_vertex( scr_clip_x + scr_clip_w - 8, scr_clip_y + track->h()/ 2 - 5 );
				fl_vertex( scr_clip_x + scr_clip_w - 8, scr_clip_y + track->h()/ 2 + 5 );
				if ( !k ) {
					fl_end_polygon();
				} else {
					fl_end_loop();
				}
			}
		}

		fl_pop_clip();
		y_coord += TRACK_SPACING + i->track->h();
	}
	fl_pop_clip();
	fl_overlay_rect( get_screen_position( m_stylusPosition ), parent()->y(), 1, parent()->h() );
}
int64_t TimelineView::get_real_position( int p, float stretchFactor )
{
	return llrint( ( ( float(p - LEFT_TRACK_SPACING - x() ) / SwitchBoard::i()->zoom() ) + m_scrollPosition ) * stretchFactor );
}
int64_t TimelineView::get_screen_position( int64_t p, float stretchFactor )
{
	return llrint( float( p - ( m_scrollPosition * stretchFactor ) ) * SwitchBoard::i()->zoom() / stretchFactor ) + LEFT_TRACK_SPACING + x();

}
void TimelineView::scroll( int64_t position )
{
	m_scrollPosition = position;
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
		if ( !get_track_rect( o->track ).inside( _x, _y ) ) {
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
int get_track_top( Track* track )
{
	int top = TRACK_SPACING;
	for ( track_node* o = g_timeline->getTracks(); o; o = o->next ) {
		if ( o->track == track ) {
			return top;
		}
		top += o->track->h() + TRACK_SPACING;
	}
	return top;	
}
int get_track_top( int track )
{
	int top = TRACK_SPACING;
	int t = 0;
	for ( track_node* o = g_timeline->getTracks(); o; o = o->next ) {
		if ( t == track ) {
			return top;
		}
		top += o->track->h() + TRACK_SPACING;
		t++;
	}
	return top;	
}
Rect TimelineView::get_track_rect( Track* track )
{
	Rect tmp(
			x() + LEFT_TRACK_SPACING,
			get_track_top( track ),
			w() - ( TRACK_SPACING + LEFT_TRACK_SPACING ),
			track->h()
		);
	return tmp;
}
Rect TimelineView::get_clip_rect( Clip* clip, bool clipping )
{
	Rect tmp(
			get_screen_position( clip->position(), clip->track()->stretchFactor() ),
			get_track_top( clip->track() ),
			int( (clip->length()+1) * SwitchBoard::i()->zoom() / clip->track()->stretchFactor() ),
			clip->track()->h()
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
	bool clear = true;
	for ( clip_node* n = m_selectedClips; n; n = n->next ) {
		if ( n->clip == clip ) {
			clear = false;
			break;
		}
	}
	if ( clear ) {
		clear_selection();
	}
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
	/* -- BEGIN CHECK NUMBER OF SELECTED TRACKS -- */
	int track_count = 0;
	int last_track = -1;
	for ( clip_node* p = m_selectedClips; p; p = p->next ) {
		if ( last_track != p->clip->track()->num() ) {
		       track_count++;
		}
 		last_track = p->clip->track()->num();
		if ( track_count > 1 ) {
			break;
		}
	}
	/* -- END CHECK NUMBER OF SELECTED TRACKS -- */
	if ( m_selectedClips && ( new_tr == old_tr || track_count == 1 ) ) {
		if ( track_count == 1 ) {
			cmd = new MoveSelectionCommand( clip, new_tr, new_position, m_selectedClips );
		} else {
			cmd = new MoveSelectionCommand( clip, 0, new_position, m_selectedClips );
		}
	} else {
		clear_selection();
		cmd = new MoveCommand( clip, new_tr, new_position );
		toggle_selection( clip );
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
	updateEffectDisplay();
	setSelectionButtons();
	g_ui->setEffectButtons();
}
void TimelineView::select_clips( int _x1, int _y1, int _x2, int _y2 )
{
	clear_selection();
	if ( _x1 > _x2 ) { int k; k = _x1; _x1 = _x2; _x2 = k; }
	if ( _y1 > _y2 ) { int k; k = _y1; _y1 = _y2; _y2 = k; }
	Rect r;
	for ( track_node* o = g_timeline->getTracks(); o; o = o->next ) {
		r = get_track_rect( o->track );
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
	updateEffectDisplay();
	setSelectionButtons();
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
	updateEffectDisplay();
	setSelectionButtons();
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
void TimelineView::updateTitlesDisplay()
{
	if ( !m_selectedClips || m_selectedClips->next ) {
		g_ui->deactivate_titles();
		return;
	}
	TitleClip* tc = dynamic_cast<TitleClip*>( m_selectedClips->clip );
	if ( !tc ) {
		g_ui->deactivate_titles();
		return;
	}
	g_ui->activate_titles( tc->font(), tc->size(), tc->text(), tc->x(), tc->y(), tc->color() );
}
void TimelineView::titles_text( const char* t )
{
	TitleClip* tc = getTitleClip();
	if ( !tc ) {
		return;
	}
	tc->text( t );
	tc->touch();
	g_videoView->redraw();
}
void TimelineView::titles_x( float x )
{
	TitleClip* tc = getTitleClip();
	if ( !tc ) {
		return;
	}
	tc->x( x );
	tc->touch();
	g_videoView->redraw();
}
void TimelineView::titles_y( float y )
{
	TitleClip* tc = getTitleClip();
	if ( !tc ) {
		return;
	}
	tc->y( y );
	tc->touch();
	g_videoView->redraw();
}
void TimelineView::titles_size( int size )
{
	TitleClip* tc = getTitleClip();
	if ( !tc ) {
		return;
	}
	tc->size( size );
	tc->touch();
	g_videoView->redraw();
}
void TimelineView::titles_font( int font )
{
	TitleClip* tc = getTitleClip();
	if ( !tc ) {
		return;
	}
	tc->font( font );
	tc->touch();
	g_videoView->redraw();
}
void TimelineView::titles_color( Fl_Color color )
{
	TitleClip* tc = getTitleClip();
	if ( !tc ) {
		return;
	}
	tc->color( color );
	tc->touch();
	g_videoView->redraw();
}
TitleClip* TimelineView::getTitleClip()
{
	if ( !m_selectedClips ) {
		return 0;
	}
	if ( m_selectedClips->next ) {
		return 0;
	}
	return dynamic_cast<TitleClip*>( m_selectedClips->clip );
}

void TimelineView::updateEffectDisplay()
{
	updateTitlesDisplay();
	g_ui->effect_browser->clear();
	if ( !m_selectedClips || m_selectedClips->next ) {
		g_ui->m_effectMenu->deactivate();
		return;
	}
	VideoEffectClip* vc = dynamic_cast<VideoEffectClip*>( m_selectedClips->clip );
	if ( !vc ) {
		g_ui->m_effectMenu->deactivate();
		return;
	}
	g_ui->m_effectMenu->activate();
	// Liste füllen
	effect_stack* es = vc->getEffects();
	while ( es ) {
		g_ui->effect_browser->add( es->effect->name(), es->effect );
		es = es->next;
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
	updateEffectDisplay();
	g_ui->setEffectButtons();
	setSelectionButtons();
	redraw();
}
void TimelineView::setSelectionButtons()
{
	if ( m_selectedClips ) {
		g_ui->cut_item->activate();
		g_ui->copy_item->activate();
		g_ui->delete_item->activate();
	} else {
		g_ui->cut_item->deactivate();
		g_ui->copy_item->deactivate();
		g_ui->delete_item->deactivate();
	}
	if ( m_pasteCommand ) {
		g_ui->paste_item->activate();
	} else {
		g_ui->paste_item->deactivate();
	}

}

void TimelineView::moveEffectDown()
{
	//TODO copy and paste from editEffect
	if ( !m_selectedClips ) {
		return;
	}
	if ( m_selectedClips->next ) {
		return;
	}
	VideoEffectClip* vc = dynamic_cast<VideoEffectClip*>( m_selectedClips->clip );
	if ( !vc ) {
		return;
	}
	int c = g_ui->effect_browser->value();
	if ( c == 0 ) {
		return;
	}
	vc->moveEffectDown( c );
	updateEffectDisplay();
	g_ui->effect_browser->value( c + 1 );
	g_ui->setEffectButtons();
	g_videoView->redraw();
}
void TimelineView::moveEffectUp()
{
	//TODO copy and paste from editEffect
	if ( !m_selectedClips ) {
		return;
	}
	if ( m_selectedClips->next ) {
		return;
	}
	VideoEffectClip* vc = dynamic_cast<VideoEffectClip*>( m_selectedClips->clip );
	if ( !vc ) {
		return;
	}
	int c = g_ui->effect_browser->value();
	if ( c == 0 ) {
		return;
	}
	vc->moveEffectUp( c );
	updateEffectDisplay();
	g_ui->effect_browser->value( c - 1 );
	g_ui->setEffectButtons();
	g_videoView->redraw();
}
void TimelineView::addEffect( AbstractEffectFactory* effectFactory )
{
	if ( !m_selectedClips ) {
		return;
	}
	if ( m_selectedClips->next ) {
		return;
	}
	VideoEffectClip* vc = dynamic_cast<VideoEffectClip*>( m_selectedClips->clip );
	if ( !vc ) {
		return;
	}
	vc->pushEffect( effectFactory );
	updateEffectDisplay();
	g_ui->setEffectButtons();
	g_videoView->redraw();
}
void TimelineView::removeEffect()
{
	if ( !m_selectedClips ) {
		return;
	}
	if ( m_selectedClips->next ) {
		return;
	}
	VideoEffectClip* vc = dynamic_cast<VideoEffectClip*>( m_selectedClips->clip );
	if ( !vc ) {
		return;
	}
	int c = g_ui->effect_browser->value();
	if ( c == 0 ) {
		return;
	}
	vc->removeEffect( c );
	updateEffectDisplay();
	g_ui->setEffectButtons();
	g_videoView->redraw();
}
void TimelineView::editEffect()
{
	if ( !m_selectedClips ) {
		return;
	}
	if ( m_selectedClips->next ) {
		return;
	}
	VideoEffectClip* vc = dynamic_cast<VideoEffectClip*>( m_selectedClips->clip );
	if ( !vc ) {
		return;
	}
	int c = g_ui->effect_browser->value();
	if ( c == 0 ) {
		return;
	}
	effect_stack* es = vc->getEffects();
	for ( int i = 1; i < c && es ; i++ ) {
		es = es->next;
	}
/*	Frei0rEffect* fe = dynamic_cast<Frei0rEffect*>( vc->getEffect() );
	for ( int i = 1; i < c && fe ; i++ ) {
		fe = dynamic_cast<Frei0rEffect*>( fe->getReader() );
	}*/
	if ( !es ) {
		return;
	}
	Frei0rEffect* fe = dynamic_cast<Frei0rEffect*>( es->effect );
	Frei0rDialog* dialog = dynamic_cast<Frei0rDialog*>(fe->m_dialog);
	if ( !dialog ) {
		dialog = new Frei0rDialog( fe );
		fe->m_dialog = dialog;
	}
	dialog->show();
/*	Frei0rDialog dialog( fe );
	dialog.show();
	while ( dialog.shown() ) {
		Fl::wait();
	}*/

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
void TimelineView::cut()
{
	if ( !m_selectedClips ) {
		return;
	}
	if ( m_pasteCommand ) {
		delete m_pasteCommand;
		m_pasteCommand = 0;
	}
	m_pasteCommand = new PasteSelectionCommand( m_selectedClips );
	Command* cmd = new RemoveSelectionCommand( m_selectedClips );
	submit( cmd );
	adjustScrollbar();
	clear_selection();
	setSelectionButtons();
}
void TimelineView::copy()
{
	if ( !m_selectedClips ) {
		return;
	}
	if ( m_pasteCommand ) {
		delete m_pasteCommand;
		m_pasteCommand = 0;
	}
	m_pasteCommand = new PasteSelectionCommand( m_selectedClips );
	setSelectionButtons();
}
void TimelineView::paste()
{
	if ( !m_pasteCommand ) {
		return;
	}
	PasteSelectionCommand* cmd = m_pasteCommand;
	m_pasteCommand = new PasteSelectionCommand( cmd );
	cmd->position( m_stylusPosition );
	submit( cmd );
}
void TimelineView::remove()
{
	if ( !m_selectedClips ) {
		return;
	}
	Command* cmd = new RemoveSelectionCommand( m_selectedClips );
	submit( cmd );
	adjustScrollbar();
	clear_selection();
	setSelectionButtons();
}
void TimelineView::help()
{
// Create a HelpDialog and show the Help
	static Fl_Help_Dialog* help = 0;
	if ( !help ) {
		help = new Fl_Help_Dialog;
	}
	struct stat buf;
	if ( stat( INSTALL_PREFIX "/share/doc/openmovieeditor/tutorial.html", &buf ) == 0 ) {
		help->load( INSTALL_PREFIX "/share/doc/openmovieeditor/tutorial.html" );
		help->show();
	} else if ( stat( "../doc/tutorial.html", &buf ) == 0 ) {
		help->load( "../doc/tutorial.html" );
		help->show();
	} else {
		fl_alert( "Help File was not found." );
	}
}

} /* namespace nle */
