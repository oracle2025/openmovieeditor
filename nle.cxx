// generated by Fast Light User Interface Designer (fluid) version 1.0106

#include "nle.h"

inline void NleUI::cb_Open_i(Fl_Menu_*, void*) {
  nle::Project::read_project();
mainWindow->redraw();
}
void NleUI::cb_Open(Fl_Menu_* o, void* v) {
  ((NleUI*)(o->parent()->user_data()))->cb_Open_i(o,v);
}

inline void NleUI::cb_Save_i(Fl_Menu_*, void*) {
  //nle::Renderer a("/home/oracle/t.mov", 368, 240, 25, 48000);
//a.go();
nle::Project::write_project();
}
void NleUI::cb_Save(Fl_Menu_* o, void* v) {
  ((NleUI*)(o->parent()->user_data()))->cb_Save_i(o,v);
}

inline void NleUI::cb_Render_i(Fl_Menu_*, void*) {
  Fl_Group::current( mainWindow );
EncodeDialog dlg;
dlg.show();
while (dlg.shown())
  Fl::wait();
}
void NleUI::cb_Render(Fl_Menu_* o, void* v) {
  ((NleUI*)(o->parent()->user_data()))->cb_Render_i(o,v);
}

Fl_Menu_Item NleUI::menu_[] = {
 {"&File", 0,  0, 0, 64, 0, 0, 14, 56},
 {"New", 0,  0, 0, 0, 0, 0, 14, 56},
 {"Open...", 0,  (Fl_Callback*)NleUI::cb_Open, 0, 128, 0, 0, 14, 56},
 {"Save", 0,  (Fl_Callback*)NleUI::cb_Save, 0, 0, 0, 0, 14, 56},
 {"Save as...", 0,  0, 0, 128, 0, 0, 14, 56},
 {"Render...", 0,  (Fl_Callback*)NleUI::cb_Render, 0, 128, 0, 0, 14, 56},
 {"Quit", 0,  0, 0, 0, 0, 0, 14, 56},
 {0,0,0,0,0,0,0,0,0},
 {0,0,0,0,0,0,0,0,0}
};

inline void NleUI::cb_scaleBar_i(Flmm_Scalebar* o, void*) {
  Flmm_Scalebar *sb = (Flmm_Scalebar *)o;
float width = sb->w();
float slider_size = sb->slider_size_i();
float zoom = width / slider_size;
m_timelineView->scroll( (int64_t)sb->value() );
m_timelineView->zoom( zoom );
}
void NleUI::cb_scaleBar(Flmm_Scalebar* o, void* v) {
  ((NleUI*)(o->parent()->parent()->parent()->parent()->user_data()))->cb_scaleBar_i(o,v);
}

#include <FL/Fl_Pixmap.H>
static const char *idata_tool_positioning[] = {
"32 32 2 1",
" \tc None",
".\tc #000000",
"                                ",
"                                ",
"                                ",
"                                ",
"                                ",
"                                ",
"                                ",
"                                ",
"                                ",
"                                ",
"                                ",
"                                ",
"       .                .       ",
"      ..                ..      ",
"     ...                ...     ",
"    ....                ....    ",
"   ..........................   ",
"    ....                ....    ",
"     ...                ...     ",
"      ..                ..      ",
"       .                .       ",
"                                ",
"                                ",
"                                ",
"                                ",
"                                ",
"                                ",
"                                ",
"                                ",
"                                ",
"                                ",
"                                "
};
static Fl_Pixmap image_tool_positioning(idata_tool_positioning);

static const char *idata_tool_automations[] = {
"32 32 2 1",
" \tc None",
".\tc #000000",
"                                ",
"                                ",
"                                ",
"                                ",
"                .               ",
"               ...              ",
"              .....             ",
"             .......            ",
"            .........           ",
"                .               ",
"                .               ",
"                .               ",
"                .               ",
"                                ",
"                  .             ",
"              .....             ",
"              .....             ",
"              .....             ",
"              .....             ",
"             ......             ",
"                                ",
"                .               ",
"                .               ",
"                .               ",
"                .               ",
"            .........           ",
"             .......            ",
"              .....             ",
"               ...              ",
"                .               ",
"                                ",
"                                "
};
static Fl_Pixmap image_tool_automations(idata_tool_automations);

