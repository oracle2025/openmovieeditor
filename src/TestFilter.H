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
		DragHandler* onMouseDown( Rect& rect, int x, int y, bool shift );
		int fillBuffer( float*, unsigned long frames, int64_t ) { return frames; }
		const char* name() { return "Test Filter"; }
		void writeXML( TiXmlElement* ) {}
		void readXML( TiXmlElement* ) {}
};

} /* namespace nle */

#endif /* _TEST_FILTER_ */
