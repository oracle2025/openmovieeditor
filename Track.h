#ifndef _TRACK_H_
#define _TRACK_H_

#include <list>

namespace nle
{
class Clip;

enum Track_Types {
	TRACK_TYPE_VIDEO = 0,
	TRACK_TYPE_AUDIO = 1
};

class Track
{
	public:
		virtual ~Track(){}
		std::list< Clip* >* getClips() { return &m_clips; }
		virtual int type() = 0;
		virtual void reset();
		void add( Clip* clip );
		void remove( Clip* clip );
		int num() { return m_num; }
	protected:
		Track( int num );
		std::list< Clip* > m_clips;
	private:
		int m_num;
};
	
} /* namespace nle */

#endif /* _TRACK_H_ */

