#ifndef _NEXT_FRAME_LISTENER_H_
#define _NEXT_FRAME_LISTENER_H_

#include <stdint.h>

namespace nle
{

class NextFrameListener
{
	public:
		virtual void nextFrame( int64_t frame ) = 0;
};

}

#endif
