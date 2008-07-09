#ifndef _EFFECT_DIALOG_BUTTON_WIDGET_H_
#define _EFFECT_DIALOG_BUTTON_WIDGET_H_

#include "IEffectWidget.H"

class Fl_Button;

namespace nle
{

class IVideoEffect;

class EffectDialogButtonWidget : public IEffectWidget
{
	public:
		EffectDialogButtonWidget( IVideoEffect* filter );
		~EffectDialogButtonWidget();
		void cb_Edit_i( Fl_Button* o );
		static void cb_Edit( Fl_Button* o, void* v );
	private:
		IVideoEffect* m_filter;
};

} /* namespace nle */

#endif /* _EFFECT_DIALOG_BUTTON_WIDGET_H_ */
