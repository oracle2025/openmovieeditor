#include "NoDropInput.H"

NoDropInput::NoDropInput( int a, int b, int c, int d, const char *e )
	: Fl_Input( a, b, c, d, e )
{
}

int NoDropInput::handle( int event )
{
	if ( event == FL_PASTE ) {
		return 1;
	}
	return Fl_Input::handle( event );
}

