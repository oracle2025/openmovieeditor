#ifndef _C_WIDGET_H_
#define _C_WIDGET_H_

#include <FL/Fl_Widget.H>

#include <FLU/Flu_DND.h>

class Cwidget : public Fl_Widget, public Flu_DND
{
	public:
		Cwidget( int x, int y, int w, int h, const char *l = 0 );
		int handle( int event );
		void on_dnd_drop( const Flu_DND_Event *e );
		void on_dnd_release();
		void on_dnd_enter();
		void on_dnd_leave();
		void draw();
};

#endif /* _C_WIDGET_H_ */

