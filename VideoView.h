#ifndef _VIDEO_VIEW_H_
#define _VIDEO_VIEW_H_
namespace nle
{
class Timeline;

class VideoView : public Fl_Widget
{
	public:
		VideoView( int x, int y, int w, int h, const char *label = 0 );
		~VideoView();
		void play();
		void stop();
		void draw();
	private:
		Timeline *m_timeline;
};
	
} /* namespace nle */
#endif /* _VIDEO_VIEW_H_ */

