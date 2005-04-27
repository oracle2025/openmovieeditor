#include <FL/Fl.H>
#include "nle.h"

int
main( int argc, char** argv ) {
  NleUI nui;
  Fl::visual(FL_DOUBLE|FL_RGB);

  nui.show( argc, argv );
  return Fl::run();
}
