// generated by Fast Light User Interface Designer (fluid) version 1.0107

#ifndef nle_h
#define nle_h
#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include "ProgressDialog/ProgressDialog.h"
#include "Renderer.H"
#include "Codecs.H"
#include "DocManager.H"
#include "Timeline.H"
#include "VideoTrack.H"
#include "AudioTrack.H"
#include <FL/Fl_Tile.H>
#include <FL/Fl_Group.H>
#include "VideoViewGL.H"
#include <FL/Fl_Tabs.H>
#include "FileBrowser.H"
#include "MediaPanel.H"
#include "FltkEffectMenu.H"
#include "IVideoEffect.H"
#include <FL/Fl_Button.H>
#include "LoadSaveManager/LoadSaveManager.H"
#include "NoDropInput.H"
#include "Flmm_Scalebar.H"
#include "globals.H"
using namespace std;
#include "Ruler.H"
#include <FL/Fl_Scrollbar.H>
#include "TimelineScroll.H"
#include "TimelineView.H"
#include <FL/Fl_Box.H>
#include <FL/Fl_Choice.H>

class NleUI {
public:
  NleUI();
private:
  Fl_Double_Window *mainWindow;
  void cb_mainWindow_i(Fl_Double_Window*, void*);
  static void cb_mainWindow(Fl_Double_Window*, void*);
  static Fl_Menu_Item menu_[];
  void cb_New_i(Fl_Menu_*, void*);
  static void cb_New(Fl_Menu_*, void*);
  void cb_Save_i(Fl_Menu_*, void*);
  static void cb_Save(Fl_Menu_*, void*);
  void cb_Render_i(Fl_Menu_*, void*);
  static void cb_Render(Fl_Menu_*, void*);
  void cb_Quit_i(Fl_Menu_*, void*);
  static void cb_Quit(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *undo_item;
private:
  void cb_undo_item_i(Fl_Menu_*, void*);
  static void cb_undo_item(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *redo_item;
private:
  void cb_redo_item_i(Fl_Menu_*, void*);
  static void cb_redo_item(Fl_Menu_*, void*);
  void cb_Add_i(Fl_Menu_*, void*);
  static void cb_Add(Fl_Menu_*, void*);
  void cb_Add1_i(Fl_Menu_*, void*);
  static void cb_Add1(Fl_Menu_*, void*);
  void cb_Transport_i(Fl_Menu_*, void*);
  static void cb_Transport(Fl_Menu_*, void*);
  void cb_Scrub_i(Fl_Menu_*, void*);
  static void cb_Scrub(Fl_Menu_*, void*);
  void cb_Fullscreen_i(Fl_Menu_*, void*);
  static void cb_Fullscreen(Fl_Menu_*, void*);
  void cb_About_i(Fl_Menu_*, void*);
  static void cb_About(Fl_Menu_*, void*);
public:
  nle::VideoViewGL *m_videoView;
private:
  void cb__i(nle::FileBrowser*, void*);
  static void cb_(nle::FileBrowser*, void*);
public:
  nle::FltkEffectMenu *m_effectMenu;
  Fl_Hold_Browser *effect_browser;
private:
  void cb_effect_browser_i(Fl_Hold_Browser*, void*);
  static void cb_effect_browser(Fl_Hold_Browser*, void*);
public:
  Fl_Button *m_effect_up;
private:
  void cb_m_effect_up_i(Fl_Button*, void*);
  static void cb_m_effect_up(Fl_Button*, void*);
public:
  Fl_Button *m_effect_down;
private:
  void cb_m_effect_down_i(Fl_Button*, void*);
  static void cb_m_effect_down(Fl_Button*, void*);
public:
  Fl_Button *m_remove_effect;
private:
  void cb_m_remove_effect_i(Fl_Button*, void*);
  static void cb_m_remove_effect(Fl_Button*, void*);
public:
  Fl_Button *m_edit_effect;
private:
  void cb_m_edit_effect_i(Fl_Button*, void*);
  static void cb_m_edit_effect(Fl_Button*, void*);
public:
  NoDropInput *projectNameInput;
private:
  void cb_projectNameInput_i(NoDropInput*, void*);
  static void cb_projectNameInput(NoDropInput*, void*);
public:
  Fl_Button *playButton;
private:
  void cb_playButton_i(Fl_Button*, void*);
  static void cb_playButton(Fl_Button*, void*);
public:
  Fl_Button *lastButton;
private:
  void cb_lastButton_i(Fl_Button*, void*);
  static void cb_lastButton(Fl_Button*, void*);
public:
  Fl_Button *firstButton;
private:
  void cb_firstButton_i(Fl_Button*, void*);
  static void cb_firstButton(Fl_Button*, void*);
public:
  Fl_Button *backButton;
private:
  void cb_backButton_i(Fl_Button*, void*);
  static void cb_backButton(Fl_Button*, void*);
public:
  Fl_Button *forwardButton;
private:
  void cb_forwardButton_i(Fl_Button*, void*);
  static void cb_forwardButton(Fl_Button*, void*);
  Flmm_Scalebar *scaleBar;
  void cb_scaleBar_i(Flmm_Scalebar*, void*);
  static void cb_scaleBar(Flmm_Scalebar*, void*);
  void cb_1_i(Fl_Button*, void*);
  static void cb_1(Fl_Button*, void*);
public:
  Fl_Scrollbar *vScrollBar;
private:
  void cb_vScrollBar_i(Fl_Scrollbar*, void*);
  static void cb_vScrollBar(Fl_Scrollbar*, void*);
public:
  nle::TimelineScroll *scroll_area;
private:
  void cb_scroll_area_i(nle::TimelineScroll*, void*);
  static void cb_scroll_area(nle::TimelineScroll*, void*);
public:
  nle::TimelineView *m_timelineView;
  Fl_Button *positioningButton;
  Fl_Button *automationsButton;
private:
  Fl_Box *trashCan;
public:
  Fl_Button *razorButton;
  Fl_Choice *projectChoice;
private:
  void cb_projectChoice_i(Fl_Choice*, void*);
  static void cb_projectChoice(Fl_Choice*, void*);
public:
  void show( int argc, char **argv );
  ~NleUI();
  int automationsMode();
  void setEffectButtons();
};
extern Flmm_Scalebar* g_scrollBar;
#include "IVideoReader.H"
#include "IAudioReader.H"
#include <FL/Fl_Return_Button.H>
#include <iostream>
#include <FL/Fl_File_Input.H>
#include <stdlib.h>
#include <FL/Fl_File_Chooser.H>

class EncodeDialog {
public:
  EncodeDialog( nle::IVideoReader*, nle::IAudioReader*, nle::CodecParameters* codecParams );
private:
  Fl_Double_Window *encodeDialog;
  void cb_Encode_i(Fl_Return_Button*, void*);
  static void cb_Encode(Fl_Return_Button*, void*);
  void cb_Cancel_i(Fl_Button*, void*);
  static void cb_Cancel(Fl_Button*, void*);
public:
  Fl_Choice *audio_codec_menu;
private:
  void cb_audio_codec_menu_i(Fl_Choice*, void*);
  static void cb_audio_codec_menu(Fl_Choice*, void*);
public:
  Fl_Choice *video_codec_menu;
private:
  void cb_video_codec_menu_i(Fl_Choice*, void*);
  static void cb_video_codec_menu(Fl_Choice*, void*);
  static Fl_Menu_Item menu_Samplerate[];
  static Fl_Menu_Item menu_Framerate[];
public:
  Fl_Choice *frame_size_choice;
  static Fl_Menu_Item menu_frame_size_choice[];
private:
  Fl_Button *audio_options;
  void cb_audio_options_i(Fl_Button*, void*);
  static void cb_audio_options(Fl_Button*, void*);
  Fl_Button *video_options;
  void cb_video_options_i(Fl_Button*, void*);
  static void cb_video_options(Fl_Button*, void*);
public:
  Fl_File_Input *export_filename;
private:
  void cb_File_i(Fl_Button*, void*);
  static void cb_File(Fl_Button*, void*);
public:
  void show();
  int shown();
private:
  void *audio_codec;
  void* video_codec;
public:
  bool go;
  ~EncodeDialog();
private:
  nle::CodecParameters* m_codecParams;
public:
  void* frameSize();
};

class ChangesDialog {
public:
  ChangesDialog();
};
extern Fl_Box *g_trashCan;
extern float g_fps;
#include <FL/Fl_Hold_Browser.H>
#include <FL/Fl_Value_Input.H>
#include <FL/Fl_Input.H>

class CodecOptions {
public:
  CodecOptions();
  Fl_Double_Window *codecOptions;
  Fl_Box *codec_label;
  Fl_Hold_Browser *parameters_browser;
private:
  void cb_parameters_browser_i(Fl_Hold_Browser*, void*);
  static void cb_parameters_browser(Fl_Hold_Browser*, void*);
public:
  Fl_Value_Input *parameter_int_input;
private:
  void cb_parameter_int_input_i(Fl_Value_Input*, void*);
  static void cb_parameter_int_input(Fl_Value_Input*, void*);
public:
  Fl_Input *parameter_string_input;
private:
  void cb_parameter_string_input_i(Fl_Input*, void*);
  static void cb_parameter_string_input(Fl_Input*, void*);
public:
  Fl_Choice *parameter_stringlist_input;
private:
  void cb_parameter_stringlist_input_i(Fl_Choice*, void*);
  static void cb_parameter_stringlist_input(Fl_Choice*, void*);
  void cb_Cancel1_i(Fl_Button*, void*);
  static void cb_Cancel1(Fl_Button*, void*);
  void cb_Ok_i(Fl_Return_Button*, void*);
  static void cb_Ok(Fl_Return_Button*, void*);
public:
  bool m_audio;
  nle::CodecParameters* m_codecParams;
  ~CodecOptions();
};
#include "config.h"

class AboutDialog {
public:
  AboutDialog();
  Fl_Double_Window *aboutDialog;
  Fl_Box *version_box;
private:
  void cb_Close_i(Fl_Return_Button*, void*);
  static void cb_Close(Fl_Return_Button*, void*);
public:
  void show();
  int shown();
  ~AboutDialog();
};
extern Fl_Button* g_playButton;
extern Fl_Button* g_firstButton;
extern Fl_Button* g_lastButton;
extern Fl_Button* g_backButton;
extern Fl_Button* g_forwardButton;
extern bool g_snap;
extern bool g_backseek;
extern bool g_use_jack_transport;
extern bool g_scrub_audio;
extern bool g_seek_audio;
extern Fl_Scrollbar* g_v_scrollbar;
#endif
