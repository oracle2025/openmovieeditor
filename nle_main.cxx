#include <FL/Fl.H>
#include "nle.h"

int
main( int argc, char** argv ) {
  NleUI nui;
  Fl::visual(FL_DOUBLE|FL_RGB);
  
  Fl::background2( 34, 52, 103 );
  Fl::background( 93, 93, 114 );
  Fl::foreground( 255, 255, 255 );
  Fl::set_color( FL_BLACK, 200, 200, 200 );
	  
//  Fl::set_color(FL_BACKGROUND_COLOR, 65, 65, 80);
//  Fl::set_color(FL_BACKGROUND2_COLOR, 65, 65, 80);

  nui.show( argc, argv );
  return Fl::run();
}
