#include <FL/Fl.H>
#include "nle.h"

int
main( int argc, char** argv ) {
  NleUI* nui = new NleUI;
  Fl::visual(FL_DOUBLE|FL_INDEX);

  nui->show( argc, argv );
  return Fl::run();
}
