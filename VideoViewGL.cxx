#include <FL/gl.h>

#include "VideoViewGL.h"
#include "tga.h"

namespace nle
{

VideoViewGL::VideoViewGL( int x, int y, int w, int h, const char *l )
	: Fl_Gl_Window( x, y, w, h, l )
{
}

void VideoViewGL::draw()
{
	if ( !valid() ) {
		glLoadIdentity(); glViewport( 0, 0, w(), h() );
		glOrtho( -10, 10, -10, 10, -20000, 10000 ); glEnable( GL_BLEND );
		glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
		glEnable (GL_TEXTURE_2D);
    		loadTGA ("texture.tga", 13);
    		
	}
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glBindTexture (GL_TEXTURE_2D, 13);
	glBegin (GL_QUADS);
	glTexCoord2f (0.0, 0.0);
	glVertex3f (0.0, 0.0, 0.0);
	glTexCoord2f (1.0, 0.0);
	glVertex3f (9.0, 0.0, 0.0);
	glTexCoord2f (1.0, 1.0);
	glVertex3f (9.0, 9.0, 0.0);
	glTexCoord2f (0.0, 1.0);
	glVertex3f (0.0, 9.0, 0.0);
	glEnd ();
	/*
	glTexSubImage2D
	http://www.gamedev.net/reference/articles/article947.asp
	http://developer.apple.com/samplecode/OpenGL_Movie/listing4.html
	 */
}

void VideoViewGL::nextFrame( int64_t frame )
{
}


} /* namespace nle */
