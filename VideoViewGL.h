#ifndef _VIDEO_VIEW_GL_H_
#define _VIDEO_VIEW_GL_H_

#include <FL/Fl.H>
#include <FL/Fl_Gl_Window.H>

#include <list>

#include "NextFrameListener.h"
#include "frame_struct.h"

namespace nle
{
class Timeline;
class Sound;

class VideoViewGL : public Fl_Gl_Window, NextFrameListener
{
	public:
		VideoViewGL( int x, int y, int w, int h, const char *l = 0 );
		void draw();
		void nextFrame( int64_t frame );
		void play(){}
		void stop(){}
		void addFrame( frame_struct* frame );
		/* add Frame */
	private:
		Timeline *m_timeline;
		Sound *m_snd;
		std::list< frame_struct* > m_frameList;
};

} /* namespace nle */


#endif /* _VIDEO_VIEW_GL_H_ */
