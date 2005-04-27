#ifndef _CLIP_H_
#define _CLIP_H_

#include <stdint.h>

namespace nle
{

class Track;

class Clip
{
	public:
		virtual ~Clip() {}
		virtual int64_t length() = 0;
		virtual void reset() = 0;
		void position( int64_t position ) { m_position = position; }
		int64_t position() { return m_position; }
		Track* track() { return m_track; }
		void track( Track* track ) { m_track = track; }
		void trimA( int64_t trim );
		void trimB( int64_t trim );
	protected:
		Clip( Track *track, int64_t position );
		int64_t m_position;
		Track* m_track;
		int64_t m_trimA;
		int64_t m_trimB; // wird von hinten gemessen.
};

} /* namespace nle */

#endif /* _CLIP_H_ */
