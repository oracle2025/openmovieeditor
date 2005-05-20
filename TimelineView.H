#ifndef _TIMELINE_VIEW_H_
#define _TIMELINE_VIEW_H_

#include <FL/Fl.H>
#include <FL/Fl_Widget.H>
#include <stdint.h>
#include "Rect.H"
#include "NextFrameListener.H"

namespace nle
{
#define TRACK_HEIGHT 30
#define TRACK_SPACING 5


class Track;
class Clip;
class Timeline;
class DragHandler;
	
class TimelineView : public Fl_Widget
{
	public:
		TimelineView( int x, int y, int w, int h, const char *label = 0 );
		~TimelineView();
		int handle( int event );
		void resize(int x, int y, int w, int h);
		void add_video( int track, int y, const char* filename ); /*remove me!*/
		void scroll( int64_t position );
		Track* get_track( int _x, int _y);
		Clip* get_clip( int _x, int _y );
		Rect get_track_rect( Track* track );
		Rect get_clip_rect( Clip* clip, bool clipping = true );
		void move_clip( Clip* clip, int _x, int _y, int offset );
		void trim_clip( Clip* clip, int _x, bool trimRight );
		void move_cursor( int64_t position );
		void stylus( long stylus_pos );
		long stylus();
		void zoom( float zoom );
	protected:
		void draw();
	private:
		int64_t get_real_position( int p );
		int get_screen_position( int64_t p, float stretchFactor = 1.0 );
		
		Timeline *m_timeline;
		DragHandler *m_dragHandler;
		int64_t m_scrollPosition;
		int64_t m_stylusPosition;
};

} /* namespace nle */

#endif /* _TIMELINE_VIEW_H_ */
