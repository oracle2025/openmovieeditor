// generated by Fast Light User Interface Designer (fluid) version 1.0106

#include "nle.h"

inline void NleUI::cb_Save_i(Fl_Menu_*, void*) {
  nle::Project::write_project();
nle::Renderer a("/home/oracle/t.mov");
a.go();
}
void NleUI::cb_Save(Fl_Menu_* o, void* v) {
  ((NleUI*)(o->parent()->user_data()))->cb_Save_i(o,v);
}

Fl_Menu_Item NleUI::menu_[] = {
 {"&File", 0,  0, 0, 64, 0, 0, 14, 56},
 {"New", 0,  0, 0, 0, 0, 0, 14, 56},
 {"Open...", 0,  0, 0, 128, 0, 0, 14, 56},
 {"Save", 0,  (Fl_Callback*)NleUI::cb_Save, 0, 0, 0, 0, 14, 56},
 {"Save as...", 0,  0, 0, 128, 0, 0, 14, 56},
 {"Quit", 0,  0, 0, 0, 0, 0, 14, 56},
 {0,0,0,0,0,0,0,0,0},
 {0,0,0,0,0,0,0,0,0}
};

inline void NleUI::cb_fileBrowser_i(nle::FileBrowser* o, void*) {
  nle::FileBrowser *fb = (nle::FileBrowser*)o;
fb->load_rel();
}
void NleUI::cb_fileBrowser(nle::FileBrowser* o, void* v) {
  ((NleUI*)(o->parent()->parent()->parent()->parent()->parent()->user_data()))->cb_fileBrowser_i(o,v);
}

inline void NleUI::cb__i(Fl_Button*, void*) {
  m_videoView->play();
}
void NleUI::cb_(Fl_Button* o, void* v) {
  ((NleUI*)(o->parent()->parent()->parent()->parent()->user_data()))->cb__i(o,v);
}

inline void NleUI::cb_1_i(Fl_Button*, void*) {
  m_videoView->stop();
}
void NleUI::cb_1(Fl_Button* o, void* v) {
  ((NleUI*)(o->parent()->parent()->parent()->parent()->user_data()))->cb_1_i(o,v);
}

inline void NleUI::cb_scaleBar_i(Flmm_Scalebar* o, void*) {
  Flmm_Scalebar *sb = (Flmm_Scalebar *)o;
float width = sb->w();
float slider_size = sb->slider_size_i();
float zoom = width / slider_size;
m_timelineView->scroll( (int64_t)sb->value() );
m_timelineView->zoom( zoom );
}
void NleUI::cb_scaleBar(Flmm_Scalebar* o, void* v) {
  ((NleUI*)(o->parent()->user_data()))->cb_scaleBar_i(o,v);
}