static const char *idata_trash[] = {
"16 16 4 1",
" \tc None",
".\tc #000000",
"+\tc #B3B3B3",
"@\tc #404040",
"     .......    ",
"   ...+++++...  ",
"  ..+++...+++.. ",
" ..+++.+++.+++..",
" ..+++++++++++..",
"  ..+++++++++.. ",
"  ....+++++.... ",
"  .++.......++. ",
"  .++@++@++@++. ",
"  .++@++@++@++. ",
"  .++@++@++@++. ",
"  .++@++@++@++. ",
"  .++@++@++@++. ",
"  ..+@++@++@+.. ",
"   ...++@++...  ",
"     .......    "
};
static Fl_Pixmap image_trash(idata_trash);

inline void NleUI::cb_PAL_i(Fl_Menu_*, void*) {
  g_fps = 25.0;
std::cout << "PAL" << std::endl;
}
void NleUI::cb_PAL(Fl_Menu_* o, void* v) {
  ((NleUI*)(o->parent()->parent()->parent()->parent()->parent()->parent()->user_data()))->cb_PAL_i(o,v);
}

inline void NleUI::cb_NTSC_i(Fl_Menu_*, void*) {
  g_fps = 29.97;
std::cout << "NTSC" << std::endl;
}
void NleUI::cb_NTSC(Fl_Menu_* o, void* v) {
  ((NleUI*)(o->parent()->parent()->parent()->parent()->parent()->parent()->user_data()))->cb_NTSC_i(o,v);
}

Fl_Menu_Item NleUI::menu_FPS[] = {
 {"PAL 25", 0,  (Fl_Callback*)NleUI::cb_PAL, 0, 0, 0, 0, 14, 56},
 {"NTSC 29.97", 0,  (Fl_Callback*)NleUI::cb_NTSC, 0, 0, 0, 0, 14, 56},
 {0,0,0,0,0,0,0,0,0}
};

