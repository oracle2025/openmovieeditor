// generated by Fast Light User Interface Designer (fluid) version 1.0107

#include "FilterItemWidget.H"
#include "FilterBase.H"
#include "FilterClip.H"
#include "IEffectWidget.H"
#include "VideoViewGL.H"
#include "TimelineView.H"
#include "FilterRemoveCommand.H"
#include "DocManager.H"
#include "DragBox.H"
#include <cassert>
#include "MasterEffectClip.H"

namespace nle
{

void FilterItemWidget::cb_Expand_i( Fl_Button* o )
{
	if ( !m_filter_widget ) {
		return;
	}
	if ( m_filter_widget->visible() ) {
		m_filter_widget->hide();
		o->label( "@-1>" );
		m_filter->expanded( false );
	} else {
		m_filter_widget->show();
		o->label( "@-12>" );
		m_filter->expanded( true );
	}
	Fl_Group* g = m_filter_widget->parent()->parent()->parent();
	g->init_sizes();
	//g->resize( g->x(), g->y(), g->w(), g->h() );
	g->redraw();
}
void FilterItemWidget::cb_Expand( Fl_Button* o, void* v )
{
	FilterItemWidget* fiw = (FilterItemWidget*)v;
	fiw->cb_Expand_i( o );
}
void FilterItemWidget::cb_Up_i( Fl_Button* o )
{
	assert( m_clip );
	m_clip->moveFilterUp( m_filter );
	parent()->parent()->parent()->do_callback();
}
void FilterItemWidget::cb_Up( Fl_Button* o, void* v )
{
	FilterItemWidget* fiw = (FilterItemWidget*)v;
	fiw->cb_Up_i( o );
}
void FilterItemWidget::cb_Down_i( Fl_Button* o )
{
	assert( m_clip );
	m_clip->moveFilterDown( m_filter );
	parent()->parent()->parent()->do_callback();
}
void FilterItemWidget::cb_Down( Fl_Button* o, void* v )
{
	FilterItemWidget* fiw = (FilterItemWidget*)v;
	fiw->cb_Down_i( o );
}
void FilterItemWidget::cb_Remove_i( Fl_Button* o )
{
	assert( m_clip );
	int c = 1;
	filter_stack* es = m_clip->getFilters();
	while ( es && es->filter != m_filter ) {
		c++;
		es = es->next;
	}
	if ( dynamic_cast<MasterEffectClip*>(m_clip) ) {
		m_clip->removeFilter( c );
	} else {
		Command* cmd = new FilterRemoveCommand( m_clip, c );
		submit( cmd );
	}
	//TODO: Don't do this for the Master Effect List
	parent()->parent()->parent()->do_callback();
}
void FilterItemWidget::cb_Remove( Fl_Button* o, void* v )
{
	FilterItemWidget* fiw = (FilterItemWidget*)v;
	fiw->cb_Remove_i( o );
}
void FilterItemWidget::cb_Bypass_i( Fl_Check_Button* o )
{
	m_filter->bypass( !o->value() );
	g_videoView->redraw();
}
void FilterItemWidget::cb_Bypass( Fl_Check_Button* o, void* v )
{
	FilterItemWidget* fiw = (FilterItemWidget*)v;
	fiw->cb_Bypass_i( o );
}
void FilterItemWidget::cb_Drag_i( DragBox* o )
{
	//SET FilterData in Timeline
	g_timelineView->m_draggedFilter = m_filter;
	Fl::copy( m_filter->identifier(), strlen(m_filter->identifier()) + 1, 0 );
	Fl::dnd();
	g_timelineView->m_draggedFilter = 0;
}
void FilterItemWidget::cb_Drag( DragBox* o, void* v )
{
	FilterItemWidget* fiw = (FilterItemWidget*)v;
	fiw->cb_Drag_i( o );
}


FilterItemWidget::FilterItemWidget(int X, int Y, int W, int H, const char *L)
  : Fl_Group(X, Y, W, H, L) {
  FilterItemWidget *o = this;
  //o->box(FL_ENGRAVED_BOX);
  { Fl_Button* o = m_expand_button = new Fl_Button(X+0, Y+0, 20, 20, "@-12>");
	o->callback((Fl_Callback*)cb_Expand, this);
	o->box(FL_NO_BOX);
  }
{ Fl_Check_Button* o = m_bypass_button = new Fl_Check_Button(X+20, Y+0, 20, 20);
  	o->callback((Fl_Callback*)cb_Bypass, this);
  o->down_box(FL_DOWN_BOX);
  o->value(1);
}
{ DragBox* o = m_filter_name = new DragBox(X+40, Y+0, 330, 20, "Filter Name");
	o->callback((Fl_Callback*)cb_Drag, this);
  o->labelsize(12);
  o->labelfont(FL_HELVETICA_BOLD);
  o->align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
  Fl_Group::current()->resizable(o);
}
{ Fl_Button* o = new Fl_Button(X+370, Y+1, 18, 18, "@-12>");
  	o->callback((Fl_Callback*)cb_Down, this);
}
  { Fl_Button* o = new Fl_Button(X+390, Y+1, 18, 18, "@-18>");	
  	o->callback((Fl_Callback*)cb_Up, this);
  }

{ Fl_Button* o = new Fl_Button(X+410, Y+1, 18, 18, "@-11+");
	o->callback((Fl_Callback*)cb_Remove, this);
}
end();
m_filter_widget = 0;
m_clip = 0;
m_filter = 0;
}
void FilterItemWidget::setFilter( FilterBase* filter )
{
	m_filter_name->label( filter->name() );
	m_filter = filter;
	m_bypass_button->value( !m_filter->bypass() );
}
void FilterItemWidget::setClip( FilterClip* clip )
{
	m_clip = clip;
}
void FilterItemWidget::setFilterWidget( IEffectWidget* filter_widget )
{
	m_filter_widget = filter_widget;
	if ( !m_filter->expanded() ) {
		m_filter_widget->hide();
		m_expand_button->label( "@>" );
	}
}

} /* namespace nle */
