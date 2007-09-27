// generated by Fast Light User Interface Designer (fluid) version 1.0107

#ifndef nle_h
#define nle_h
#include <FL/Fl.H>
#include "EncodingPreset.H"
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include "ProgressDialog/ProgressDialog.h"
#include "Renderer.H"
#include "Prefs.H"
#include "LoadSaveManager/LoadSaveManager.H"
#include "DocManager.H"
#include "Timeline.H"
#include "VideoTrack.H"
#include "AudioTrack.H"
#include <FL/Fl_Tooltip.H>
#include <FL/Fl_Tile.H>
#include <FL/Fl_Group.H>
#include "VideoViewGL.H"
#include <FL/Fl_Slider.H>
#include "IPlaybackCore.H"
#include <FL/Fl_Button.H>
#include <FL/Fl_Tabs.H>
#include "FileBrowser.H"
#include "MediaPanel.H"
#include "FltkEffectMenu.H"
#include "IVideoEffect.H"
#include "SpecialClipsBrowser.H"
#include <FL/Fl_Input.H>
#include <FL/Fl_Value_Input.H>
#include <math.h>
#include <FL/fl_show_colormap.H>
#include <FL/Fl_Box.H>
#include "fl_font_browser.h"
#include <FL/Fl_Choice.H>
#include "Ruler.H"
#include <FL/Fl_Scrollbar.H>
#include "TimelineScroll.H"
#include "TimelineView.H"
#include "Flmm_Scalebar.H"
#include "globals.H"
using namespace std;

