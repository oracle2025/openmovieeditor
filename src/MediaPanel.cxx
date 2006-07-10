/*  MediaPanel.cxx
 *
 *  Copyright (C) 2006 Richard Spindler <richard.spindler AT gmail.com>
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

#include "MediaPanel.H"

namespace nle
{

	inline void MediaPanel::cb_browser_i(nle::FolderBrowser*, void*) {
	  browser->click();
	folderDisplay->value( browser->folder() );
	files->load( browser->folder() );
	}
	void MediaPanel::cb_browser(nle::FolderBrowser* o, void* v) {
	  ((MediaPanel*)(o->parent()->parent()->user_data()))->cb_browser_i(o,v);
	}

	inline void MediaPanel::cb_files_i(nle::MediaBrowser* o, void*) {
	  o->click();
	}
	void MediaPanel::cb_files(nle::MediaBrowser* o, void* v) {
	  ((MediaPanel*)(o->parent()->parent()->user_data()))->cb_files_i(o,v);
	}

	inline void MediaPanel::cb_8_i(Fl_Button*, void*) {
	  browser->up();
	folderDisplay->value( browser->folder() );
	files->load( browser->folder() );
	}
	void MediaPanel::cb_8(Fl_Button* o, void* v) {
	  ((MediaPanel*)(o->parent()->parent()->user_data()))->cb_8_i(o,v);
	}

	inline void MediaPanel::cb_folderDisplay_i(Fl_File_Input* o, void*) {
	  browser->load( o->value() );
	}
	void MediaPanel::cb_folderDisplay(Fl_File_Input* o, void* v) {
	  ((MediaPanel*)(o->parent()->parent()->user_data()))->cb_folderDisplay_i(o,v);
	}

	inline void MediaPanel::cb_81_i(Fl_Button*, void*) {
	  browser->size( bt->w(), bt->h() / 2 );
	files->resize(  bt->x(), bt->y() + bt->h() / 2, bt->w(), bt->h() - bt->h() / 2 );
	bt->remove( browser );
	bt->remove( files );
	bt->add( browser );
	bt->add( files );
	bt->redraw();
	}
	void MediaPanel::cb_81(Fl_Button* o, void* v) {
	  ((MediaPanel*)(o->parent()->parent()->user_data()))->cb_81_i(o,v);
	}

	inline void MediaPanel::cb__i(Fl_Button*, void*) {
	  browser->resize( bt->x(), bt->y(), bt->w() / 2, bt->h() );
	files->resize( bt->x() + bt->w() / 2, bt->y(), bt->w() / 2, bt->h() );
	bt->remove( browser );
	bt->remove( files );
	bt->add( browser );
	bt->add( files );
	bt->redraw();
	}
	void MediaPanel::cb_(Fl_Button* o, void* v) {
	  ((MediaPanel*)(o->parent()->parent()->user_data()))->cb__i(o,v);
	}

	MediaPanel::~MediaPanel()
	{
	}

	MediaPanel::MediaPanel( int x, int y, int w, int h, const char *l )
		: Fl_Group( 0, 0, 360, 245, l )
	{
	  //Fl_Double_Window* w;
	  { //Fl_Double_Window* o = mainWindow = new Fl_Double_Window(360, 245);
	    Fl_Group* o = this;
	    o->user_data((void*)(this));
	    { Fl_Tile* o = bt = new Fl_Tile(0, 35, 360, 210);
	      { nle::FolderBrowser* o = browser = new nle::FolderBrowser(0, 35, 190, 210);
		o->box(FL_NO_BOX);
		o->color(FL_BACKGROUND2_COLOR);
		o->selection_color(FL_SELECTION_COLOR);
		o->labeltype(FL_NORMAL_LABEL);
		o->labelfont(0);
		o->labelsize(14);
		o->labelcolor(FL_BLACK);
		o->callback((Fl_Callback*)cb_browser);
		o->align(FL_ALIGN_BOTTOM);
		o->when(FL_WHEN_RELEASE_ALWAYS);
	      }
	      { nle::MediaBrowser* o = files = new nle::MediaBrowser(190, 35, 170, 210);
		o->box(FL_NO_BOX);
		o->color(FL_BACKGROUND2_COLOR);
		o->selection_color(FL_SELECTION_COLOR);
		o->labeltype(FL_NORMAL_LABEL);
		o->labelfont(0);
		o->labelsize(14);
		o->labelcolor(FL_BLACK);
		o->callback((Fl_Callback*)cb_files);
		o->align(FL_ALIGN_BOTTOM);
		o->when(FL_WHEN_RELEASE_ALWAYS);
	      }
	      o->end();
	      Fl_Group::current()->resizable(o);
	    }
	    { Fl_Group* o = new Fl_Group(0, 0, 360, 35);
	      { Fl_Button* o = new Fl_Button(0, 10, 25, 25, "@8>");
		o->callback((Fl_Callback*)cb_8);
	      }
	      { Fl_File_Input* o = folderDisplay = new Fl_File_Input(25, 0, 285, 35);
		o->callback((Fl_Callback*)cb_folderDisplay);
		Fl_Group::current()->resizable(o);
	      }
	      { Fl_Button* o = new Fl_Button(335, 10, 25, 25, "@8||");
		o->callback((Fl_Callback*)cb_81);
	      }
	      { Fl_Button* o = new Fl_Button(310, 10, 25, 25, "@||");
		o->callback((Fl_Callback*)cb_);
	      }
	      o->end();
	    }
	    o->end();
	  }
	  resize( x, y, w, h );
	}


} /* namespace nle */
