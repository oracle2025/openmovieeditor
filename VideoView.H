#ifndef _VIDEO_VIEW_H_
#define _VIDEO_VIEW_H_

#include "NextFrameListener.H"
namespace nle
{
class Timeline;
class Sound;

class VideoView : public Fl_Widget, NextFrameListener
{
	public:
		VideoView( int x, int y, int w, int h, const char *label = 0 );
		~VideoView();
		void play();
		void stop();
		void draw();
		void nextFrame( int64_t frame );
	private:
		Timeline *m_timeline;
		Sound *m_snd;
};
	
} /* namespace nle */
#endif /* _VIDEO_VIEW_H_ */

