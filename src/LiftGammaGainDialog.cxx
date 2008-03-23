/* LiftGammaGainDialog.cxx
 *
 *  Copyright (C) 2008 Richard Spindler <richard.spindler AT gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Return_Button.H>

#include "LiftGammaGainDialog.H"
#include "LiftGammaGainFilter.H"
#include "LiftGammaGainWidget.H"
#include "VideoViewGL.H"


namespace nle
{

static void closeCallback( Fl_Widget*, void* data ) {
	LiftGammaGainDialog* dlg = (LiftGammaGainDialog*)data;
	delete dlg;
}

LiftGammaGainDialog::LiftGammaGainDialog( LiftGammaGainFilter* filter )
	: m_filter( filter )
{
	m_dialog = new Fl_Double_Window( 430, 195 + 25 + 10, "Lift Gamma Gain" );

	m_widget = new LiftGammaGainWidget( 5, 5, 420, 190 );
	m_widget->m_dialog = this;

	{
		Fl_Return_Button* o = new Fl_Return_Button( 5, 200, 420, 25, "Close" );
		o->callback( closeCallback, this );
		m_dialog->hotspot( o );
	}


	m_dialog->set_non_modal();
	m_dialog->end();
}
LiftGammaGainDialog::~LiftGammaGainDialog()
{
	m_filter->m_dialog = 0;
	m_dialog->hide();
	delete m_dialog;
}
void LiftGammaGainDialog::show()
{
	m_dialog->show();
}
int LiftGammaGainDialog::shown()
{
	return m_dialog->shown();
}
void LiftGammaGainDialog::read_values()
{
	//pass values from dialog to filter
	struct lift_gamma_gain_data* lggd = &(m_filter->m_parameters);

	lggd->lift.x = m_widget->lift->m_x;
	lggd->lift.y = m_widget->lift->m_y;
	lggd->gamma.x = m_widget->gamma->m_x;
	lggd->gamma.y = m_widget->gamma->m_y;
	lggd->gain.x = m_widget->gain->m_x;
	lggd->gain.y = m_widget->gain->m_y;

	m_filter->lift( m_widget->lift->m_fcolor[0], m_widget->lift->m_fcolor[1], m_widget->lift->m_fcolor[2] );
	m_filter->gamma( m_widget->gamma->m_fcolor[0], m_widget->gamma->m_fcolor[1], m_widget->gamma->m_fcolor[2] );
	m_filter->gain( m_widget->gain->m_fcolor[0], m_widget->gain->m_fcolor[1], m_widget->gain->m_fcolor[2] );
	m_filter->calculate_values();
	m_filter->bypass( m_widget->bypass->value() );

	g_videoView->redraw();
}

} /* namespace nle */
