// Custom scroll that tells children to follow scroll's width when resized

#include <FL/Fl.H>
#include <FL/Fl_Scroll.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Pack.H>
#include "FilterClip.H"
#include "FilterScroll.H"
#include "FilterItemWidget.H"
#include "IEffectWidget.H"
#include "FilterBase.H"

#include <iostream>

namespace nle
{

const int defaultHeight = 20;

FilterScroll::FilterScroll(int X, int Y, int W, int H, const char* L) : Fl_Scroll(X,Y,W,H,L) {
	nchild = 0;
	nchildheight = 0;
	m_pack = 0;
}
void FilterScroll::resize(int X, int Y, int W, int H) {
	// Tell children to resize to our new width
	if ( m_pack ) {
		m_pack->resize(m_pack->x(), m_pack->y(), W-20, m_pack->h() );
	}
	/*
	for ( int t=0; t<nchild; t++ ) {
		Fl_Widget *w = child(t);
		w->resize(w->x(), w->y(), W-20, w->h());    // W-20: leave room for scrollbar
	}
	*/
	// Tell scroll children changed in size
	init_sizes();
	Fl_Scroll::resize(X,Y,W,H);
}

// Append new scrollitem to bottom
//     Note: An Fl_Pack would be a good way to do this, too
//
void FilterScroll::AddItem(FilterBase* filter) {
	int X = x() + 1,
	    Y = y() - yposition() + (nchildheight) + 1,
	    W = w() - 20,                           // -20: compensate for vscroll bar
	    H = defaultHeight;
	//add(new ScrollItem(X,Y,W,H));
	
	Fl_Pack* p = new Fl_Pack(X,Y,430,H);
	FilterItemWidget *w = new FilterItemWidget(X,Y,430,H);
	w->setFilter(filter);
	IEffectWidget* f = filter->widget();
	nchildheight += defaultHeight;
	w->setFilterWidget(f);
	if ( f ) {
		f->resize(X, Y+H, 430, f->h() );
		nchildheight += f->h();
	}
	//Fl_Slider* s = new Fl_Slider(X,Y+H,430,20);
	//s->type(5);
	p->end();
	p->resize(X,Y,W,H);
	m_pack->add(p);
	redraw();
	nchild++;
}
void FilterScroll::setClip( FilterClip* clip )
{
	nchild=0;
	nchildheight=0;
	clear();
	m_pack = new Fl_Pack( x(), y(), w()-20, h() );
	m_pack->spacing(0);
	add(m_pack);
	filter_stack* es = clip->getFilters();
	while ( es ) {
		AddItem( es->filter );
		es = es->next;
	}
}

} /* namespace nle */