inline void NleUI::cb_fileBrowser_i(nle::FileBrowser* o, void*) {
  nle::FileBrowser *fb = (nle::FileBrowser*)o;
fb->load_rel();
}
void NleUI::cb_fileBrowser(nle::FileBrowser* o, void* v) {
  ((NleUI*)(o->parent()->parent()->parent()->parent()->parent()->parent()->user_data()))->cb_fileBrowser_i(o,v);
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

Fl_Menu_Item NleUI::menu_1[] = {
 {"Project 1", 0,  0, 0, 0, 0, 0, 14, 56},
 {"Project 2", 0,  0, 0, 0, 0, 0, 14, 56},
 {0,0,0,0,0,0,0,0,0}
};

NleUI::NleUI() {
  Fl_Double_Window* w;
  { Fl_Double_Window* o = mainWindow = new Fl_Double_Window(515, 455, "MovieEditor");
    w = o;
    o->user_data((void*)(this));
    { Fl_Menu_Bar* o = new Fl_Menu_Bar(0, 0, 345, 25);
      o->menu(menu_);
    }
    { Fl_Tile* o = new Fl_Tile(0, 25, 515, 430);
      { Fl_Group* o = new Fl_Group(0, 275, 515, 180);
        { Fl_Group* o = new Fl_Group(40, 275, 475, 180);
          { nle::Ruler* o = new nle::Ruler(40, 275, 475, 25, "Ruler");
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
          { nle::TimelineView* o = m_timelineView = new nle::TimelineView(40, 300, 475, 135);
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
          { Flmm_Scalebar* o = scaleBar = new Flmm_Scalebar(40, 435, 475, 20);
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
          Fl_Group::current()->resizable(o);
        }
        { Fl_Group* o = new Fl_Group(0, 275, 40, 180);
          o->labelsize(12);
          { Fl_Button* o = new Fl_Button(0, 300, 40, 40);
            o->tooltip("Positioning");
            o->type(102);
            o->value(1);
            o->image(image_tool_positioning);
            o->align(FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
          }
          { Fl_Button* o = new Fl_Button(0, 340, 40, 40);
            o->tooltip("Automations");
            o->type(102);
            o->image(image_tool_automations);
            o->align(FL_ALIGN_BOTTOM|FL_ALIGN_INSIDE);
          }
          { Fl_Box* o = new Fl_Box(0, 380, 40, 35);
            o->box(FL_THIN_UP_BOX);
            Fl_Group::current()->resizable(o);
          }
          { Fl_Box* o = new Fl_Box(0, 275, 40, 25, "Tools");
            o->box(FL_UP_BOX);
            o->labelsize(12);
          }
          { Fl_Box* o = trashCan = new Fl_Box(0, 415, 40, 40);
            o->tooltip("Trash");
            o->box(FL_UP_BOX);
            o->image(image_trash);
          }
          o->end();
        }
        o->end();
      }
      { Fl_Tile* o = new Fl_Tile(0, 25, 515, 250);
        { Fl_Group* o = new Fl_Group(280, 25, 235, 250);
          { nle::VideoViewGL* o = m_videoView = new nle::VideoViewGL(280, 25, 235, 250, "VideoView");
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
          o->end();
        }
        { Fl_Group* o = new Fl_Group(0, 25, 280, 250);
          { Fl_Tabs* o = new Fl_Tabs(0, 50, 280, 185);
            o->box(FL_UP_BOX);
            o->labelcolor(FL_GRAY0);
            { Fl_Group* o = new Fl_Group(0, 75, 280, 160, "Project");
              o->hide();
              { Fl_Value_Input* o = new Fl_Value_Input(55, 85, 100, 25, "Width");
                o->maximum(1024);
                o->step(1);
                o->value(640);
              }
              { Fl_Value_Input* o = new Fl_Value_Input(55, 115, 100, 25, "Height");
                o->maximum(768);
                o->step(1);
                o->value(480);
              }
              { Fl_Box* o = new Fl_Box(180, 207, 80, 13);
                Fl_Group::current()->resizable(o);
              }
              { Fl_Choice* o = new Fl_Choice(55, 145, 100, 25, "FPS");
                o->down_box(FL_BORDER_BOX);
                o->menu(menu_FPS);
              }
              { Fl_Value_Input* o = new Fl_Value_Input(55, 175, 100, 25, "Rate");
                o->minimum(32000);
                o->maximum(48000);
                o->step(1);
                o->value(48000);
                o->deactivate();
              }
              o->end();
              Fl_Group::current()->resizable(o);
            }
            { Fl_Group* o = new Fl_Group(0, 75, 280, 160, "Files");
              { nle::FileBrowser* o = fileBrowser = new nle::FileBrowser(5, 80, 270, 150);
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
            { Fl_Group* o = new Fl_Group(0, 75, 280, 160, "Object Inspector");
              o->hide();
              { Fl_Menu_Button* o = new Fl_Menu_Button(80, 80, 195, 25, "Font");
                o->align(FL_ALIGN_LEFT);
              }
              { Fl_Value_Input* o = new Fl_Value_Input(80, 110, 195, 25, "Font Size");
                o->maximum(40);
                o->step(1);
              }
              { Fl_Text_Editor* o = new Fl_Text_Editor(80, 140, 195, 90, "Text");
                o->align(FL_ALIGN_LEFT);
                Fl_Group::current()->resizable(o);
              }
              o->end();
            }
            o->end();
            Fl_Group::current()->resizable(o);
          }
          { Fl_Input* o = projectNameInput = new Fl_Input(0, 25, 280, 25);
            o->box(FL_FLAT_BOX);
            o->color(FL_BACKGROUND_COLOR);
            o->textfont(1);
          }
          { Fl_Button* o = new Fl_Button(100, 235, 80, 40, "@>");
            o->tooltip("Play");
            o->callback((Fl_Callback*)cb_);
          }
          { Fl_Button* o = new Fl_Button(230, 235, 50, 40, "@>|");
            o->tooltip("Goto End");
          }
          { Fl_Button* o = new Fl_Button(0, 235, 50, 40, "@|<");
            o->tooltip("Goto Start");
          }
          { Fl_Button* o = new Fl_Button(50, 235, 50, 40, "@<|");
            o->tooltip("Skip Frame backwards");
            o->callback((Fl_Callback*)cb_1);
          }
          { Fl_Button* o = new Fl_Button(180, 235, 50, 40, "@|>");
            o->tooltip("Skip Frame forward");
          }
          o->end();
        }
        o->end();
      }
      o->end();
      Fl_Group::current()->resizable(o);
    }
    { Fl_Choice* o = new Fl_Choice(345, 0, 170, 25);
      o->down_box(FL_BORDER_BOX);
      o->deactivate();
      o->menu(menu_1);
    }
    o->end();
  }
}

void NleUI::show( int argc, char **argv ) {
  g_fps = 25.0;
g_scrollBar = scaleBar;
g_trashCan = trashCan;
scaleBar->slider_size_i(300);
mainWindow->show(argc, argv);
projectNameInput->value("Project 1");
}
Flmm_Scalebar* g_scrollBar;

inline void EncodeDialog::cb_Encode_i(Fl_Return_Button* o, void*) {
  //nle::renderMovie();
o->window()->hide();
}
void EncodeDialog::cb_Encode(Fl_Return_Button* o, void* v) {
  ((EncodeDialog*)(o->parent()->user_data()))->cb_Encode_i(o,v);
}

inline void EncodeDialog::cb_Cancel_i(Fl_Button* o, void*) {
  o->window()->hide();
}
void EncodeDialog::cb_Cancel(Fl_Button* o, void* v) {
  ((EncodeDialog*)(o->parent()->user_data()))->cb_Cancel_i(o,v);
}

inline void EncodeDialog::cb_audio_codec_menu_i(Fl_Choice* o, void*) {
  audio_codec = o->menu()[o->value()].user_data();
}
void EncodeDialog::cb_audio_codec_menu(Fl_Choice* o, void* v) {
  ((EncodeDialog*)(o->parent()->user_data()))->cb_audio_codec_menu_i(o,v);
}

inline void EncodeDialog::cb_video_codec_menu_i(Fl_Choice* o, void*) {
  video_codec = o->menu()[o->value()].user_data();
}
void EncodeDialog::cb_video_codec_menu(Fl_Choice* o, void* v) {
  ((EncodeDialog*)(o->parent()->user_data()))->cb_video_codec_menu_i(o,v);
}

Fl_Menu_Item EncodeDialog::menu_Samplerate[] = {
 {"48000", 0,  0, 0, 0, 0, 0, 14, 56},
 {0,0,0,0,0,0,0,0,0}
};

Fl_Menu_Item EncodeDialog::menu_Framerate[] = {
 {"25 (PAL)", 0,  0, 0, 0, 0, 0, 14, 56},
 {0,0,0,0,0,0,0,0,0}
};

Fl_Menu_Item EncodeDialog::menu_Framesize[] = {
 {"720x576", 0,  0, 0, 0, 0, 0, 14, 56},
 {"360x288", 0,  0, 0, 0, 0, 0, 14, 56},
 {"180x144", 0,  0, 0, 0, 0, 0, 14, 56},
 {0,0,0,0,0,0,0,0,0}
};

inline void EncodeDialog::cb_audio_options_i(Fl_Button*, void*) {
  if ( audio_codec ) {
	Fl_Group::current( encodeDialog );
	CodecOptions dlg;
	nle::setCodecInfo( &dlg, audio_codec );
	dlg.codecOptions->show();
	while (dlg.codecOptions->shown())
		Fl::wait();
};
}
void EncodeDialog::cb_audio_options(Fl_Button* o, void* v) {
  ((EncodeDialog*)(o->parent()->user_data()))->cb_audio_options_i(o,v);
}

inline void EncodeDialog::cb_video_options_i(Fl_Button*, void*) {
  if ( video_codec ) {
	Fl_Group::current( encodeDialog );
	CodecOptions dlg;
	nle::setCodecInfo( &dlg, video_codec );
	dlg.codecOptions->show();
	while (dlg.codecOptions->shown())
		Fl::wait();
};
}
void EncodeDialog::cb_video_options(Fl_Button* o, void* v) {
  ((EncodeDialog*)(o->parent()->user_data()))->cb_video_options_i(o,v);
}

EncodeDialog::EncodeDialog() {
  Fl_Double_Window* w;
  { Fl_Double_Window* o = encodeDialog = new Fl_Double_Window(485, 340, "Render");
    w = o;
    o->user_data((void*)(this));
    { Fl_Return_Button* o = new Fl_Return_Button(250, 300, 200, 25, "Encode");
      o->callback((Fl_Callback*)cb_Encode);
      w->hotspot(o);
    }
    { Fl_Button* o = new Fl_Button(35, 300, 200, 25, "Cancel");
      o->callback((Fl_Callback*)cb_Cancel);
    }
    { Fl_Choice* o = audio_codec_menu = new Fl_Choice(145, 220, 205, 25, "Audio Codec");
      o->down_box(FL_BORDER_BOX);
      o->callback((Fl_Callback*)cb_audio_codec_menu);
    }
    { Fl_Choice* o = video_codec_menu = new Fl_Choice(145, 100, 205, 25, "Video Codec");
      o->down_box(FL_BORDER_BOX);
      o->callback((Fl_Callback*)cb_video_codec_menu);
    }
    { Fl_Choice* o = new Fl_Choice(145, 250, 205, 25, "Samplerate");
      o->down_box(FL_BORDER_BOX);
      o->menu(menu_Samplerate);
    }
    { Fl_Choice* o = new Fl_Choice(145, 130, 205, 25, "Framerate");
      o->down_box(FL_BORDER_BOX);
      o->menu(menu_Framerate);
    }
    { Fl_Choice* o = new Fl_Choice(145, 160, 205, 25, "Framesize");
      o->down_box(FL_BORDER_BOX);
      o->menu(menu_Framesize);
    }
    { Fl_Box* o = new Fl_Box(0, 0, 485, 35, "Export");
      o->labelfont(1);
      o->labelsize(16);
    }
    { Fl_Button* o = audio_options = new Fl_Button(355, 220, 75, 25, "Options...");
      o->callback((Fl_Callback*)cb_audio_options);
    }
    { Fl_Button* o = video_options = new Fl_Button(355, 100, 75, 25, "Options...");
      o->callback((Fl_Callback*)cb_video_options);
    }
    new Fl_File_Input(145, 35, 205, 35, "Filename");
    new Fl_Button(355, 45, 75, 25, "File...");
    { Fl_Box* o = new Fl_Box(35, 95, 415, 95, "Video");
      o->box(FL_ENGRAVED_FRAME);
      o->labelfont(1);
      o->align(FL_ALIGN_TOP_LEFT);
    }
    { Fl_Box* o = new Fl_Box(35, 215, 415, 65, "Audio");
      o->box(FL_ENGRAVED_FRAME);
      o->labelfont(1);
      o->align(FL_ALIGN_TOP_LEFT);
    }
    o->set_modal();
    o->end();
  }
}

void EncodeDialog::show() {
  audio_codec = 0;
video_codec = 0;
nle::setAudioCodecMenu( audio_codec_menu );
nle::setVideoCodecMenu( video_codec_menu );
encodeDialog->show();
}

int EncodeDialog::shown() {
  return encodeDialog->shown();
}

ChangesDialog::ChangesDialog() {
  Fl_Double_Window* w;
  { Fl_Double_Window* o = new Fl_Double_Window(410, 80, "Unsaved changes!");
    w = o;
    o->user_data((void*)(this));
    new Fl_Box(5, 10, 385, 25, "Save changes?");
    new Fl_Button(5, 50, 120, 25, "Don\'t Save");
    new Fl_Button(160, 50, 120, 25, "Cancel");
    new Fl_Return_Button(285, 50, 120, 25, "Save");
    o->end();
  }
}
Fl_Box *g_trashCan;
float g_fps;

inline void CodecOptions::cb_parameters_browser_i(Fl_Hold_Browser*, void*) {
  if ( parameters_browser->value() ) {
	nle::setCodecParameter( this, parameters_browser->data( parameters_browser->value() ) );
};
}
void CodecOptions::cb_parameters_browser(Fl_Hold_Browser* o, void* v) {
  ((CodecOptions*)(o->parent()->parent()->user_data()))->cb_parameters_browser_i(o,v);
}

inline void CodecOptions::cb_Cancel1_i(Fl_Button* o, void*) {
  o->window()->hide();
}
void CodecOptions::cb_Cancel1(Fl_Button* o, void* v) {
  ((CodecOptions*)(o->parent()->parent()->user_data()))->cb_Cancel1_i(o,v);
}

CodecOptions::CodecOptions() {
  Fl_Double_Window* w;
  { Fl_Double_Window* o = codecOptions = new Fl_Double_Window(345, 305, "Codec Options");
    w = o;
    o->user_data((void*)(this));
    { Fl_Box* o = codec_label = new Fl_Box(0, 0, 345, 35, "Codec");
      o->labelfont(1);
      o->labelsize(16);
    }
    { Fl_Group* o = new Fl_Group(0, 35, 345, 230);
      { Fl_Hold_Browser* o = parameters_browser = new Fl_Hold_Browser(5, 40, 165, 220);
        o->box(FL_NO_BOX);
        o->color(FL_BACKGROUND2_COLOR);
        o->selection_color(FL_SELECTION_COLOR);
        o->labeltype(FL_NORMAL_LABEL);
        o->labelfont(0);
        o->labelsize(14);
        o->labelcolor(FL_BLACK);
        o->callback((Fl_Callback*)cb_parameters_browser);
        o->align(FL_ALIGN_BOTTOM);
        o->when(FL_WHEN_RELEASE_ALWAYS);
        Fl_Group::current()->resizable(o);
      }
      { Fl_Group* o = new Fl_Group(170, 35, 175, 230);
        { Fl_Value_Input* o = parameter_int_input = new Fl_Value_Input(175, 40, 165, 25);
          o->step(1);
          o->deactivate();
        }
        { Fl_Input* o = parameter_string_input = new Fl_Input(175, 70, 165, 25);
          o->deactivate();
        }
        { Fl_Choice* o = parameter_stringlist_input = new Fl_Choice(175, 100, 165, 25);
          o->down_box(FL_BORDER_BOX);
          o->deactivate();
        }
        { Fl_Box* o = new Fl_Box(230, 175, 25, 25);
          Fl_Group::current()->resizable(o);
        }
        o->end();
      }
      o->end();
      Fl_Group::current()->resizable(o);
    }
    { Fl_Group* o = new Fl_Group(0, 265, 345, 40);
      { Fl_Button* o = new Fl_Button(20, 275, 140, 25, "Cancel");
        o->callback((Fl_Callback*)cb_Cancel1);
      }
      { Fl_Return_Button* o = new Fl_Return_Button(185, 275, 140, 25, "Ok");
        w->hotspot(o);
      }
      o->end();
    }
    o->set_modal();
    o->end();
  }
}
