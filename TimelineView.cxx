#include <iostream>
#include <FL/Fl.H>
#include <FL/Fl_Widget.H>
#include <FL/Fl_Window.H>
#include <FL/fl_draw.H>
#include "TimelineView.h"
#include "Timeline.h"
#include "SwitchBoard.h"
#include "Draw.h"
#include "VideoClip.h"
#include "VideoTrack.h"
#include "MoveDragHandler.h"
#include "TrimDragHandler.h"
#include "Rect.h"
using namespace std;

namespace nle
{

bool USING_AUDIO = 0;
	

TimelineView::TimelineView( int x, int y, int w, int h, const char *label )
	: Fl_Widget( x, y, w, h, label )//, Flu_DND("DND_Timeline")
{
	m_dragHandler = NULL;
	m_timeline = new Timeline();
	m_timeline->add_video( 0, 30, "/home/oracle/tmp/test3.mov" );
	m_timeline->add_video( 1, 100, "/home/oracle/tmp/test3.mov" );
	m_timeline->add_audio( 2, 0, "/home/oracle/tmp/test3.mov" );
	m_scrollPosition = 0;
	SwitchBoard::i()->timelineView(this);
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
		fl_push_matrix();
		fl_translate( x() + TRACK_SPACING,
				y() + TRACK_SPACING + cnt * (TRACK_HEIGHT + TRACK_SPACING) );
		Draw::box( 0, 0, w() - 2 * TRACK_SPACING, TRACK_HEIGHT, FL_DARK2 );
		fl_push_clip( x() + TRACK_SPACING,
				y() + TRACK_SPACING + cnt * (TRACK_HEIGHT + TRACK_SPACING),
				w() - 2 * TRACK_SPACING,
				TRACK_HEIGHT );
		USING_AUDIO = (*i)->type() == TRACK_TYPE_AUDIO;
		fl_scale( SwitchBoard::i()->zoom() / (*i)->stretchFactor(), 1.0 );
		fl_translate( - (m_scrollPosition * (*i)->stretchFactor()), 0.0 );
		//if ( (*i)->type() == TRACK_TYPE_VIDEO ) {
			for ( std::list< Clip* >::iterator j = vcl->begin(); j != vcl->end(); j++ ) {
				p = float( (*j)->position() );
				l = float( (*j)->length() );
				Draw::box( p, 0, l, TRACK_HEIGHT, FL_DARK3 );
				Draw::triangle( p, TRACK_HEIGHT/2, true );
				Draw::triangle( p + l, TRACK_HEIGHT/2, false );
		//	}
		}/* else {
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
	/*
	for i in VideoTracks:
		draw VideoTrackBackground
			for j in i(VideoTrack):
				drawClip i
	 */
}
void TimelineView::add_video( int track, int y, const char* filename )
{
	m_timeline->add_video( track, get_real_position( y ), filename );
}
int64_t TimelineView::get_real_position( int p )
{
	return int64_t( float(p - TRACK_SPACING) * SwitchBoard::i()->zoom() ) + m_scrollPosition;
}
int TimelineView::get_screen_position( int64_t p, float stretchFactor )
{
	return int ( float( p - ( m_scrollPosition * stretchFactor ) ) / SwitchBoard::i()->zoom() / stretchFactor ) + TRACK_SPACING;

}
void TimelineView::scroll( int64_t position )
{
	m_scrollPosition = position;
	//redraw();
}
void TimelineView::zoom( float zoom )
{
	SwitchBoard::i()->zoom( zoom );
	redraw();
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
			get_screen_position( int( clip->position() / clip->track()->stretchFactor() ) ),
			int( TRACK_SPACING + (TRACK_SPACING + TRACK_HEIGHT) * clip->track()->num() ),
			int( clip->length() / SwitchBoard::i()->zoom() / clip->track()->stretchFactor() ),
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
void TimelineView::move_clip( Clip* clip, int _x, int _y, int offset )
{
	Track *new_tr = get_track( _x, _y );
	Track *old_tr = clip->track();
	if (!new_tr || new_tr->type() != old_tr->type() )
		return;
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
	window()->make_current();
	fl_overlay_rect( get_screen_position(position), y(), 1, h() );
}

} /* namespace nle */
