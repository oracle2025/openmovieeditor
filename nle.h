// generated by Fast Light User Interface Designer (fluid) version 1.0105

#ifndef nle_h
#define nle_h
#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Tile.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Tabs.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_File_Browser.H>
#include "VideoView.h"
#include <FL/Fl_Button.H>
#include "TimelineView.h"
#include <FL/Fl_Slider.H>

class NleUI {
public:
  NleUI();
private:
  Fl_Double_Window *mainWindow;
  static Fl_Menu_Item menu_[];
public:
  Fl_File_Browser *fileBrowser;
  nle::VideoView *m_videoView;
private:
  inline void cb__i(Fl_Button*, void*);
  static void cb_(Fl_Button*, void*);
  inline void cb_1_i(Fl_Button*, void*);
  static void cb_1(Fl_Button*, void*);
public:
  nle::TimelineView *m_timelineView;
private:
  inline void cb_2_i(Fl_Slider*, void*);
  static void cb_2(Fl_Slider*, void*);
public:
  void show( int argc, char **argv );
};
#endif
