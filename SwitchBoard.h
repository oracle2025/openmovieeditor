#ifndef _SWITCH_BOARD_H_
#define _SWITCH_BOARD_H_

namespace nle
{
class Timeline;
class TimelineView;

class SwitchBoard
{
	public:
		inline static SwitchBoard* i() //Singleton sucks, should be fixed,
		{
			static SwitchBoard inst;
			return &inst;
		}
		void zoom( float zoom );
		float zoom();
		void timeline( Timeline* tl );
		void timelineView( TimelineView* tlv );
		Timeline* timeline();
		void move_cursor();
	private:
		SwitchBoard();
		float m_zoom; // = 1.0;
		Timeline *m_timeline;
		TimelineView *m_tlv;
};

} /* namespace nle */


#endif /* _SWITCH_BOARD_H_ */
