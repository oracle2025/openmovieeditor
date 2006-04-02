#ifndef _NO_DROP_INPUT_H_
#define _NO_DROP_INPUT_H_

#include <FL/Fl_Input.H>

class NoDropInput : public Fl_Input
{
	public:
		NoDropInput( int, int, int, int, const char * = 0 );
		int handle( int event );
};

#endif /* _NO_DROP_INPUT_H_ */
