#ifndef _DRAW_H_
#define _DRAW_H_

namespace nle
{

class Draw
{
	public:
		static void box( float x, float y, float w, float h, Fl_Color c );
		static void triangle( float x, float y, bool direction );
/*		static void VideoClip();
		static void VideoTrack();
		static void VideoTrackBg();*/
};

} /* namespace nle */

#endif /* _DRAW_H_ */
