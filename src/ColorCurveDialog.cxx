/* ColorCurveDialog.cxx
 *
 *  Copyright (C) 2007 Richard Spindler <richard.spindler AT gmail.com>
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

#include "ColorCurveDialog.H"
#include "CurveEditor.H"
#include "ColorCurveFilter.H"


namespace nle
{
static void closeCallback( Fl_Widget*, void* data ) {
//	i->window()->hide();
	ColorCurveDialog* dlg = (ColorCurveDialog*)data;
	delete dlg;
}

ColorCurveDialog::ColorCurveDialog( ColorCurveFilter* filter )
	: m_filter( filter )
{
	m_dialog = new Fl_Double_Window( 210, 210 + 25 + 5, "Frei0r Effect" );

	new CurveEditor( 5, 5, 200, 200, filter->m_values );
	{
		Fl_Return_Button* o = new Fl_Return_Button( 5, 210, 200, 25, "Close" );
		o->callback( closeCallback, this );
		m_dialog->hotspot( o );
	}
	
	m_dialog->set_non_modal();
	m_dialog->end();
}
ColorCurveDialog::~ColorCurveDialog()
{
	m_filter->m_dialog = 0;
	m_dialog->hide();
	delete m_dialog;
}
void ColorCurveDialog::show()
{
	m_dialog->show();
}
int ColorCurveDialog::shown()
{
	return m_dialog->shown();
}

} /* namespace nle */
