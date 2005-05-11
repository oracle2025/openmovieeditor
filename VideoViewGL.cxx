#include <FL/gl.h>

#include "VideoViewGL.h"
#include "tga.h"

namespace nle
{

VideoViewGL::VideoViewGL( int x, int y, int w, int h, const char *l )
	: Fl_Gl_Window( x, y, w, h, l )
{
	frame_struct *fs = new frame_struct;
	fs->x   = 0;
	fs->y   = 0;
	fs->w   = 368;
	fs->h   = 240;
	fs->RGB = ;
	m_frameList.push_back( fs );
	
}

void VideoViewGL::draw()
{
	if ( !valid() ) {
		glLoadIdentity(); glViewport( 0, 0, w(), h() );
		glOrtho( -10, 10, -10, 10, -20000, 10000 ); glEnable( GL_BLEND );
		glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
		glEnable (GL_TEXTURE_2D);
    		loadTGA ("texture.tga", 13);
		/* - copied from tga.c - */
		
/*		glBindTexture (GL_TEXTURE_2D, 13);
		glPixelStorei (GL_UNPACK_ALIGNMENT, 1);
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glTexImage2D (GL_TEXTURE_2D, 0, texFormat, imageWidth, imageHeight, 0, texFormat, GL_UNSIGNED_BYTE, imageData);*/


		/* - - */
    		
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
