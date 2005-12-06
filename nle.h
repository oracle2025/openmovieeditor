// generated by Fast Light User Interface Designer (fluid) version 1.0106

#ifndef nle_h
#define nle_h
#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include "ProgressDialog/ProgressDialog.h"
#include "Renderer.H"
#include "Codecs.H"
#include <FL/Fl_Tile.H>
#include <FL/Fl_Group.H>
#include "Ruler.H"
#include "TimelineView.H"
#include "Flmm_Scalebar.H"
#include "globals.H"
using namespace std;
#include <FL/Fl_Button.H>
#include <FL/Fl_Box.H>
#include "VideoViewGL.H"
#include <FL/Fl_Tabs.H>
#include <iostream>
#include <string>
#include "FileBrowser.H"
#include <FL/Fl_Input.H>
#include "LoadSaveManager/LoadSaveManager.H"
#include <FL/Fl_Choice.H>

class NleUI {
public:
  NleUI();
private:
  Fl_Double_Window *mainWindow;
  static Fl_Menu_Item menu_[];
  inline void cb_Save_i(Fl_Menu_*, void*);
  static void cb_Save(Fl_Menu_*, void*);
  inline void cb_Render_i(Fl_Menu_*, void*);
  static void cb_Render(Fl_Menu_*, void*);
  inline void cb_Fullscreen_i(Fl_Menu_*, void*);
  static void cb_Fullscreen(Fl_Menu_*, void*);
  inline void cb_About_i(Fl_Menu_*, void*);
  static void cb_About(Fl_Menu_*, void*);
public:
  nle::TimelineView *m_timelineView;
private:
  Flmm_Scalebar *scaleBar;
  inline void cb_scaleBar_i(Flmm_Scalebar*, void*);
  static void cb_scaleBar(Flmm_Scalebar*, void*);
  Fl_Box *trashCan;
public:
  nle::VideoViewGL *m_videoView;
  nle::FileBrowser *fileBrowser;
private:
  inline void cb_fileBrowser_i(nle::FileBrowser*, void*);
  static void cb_fileBrowser(nle::FileBrowser*, void*);
public:
  Fl_Input *projectNameInput;
private:
  inline void cb_projectNameInput_i(Fl_Input*, void*);
  static void cb_projectNameInput(Fl_Input*, void*);
  inline void cb__i(Fl_Button*, void*);
  static void cb_(Fl_Button*, void*);
public:
  Fl_Choice *projectChoice;
  void show( int argc, char **argv );
  ~NleUI();
};
extern Flmm_Scalebar* g_scrollBar;
#include "IVideoReader.H"
#include "IAudioReader.H"
#include <FL/Fl_Return_Button.H>
#include <FL/Fl_File_Input.H>
#include <stdlib.h>
#include <FL/Fl_File_Chooser.H>

class EncodeDialog {
public:
  EncodeDialog( nle::IVideoReader*, nle::IAudioReader*, nle::CodecParameters* codecParams );
private:
  Fl_Double_Window *encodeDialog;
  inline void cb_Encode_i(Fl_Return_Button*, void*);
  static void cb_Encode(Fl_Return_Button*, void*);
  inline void cb_Cancel_i(Fl_Button*, void*);
  static void cb_Cancel(Fl_Button*, void*);
public:
  Fl_Choice *audio_codec_menu;
private:
  inline void cb_audio_codec_menu_i(Fl_Choice*, void*);
  static void cb_audio_codec_menu(Fl_Choice*, void*);
public:
  Fl_Choice *video_codec_menu;
private:
  inline void cb_video_codec_menu_i(Fl_Choice*, void*);
  static void cb_video_codec_menu(Fl_Choice*, void*);
  static Fl_Menu_Item menu_Samplerate[];
  static Fl_Menu_Item menu_Framerate[];
  static Fl_Menu_Item menu_Framesize[];
  Fl_Button *audio_options;
  inline void cb_audio_options_i(Fl_Button*, void*);
  static void cb_audio_options(Fl_Button*, void*);
  Fl_Button *video_options;
  inline void cb_video_options_i(Fl_Button*, void*);
  static void cb_video_options(Fl_Button*, void*);
public:
  Fl_File_Input *export_filename;
private:
  inline void cb_File_i(Fl_Button*, void*);
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
};

class ChangesDialog {
public:
  ChangesDialog();
};
extern Fl_Box *g_trashCan;
extern float g_fps;
#include <FL/Fl_Hold_Browser.H>
#include <FL/Fl_Value_Input.H>

class CodecOptions {
public:
  CodecOptions();
  Fl_Double_Window *codecOptions;
  Fl_Box *codec_label;
  Fl_Hold_Browser *parameters_browser;
private:
  inline void cb_parameters_browser_i(Fl_Hold_Browser*, void*);
  static void cb_parameters_browser(Fl_Hold_Browser*, void*);
public:
  Fl_Value_Input *parameter_int_input;
private:
  inline void cb_parameter_int_input_i(Fl_Value_Input*, void*);
  static void cb_parameter_int_input(Fl_Value_Input*, void*);
public:
  Fl_Input *parameter_string_input;
private:
  inline void cb_parameter_string_input_i(Fl_Input*, void*);
  static void cb_parameter_string_input(Fl_Input*, void*);
public:
  Fl_Choice *parameter_stringlist_input;
private:
  inline void cb_parameter_stringlist_input_i(Fl_Choice*, void*);
  static void cb_parameter_stringlist_input(Fl_Choice*, void*);
  inline void cb_Cancel1_i(Fl_Button*, void*);
  static void cb_Cancel1(Fl_Button*, void*);
public:
  bool m_audio;
  nle::CodecParameters* m_codecParams;
  ~CodecOptions();
};

class AboutDialog {
public:
  AboutDialog();
  Fl_Double_Window *aboutDialog;
private:
  inline void cb_Close_i(Fl_Return_Button*, void*);
  static void cb_Close(Fl_Return_Button*, void*);
public:
  void show();
  int shown();
  ~AboutDialog();
};
#include <FL/Fl_Text_Display.H>

class ErrorDialog {
public:
  ErrorDialog();
};

class SaveAsDialog {
public:
  SaveAsDialog();
  Fl_Double_Window *saveAsDialog;
private:
  inline void cb_Ok_i(Fl_Return_Button*, void*);
  static void cb_Ok(Fl_Return_Button*, void*);
  inline void cb_Cancel2_i(Fl_Button*, void*);
  static void cb_Cancel2(Fl_Button*, void*);
public:
  void show();
  int shown();
};
#endif