NleUI::NleUI() {
  Fl_Double_Window* w;
  { Fl_Double_Window* o = mainWindow = new Fl_Double_Window(475, 380, "MovieEditor");
    w = o;
    o->user_data((void*)(this));
    { Fl_Menu_Bar* o = new Fl_Menu_Bar(0, 0, 475, 25);
      o->menu(menu_);
    }
    { Fl_Tile* o = new Fl_Tile(0, 25, 475, 335);
      { Fl_Group* o = new Fl_Group(0, 25, 475, 175);
        { Fl_Tabs* o = new Fl_Tabs(0, 25, 235, 175);
          o->box(FL_UP_BOX);
          o->labelcolor(FL_GRAY0);
          { Fl_Box* o = new Fl_Box(0, 50, 235, 150, "Object Inspector");
            o->hide();
          }
          { Fl_Group* o = new Fl_Group(0, 50, 235, 150, "Project");
            o->hide();
            { Fl_Value_Input* o = new Fl_Value_Input(55, 60, 55, 25, "Width");
              o->maximum(1024);
              o->step(1);
              o->value(640);
            }
            { Fl_Value_Input* o = new Fl_Value_Input(55, 90, 55, 25, "Height");
              o->maximum(768);
              o->step(1);
              o->value(480);
            }
            { Fl_Box* o = new Fl_Box(135, 145, 55, 25);
              Fl_Group::current()->resizable(o);
            }
            o->end();
            Fl_Group::current()->resizable(o);
          }
          { Fl_Group* o = new Fl_Group(0, 50, 235, 150, "Files");
            { nle::FileBrowser* o = fileBrowser = new nle::FileBrowser(5, 55, 225, 140);
              o->box(FL_NO_BOX);
              o->color(FL_BACKGROUND2_COLOR);
              o->selection_color(FL_SELECTION_COLOR);
              o->labeltype(FL_NORMAL_LABEL);
              o->labelfont(0);
              o->labelsize(14);
              o->labelcolor(FL_BLACK);
              o->callback((Fl_Callback*)cb_fileBrowser);
              o->align(FL_ALIGN_BOTTOM);
              o->when(FL_WHEN_RELEASE_ALWAYS);
              Fl_Group::current()->resizable(o);
            }
            o->end();
          }
          o->end();
        }
        { Fl_Group* o = new Fl_Group(235, 25, 240, 175);
          { nle::VideoViewGL* o = m_videoView = new nle::VideoViewGL(235, 25, 240, 150, "VideoView");
            o->box(FL_DOWN_BOX);
            o->color(FL_BACKGROUND_COLOR);
            o->selection_color(FL_BACKGROUND_COLOR);
            o->labeltype(FL_NORMAL_LABEL);
            o->labelfont(0);
            o->labelsize(14);
            o->labelcolor(FL_GRAY0);
            o->align(FL_ALIGN_CENTER);
            o->when(FL_WHEN_RELEASE);
            Fl_Group::current()->resizable(o);
          }
          { Fl_Button* o = new Fl_Button(355, 175, 60, 25, "@>");
            o->callback((Fl_Callback*)cb_);
          }
          new Fl_Button(415, 175, 60, 25, "@>|");
          new Fl_Button(235, 175, 60, 25, "@|<");
          { Fl_Button* o = new Fl_Button(295, 175, 60, 25, "@||");
            o->callback((Fl_Callback*)cb_1);
          }
          o->end();
        }
        o->end();
      }
      { Fl_Group* o = new Fl_Group(0, 200, 475, 160);
        { nle::Ruler* o = new nle::Ruler(0, 200, 475, 25, "Ruler");
          o->box(FL_UP_BOX);
          o->color(FL_BACKGROUND_COLOR);
          o->selection_color(FL_BACKGROUND_COLOR);
          o->labeltype(FL_NORMAL_LABEL);
          o->labelfont(0);
          o->labelsize(14);
          o->labelcolor(FL_BLACK);
          o->align(FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
          o->when(FL_WHEN_RELEASE);
        }
        { nle::TimelineView* o = m_timelineView = new nle::TimelineView(0, 225, 475, 135);
          o->box(FL_NO_BOX);
          o->color(FL_BACKGROUND_COLOR);
          o->selection_color(FL_BACKGROUND_COLOR);
          o->labeltype(FL_NORMAL_LABEL);
          o->labelfont(0);
          o->labelsize(14);
          o->labelcolor(FL_BLACK);
          o->align(FL_ALIGN_CENTER);
          o->when(2);
          Fl_Group::current()->resizable(o);
        }
        o->end();
      }
      o->end();
      Fl_Group::current()->resizable(o);
    }
    { Flmm_Scalebar* o = scaleBar = new Flmm_Scalebar(0, 360, 475, 20);
      o->type(1);
      o->box(FL_FLAT_BOX);
      o->color(FL_DARK2);
      o->selection_color(FL_BACKGROUND_COLOR);
      o->labeltype(FL_NORMAL_LABEL);
      o->labelfont(0);
      o->labelsize(14);
      o->labelcolor(FL_BLACK);
      o->maximum(1024);
      o->slider_size(0.40404);
      o->callback((Fl_Callback*)cb_scaleBar);
      o->align(FL_ALIGN_BOTTOM);
      o->when(FL_WHEN_CHANGED);
    }
    o->end();
  }
}

void NleUI::show( int argc, char **argv ) {
  g_scrollBar = scaleBar;
scaleBar->slider_size_i(300);
mainWindow->show(argc, argv);
}
Flmm_Scalebar* g_scrollBar;

Fl_Menu_Item EncodeDialog::menu_Audio[] = {
 {"aa", 0,  0, 0, 0, 0, 0, 14, 56},
 {0,0,0,0,0,0,0,0,0}
};

EncodeDialog::EncodeDialog() {
  Fl_Double_Window* w;
  { Fl_Double_Window* o = encodeDialog = new Fl_Double_Window(325, 145);
    w = o;
    o->user_data((void*)(this));
    { Fl_Menu_Button* o = new Fl_Menu_Button(110, 25, 200, 25, "Audio Codec");
      o->align(FL_ALIGN_LEFT);
      o->menu(menu_Audio);
    }
    { Fl_Menu_Button* o = new Fl_Menu_Button(110, 55, 200, 25, "Video Codec");
      o->align(FL_ALIGN_LEFT);
    }
    new Fl_Return_Button(170, 110, 140, 25, "Encode");
    new Fl_Button(15, 110, 140, 25, "Cancel");
    o->end();
  }
}
