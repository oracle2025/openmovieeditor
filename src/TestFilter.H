/* TestFilter.H
 */

#ifndef _TEST_FILTER_
#define _TEST_FILTER_

#include "AudioFilter.H"

namespace nle
{

class Clip;

class TestFilter : public AudioFilter
{
	public:
		TestFilter( Clip* clip );
		void onDraw( Rect& rect );
		DragHandler* onMouseDown();
		int fillBuffer( float*, unsigned long frames, int64_t ) { return frames; }
};

} /* namespace nle */

#endif /* _TEST_FILTER_ */
