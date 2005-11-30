// generated by Fast Light User Interface Designer (fluid) version 1.0106

#include "ProgressDialog.h"

inline void ProgressDialog::cb_cancel_button_i(Fl_Button*, void*) {
  cancel = true;
cancel_button->deactivate();
}
void ProgressDialog::cb_cancel_button(Fl_Button* o, void* v) {
  ((ProgressDialog*)(o->parent()->user_data()))->cb_cancel_button_i(o,v);
}

ProgressDialog::ProgressDialog( const char* title ) {
  Fl_Double_Window* w;
  { Fl_Double_Window* o = progressDialog = new Fl_Double_Window(335, 145, "Progress Dialog");
    w = o;
    o->user_data((void*)(this));
    { Fl_Button* o = cancel_button = new Fl_Button(240, 115, 90, 25, "Cancel");
      o->callback((Fl_Callback*)cb_cancel_button);
      w->hotspot(o);
    }
    { Fl_Progress* o = progress_bar = new Fl_Progress(40, 75, 255, 25);
      o->selection_color((Fl_Color)5);
    }
    { Fl_Box* o = title_box = new Fl_Box(0, 5, 335, 30, "Loading Project...");
      o->labelfont(1);
      o->labelsize(16);
    }
    { Fl_Output* o = new Fl_Output(10, 40, 315, 25);
      o->box(FL_FLAT_BOX);
      o->color(FL_BACKGROUND_COLOR);
    }
    o->set_modal();
    o->end();
  }
  progress_bar->minimum( 0.0 );
progress_bar->maximum( 100.0 );
progress_bar->value( 0.0 );
title_box->label( title );
}

bool ProgressDialog::progress( int percent ) {
  progress_bar->value( percent );
Fl::check();
return cancel;
}

void ProgressDialog::start() {
  cancel = false;
progressDialog->show();
}

void ProgressDialog::end() {
  progressDialog->hide();
}

ProgressDialog::~ProgressDialog() {
  delete progressDialog;
}
