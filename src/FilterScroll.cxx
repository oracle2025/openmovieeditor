// Custom scroll that tells children to follow scroll's width when resized

#include <FL/Fl.H>
#include <FL/Fl_Scroll.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Box.H>
#include "FilterClip.H"
#include "FilterScroll.H"
#include "FilterItemWidget.H"

namespace nle
{

const int defaultHeight = 20;

FilterScroll::FilterScroll(int X, int Y, int W, int H, const char* L) : Fl_Scroll(X,Y,W,H,L) {
	nchild = 0;
}
void FilterScroll::resize(int X, int Y, int W, int H) {
	// Tell children to resize to our new width
	for ( int t=0; t<nchild; t++ ) {
		Fl_Widget *w = child(t);
		w->resize(w->x(), w->y(), W-20, w->h());    // W-20: leave room for scrollbar
	}
	// Tell scroll children changed in size
	init_sizes();
	Fl_Scroll::resize(X,Y,W,H);
}

// Append new scrollitem to bottom
//     Note: An Fl_Pack would be a good way to do this, too
//
void FilterScroll::AddItem(FilterBase* filter) {
	int X = x() + 1,
	    Y = y() - yposition() + (nchild*defaultHeight) + 1,
	    W = w() - 20,                           // -20: compensate for vscroll bar
	    H = defaultHeight;
	//add(new ScrollItem(X,Y,W,H));
	FilterItemWidget *w = new FilterItemWidget(X,Y,430,H);
	w->setFilter(filter);
	add(w);
	w->resize(X,Y,W,H);
	redraw();
	nchild++;
}
void FilterScroll::setClip( FilterClip* clip )
{
	nchild=0;
	clear();
	filter_stack* es = clip->getFilters();
	while ( es ) {
		AddItem( es->filter );
		es = es->next;
	}
}

} /* namespace nle */

