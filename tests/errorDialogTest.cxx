
#include "ErrorDialog/FltkErrorHandler.H"


int main()
{
	nle::FltkErrorHandler e;

	e.detail( "This is a detail" );
	e.detail( "This is a second detail" );
	e.error( "This is the error" );
	return 0;
}