class NleUI {
public:
  NleUI();
  Fl_Double_Window *mainWindow;
private:
  void cb_mainWindow_i(Fl_Double_Window*, void*);
  static void cb_mainWindow(Fl_Double_Window*, void*);
  static Fl_Menu_Item menu_Black[];
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
public:
  static Fl_Menu_Item *cut_item;
private:
  void cb_cut_item_i(Fl_Menu_*, void*);
  static void cb_cut_item(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *copy_item;
private:
  void cb_copy_item_i(Fl_Menu_*, void*);
  static void cb_copy_item(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *paste_item;
private:
  void cb_paste_item_i(Fl_Menu_*, void*);
  static void cb_paste_item(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *delete_item;
private:
  void cb_delete_item_i(Fl_Menu_*, void*);
  static void cb_delete_item(Fl_Menu_*, void*);
  void cb_Add_i(Fl_Menu_*, void*);
  static void cb_Add(Fl_Menu_*, void*);
  void cb_Add1_i(Fl_Menu_*, void*);
  static void cb_Add1(Fl_Menu_*, void*);
  void cb_4_i(Fl_Menu_*, void*);
  static void cb_4(Fl_Menu_*, void*);
  void cb_16_i(Fl_Menu_*, void*);
  static void cb_16(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *black_border_item;
private:
  void cb_black_border_item_i(Fl_Menu_*, void*);
  static void cb_black_border_item(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *black_border_item_2_35;
private:
  void cb_black_border_item_2_35_i(Fl_Menu_*, void*);
  static void cb_black_border_item_2_35(Fl_Menu_*, void*);
  void cb_Disable_i(Fl_Menu_*, void*);
  static void cb_Disable(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *jackMenu;
private:
  void cb_Transport_i(Fl_Menu_*, void*);
  static void cb_Transport(Fl_Menu_*, void*);
  void cb_Scrub_i(Fl_Menu_*, void*);
  static void cb_Scrub(Fl_Menu_*, void*);
  void cb_Fullscreen_i(Fl_Menu_*, void*);
  static void cb_Fullscreen(Fl_Menu_*, void*);
  void cb_Normal_i(Fl_Menu_*, void*);
  static void cb_Normal(Fl_Menu_*, void*);
  void cb_Plastic_i(Fl_Menu_*, void*);
  static void cb_Plastic(Fl_Menu_*, void*);
  void cb_Plastic1_i(Fl_Menu_*, void*);
  static void cb_Plastic1(Fl_Menu_*, void*);
  void cb_Tutorial_i(Fl_Menu_*, void*);
  static void cb_Tutorial(Fl_Menu_*, void*);
  void cb_About_i(Fl_Menu_*, void*);
  static void cb_About(Fl_Menu_*, void*);
public:
  nle::VideoViewGL *m_videoView;
  Fl_Slider *zoom_slider;
private:
  void cb_zoom_slider_i(Fl_Slider*, void*);
  static void cb_zoom_slider(Fl_Slider*, void*);
  void cb__i(Fl_Button*, void*);
  static void cb_(Fl_Button*, void*);
public:
  Fl_Tabs *tab_view;
private:
  void cb_1_i(nle::FileBrowser*, void*);
  static void cb_1(nle::FileBrowser*, void*);
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
  nle::SpecialClipsBrowser *special_clips;
private:
  Fl_Group *titles_tab;
public:
  Fl_Input *titles_text;
private:
  void cb_titles_text_i(Fl_Input*, void*);
  static void cb_titles_text(Fl_Input*, void*);
public:
  Fl_Value_Input *titles_size;
private:
  void cb_titles_size_i(Fl_Value_Input*, void*);
  static void cb_titles_size(Fl_Value_Input*, void*);
public:
  Fl_Button *titles_color;
private:
  void cb_titles_color_i(Fl_Button*, void*);
  static void cb_titles_color(Fl_Button*, void*);
public:
  Fl_Slider *titles_x;
private:
  void cb_titles_x_i(Fl_Slider*, void*);
  static void cb_titles_x(Fl_Slider*, void*);
public:
  Fl_Slider *titles_y;
private:
  void cb_titles_y_i(Fl_Slider*, void*);
  static void cb_titles_y(Fl_Slider*, void*);
  void cb_Font_i(Fl_Button*, void*);
  static void cb_Font(Fl_Button*, void*);
public:
  Fl_Choice *titles_font;
private:
  void cb_titles_font_i(Fl_Choice*, void*);
  static void cb_titles_font(Fl_Choice*, void*);
  static Fl_Menu_Item menu_titles_font[];
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
public:
  Fl_Button *pauseButton;
private:
  void cb_pauseButton_i(Fl_Button*, void*);
  static void cb_pauseButton(Fl_Button*, void*);
public:
  Fl_Button *pa_playButton;
private:
  void cb_pa_playButton_i(Fl_Button*, void*);
  static void cb_pa_playButton(Fl_Button*, void*);
public:
  Fl_Button *pa_lastButton;
private:
  void cb_pa_lastButton_i(Fl_Button*, void*);
  static void cb_pa_lastButton(Fl_Button*, void*);
public:
  Fl_Button *pa_firstButton;
private:
  void cb_pa_firstButton_i(Fl_Button*, void*);
  static void cb_pa_firstButton(Fl_Button*, void*);
public:
  Fl_Button *pa_backButton;
private:
  void cb_pa_backButton_i(Fl_Button*, void*);
  static void cb_pa_backButton(Fl_Button*, void*);
public:
  Fl_Button *pa_forwardButton;
private:
  void cb_pa_forwardButton_i(Fl_Button*, void*);
  static void cb_pa_forwardButton(Fl_Button*, void*);
public:
  Fl_Button *projectNameInput;
private:
  void cb_projectNameInput_i(Fl_Button*, void*);
  static void cb_projectNameInput(Fl_Button*, void*);
  void cb_2_i(Fl_Button*, void*);
  static void cb_2(Fl_Button*, void*);
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
private:
  Flmm_Scalebar *scaleBar;
  void cb_scaleBar_i(Flmm_Scalebar*, void*);
  static void cb_scaleBar(Flmm_Scalebar*, void*);
public:
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
  void deactivate_titles();
  void activate_titles(int font, int size, const char* text, float x, float y, Fl_Color color );
  void portaudio();
  void jack();
};
extern Flmm_Scalebar* g_scrollBar;
#include <FL/Fl_Return_Button.H>

class ChangesDialog {
public:
  ChangesDialog();
};
extern Fl_Box *g_trashCan;
extern float g_fps;
#include <FL/Fl_Hold_Browser.H>

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
  void cb_Cancel_i(Fl_Button*, void*);
  static void cb_Cancel(Fl_Button*, void*);
  void cb_Ok_i(Fl_Return_Button*, void*);
  static void cb_Ok(Fl_Return_Button*, void*);
public:
  bool m_audio;
  nle::EncodingPreset* m_preset;
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
extern bool g_16_9;
extern bool g_black_borders;
extern bool g_black_borders_2_35;
void font_dialog_callback( Fl_Widget*, void* v );
extern Fl_Font titleFont;
#include <FL/Fl_Browser.H>
#include <FL/Fl_File_Input.H>
extern Fl_File_Input *export_filename_simple;
#include <stdlib.h>
#include <FL/Fl_File_Chooser.H>
Fl_Double_Window* encoder_2_dlg();
extern Fl_Menu_Item menu_Framerate[];
extern Fl_Menu_Item menu_Framesize[];
extern Fl_Menu_Item menu_Samplerate[];
extern Fl_Menu_Item menu_Quality[];
extern Fl_Menu_Item menu_Aspect[];
#include "VideoWriterQT.H"
#include <colormodels.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <FL/Fl_Text_Display.H>

class ExportDialog {
public:
  ExportDialog();
  Fl_Double_Window *dialog_window;
  Fl_File_Input *export_filename;
private:
  void cb_File1_i(Fl_Button*, void*);
  static void cb_File1(Fl_Button*, void*);
  void cb_Encode_i(Fl_Return_Button*, void*);
  static void cb_Encode(Fl_Return_Button*, void*);
  void cb_Cancel1_i(Fl_Button*, void*);
  static void cb_Cancel1(Fl_Button*, void*);
public:
  Fl_Text_Display *information_display;
  Fl_Hold_Browser *presets_browser;
private:
  void cb_presets_browser_i(Fl_Hold_Browser*, void*);
  static void cb_presets_browser(Fl_Hold_Browser*, void*);
  void cb_3_i(Fl_Button*, void*);
  static void cb_3(Fl_Button*, void*);
public:
  Fl_Button *edit_format_button;
private:
  void cb_edit_format_button_i(Fl_Button*, void*);
  static void cb_edit_format_button(Fl_Button*, void*);
public:
  Fl_Button *remove_format_button;
private:
  void cb_remove_format_button_i(Fl_Button*, void*);
  static void cb_remove_format_button(Fl_Button*, void*);
public:
  nle::IVideoFileWriter* getFileWriter();
  bool go;
  ~ExportDialog();
  int shown();
  void show();
};
#include <iostream>
#include <FL/Fl_Spinner.H>
#include <FL/Fl_Output.H>
#include <FL/Fl_Check_Button.H>

class CustomFormatDialog {
  void *audio_codec;
  void* video_codec;
  nle::EncodingPreset* m_preset;
public:
  CustomFormatDialog();
  Fl_Double_Window *dialog_window;
  Fl_Choice *video_codec_menu;
private:
  void cb_video_codec_menu_i(Fl_Choice*, void*);
  static void cb_video_codec_menu(Fl_Choice*, void*);
  Fl_Button *video_options;
  void cb_video_options_i(Fl_Button*, void*);
  static void cb_video_options(Fl_Button*, void*);
public:
  Fl_Choice *audio_codec_menu;
private:
  void cb_audio_codec_menu_i(Fl_Choice*, void*);
  static void cb_audio_codec_menu(Fl_Choice*, void*);
  Fl_Button *audio_options;
  void cb_audio_options_i(Fl_Button*, void*);
  static void cb_audio_options(Fl_Button*, void*);
public:
  Fl_Choice *samplerate;
  static Fl_Menu_Item menu_samplerate[];
  Fl_Input *name;
  Fl_Choice *frame_rate_choice;
  static Fl_Menu_Item menu_frame_rate_choice[];
  Fl_Spinner *frame_size_w;
  Fl_Spinner *frame_size_h;
  Fl_Spinner *aspect_w;
  Fl_Spinner *aspect_h;
  Fl_Choice *interlacing;
  static Fl_Menu_Item menu_interlacing[];
  Fl_Spinner *black_pixel_v;
  Fl_Spinner *black_pixel_h;
  Fl_Spinner *analog_blank;
  Fl_Output *pixel_aspect;
private:
  void cb_Save1_i(Fl_Return_Button*, void*);
  static void cb_Save1(Fl_Return_Button*, void*);
  void cb_Cancel2_i(Fl_Button*, void*);
  static void cb_Cancel2(Fl_Button*, void*);
public:
  int shown();
  void show();
  ~CustomFormatDialog();
  nle::EncodingPreset* getEncodingPreset();
private:
  bool go;
public:
  void setEncodingPreset(nle::EncodingPreset* preset);
};
#endif
