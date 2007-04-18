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
#include "ColorCurveFilter.H"
#include "ColorGrader2.h"


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
	m_dialog = new Fl_Double_Window( 605, 200 + 25 + 10, "Color Curves" );

	//ColorGrader2* cg = new ColorGrader2( 0, 0, 430, 335 );
	ColorGrader2* cg = new ColorGrader2( 0, 0, 605, 200 );
	cg->editor_red->lineColor( FL_RED );
	cg->editor_green->lineColor( FL_GREEN );
	cg->editor_blue->lineColor( FL_BLUE );
	cg->editor_master->lineColor( FL_BLACK );

	cg->m_values_r = filter->m_values_r;
	cg->m_values_g = filter->m_values_g;
	cg->m_values_b = filter->m_values_b;

	{
		Fl_Return_Button* o = new Fl_Return_Button( 5, 205, 595, 25, "Close" );
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
