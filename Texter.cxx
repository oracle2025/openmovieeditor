#include <string>
#include <iostream>
#include <Magick++.h>

#include "Texter.H"

using namespace std;
using namespace Magick;

namespace nle
{
/* 

   
   Image image( 640, 480, "RGBA" )
   Image image( "640x480", Magick::Color( 0, 0, 0, 0 ) );
   image.annotate("sunfun",CenterGravity )
*/

Texter::Texter()
{
	m_blob = new Blob();
}
Texter::~Texter()
{
	delete m_blob;
}
void *Texter::generateText( unsigned int w, unsigned int h, const char* text )
{
	char buf[255];
	snprintf( buf, 255, "%dx%d", w, h );

	try {
		string font = "Helvetica";
		//Image model( buf, Color( 0, 0, 0, MaxRGB ) );
		Image model("out.png");
		Blob blob;
		model.fontPointsize( 50 );
		model.font( "Helvetica" );
		model.strokeColor( Color() );                                                                    
		model.fillColor( "black" );
		model.annotate( "OpenGL", "+0+20", NorthGravity );

		model.magick( "RGBA" );
		model.write( m_blob );
		
		/*Image model( buf, Color( MaxRGB, MaxRGB, MaxRGB, 0 ) );
		model.fontPointsize( 18 );
		model.density( "72x72" );
		model.font( font );
		model.strokeColor( Color() );                                                                    
		model.fillColor( "goldenrod" );
		model.annotate( text, "+0+20", NorthGravity );

		model.magick( "RGBA" );
		model.write( m_blob );
		
		model.magick( "JPEG" );
		model.write( "out.jpg" );*/

		
	} catch( exception &error_ ) {
//		cout << "Caught exception: " << error_.what() << endl;
		return (void *)0;
	}
	return (void *)m_blob->data();

}
} /* namespace nle */


