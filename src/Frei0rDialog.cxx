/* Frei0rDialog.cxx
 *
 *  Copyright (C) 2006 Richard Spindler <richard.spindler AT gmail.com>
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
#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Value_Input.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Spinner.H>

#include "Frei0rDialog.H"
#include "Frei0rEffect.H"
#include "frei0r.h"
#include "globals.H"
#include "VideoViewGL.H"
#include "global_includes.H"

namespace nle
{

void doubleCallback( Fl_Widget* i, void* v )
{
	Fl_Value_Input* vi = dynamic_cast<Fl_Value_Input*>(i);
	callback_info* info = (callback_info*)v;
	info->dialog->setDouble( info->number, vi->value() );
}
void boolCallback( Fl_Widget* i, void* v )
{
	Fl_Check_Button* vi = dynamic_cast<Fl_Check_Button*>(i);
	callback_info* info = (callback_info*)v;
	info->dialog->setDouble( info->number, vi->value() );
}
void xCallback( Fl_Widget* i, void* v )
{
	Fl_Spinner* vi = dynamic_cast<Fl_Spinner*>(i);
	callback_info* info = (callback_info*)v;
	info->dialog->setPositionX( info->number, vi->value() );
}
void yCallback( Fl_Widget* i, void* v )
{
	Fl_Spinner* vi = dynamic_cast<Fl_Spinner*>(i);
	callback_info* info = (callback_info*)v;
	info->dialog->setPositionY( info->number, vi->value() );
}

Frei0rDialog::Frei0rDialog( Frei0rEffect* effect )
	: m_effect( effect )
{
	f0r_plugin_info_t* finfo;
	f0r_param_info_t pinfo;
	
	finfo = m_effect->getPluginInfo();
	
	int height = 30 * finfo->num_params + 5;

	m_infostack = new callback_info[finfo->num_params];
	
	m_dialog = new Fl_Double_Window( 340, height, "Frei0r Effect" );

	for ( int i = 0; i < finfo->num_params; i++ ) {
		m_effect->getParamInfo( &pinfo, i );
		m_infostack[i].dialog = this;
		m_infostack[i].number = i;
		int x, y, w, h;
		w = 240;
		h = 25;
		x = 100;
		y = 5 + ( i * 30 );
		switch ( pinfo.type ) {
			case F0R_PARAM_DOUBLE: //Seems to be always between 0.0 and 1.0
				{
				Fl_Value_Input* vi = new Fl_Value_Input( x, y, w, h, pinfo.name );
				vi->callback( doubleCallback, &(m_infostack[i]) );
				break;
				}
			case F0R_PARAM_BOOL:
				{
				Fl_Check_Button* b = new Fl_Check_Button( x, y, w, h, pinfo.name );
				b->callback( boolCallback, &(m_infostack[i]) );
				break;
				}
			case F0R_PARAM_COLOR:
				break;
			case F0R_PARAM_POSITION:
				{
				Fl_Spinner* sx = new Fl_Spinner( x, y, 150, h, pinfo.name);
				Fl_Spinner* sy = new Fl_Spinner( x + 150, y, 150, h );
				sx->callback( boolCallback, &(m_infostack[i]) );
				sy->callback( boolCallback, &(m_infostack[i]) );
				break;
				}
			default:
				break;
		};
	}
	m_dialog->set_modal();
	m_dialog->end();
}

Frei0rDialog::~Frei0rDialog()
{
	delete [] m_infostack;
	delete m_dialog;
}

void Frei0rDialog::show()
{
	m_dialog->show();
}

int Frei0rDialog::shown()
{
	return m_dialog->shown();
}

void Frei0rDialog::setDouble( int num, double val )
{
	f0r_param_double dvalue;
	dvalue = val;
	m_effect->setValue( &dvalue, num );
	cout << "setDouble " << dvalue << endl;
	g_videoView->redraw();
}
void Frei0rDialog::setBool( int num, bool val )
{
	f0r_param_bool bvalue;
	bvalue = val;
	m_effect->setValue( &bvalue, num );
	g_videoView->redraw();
}
void Frei0rDialog::setPositionX( int num, double val )
{
	f0r_param_position_t pos;
	pos.y = m_infostack[num].y;
	pos.x = val;
	m_effect->setValue( &pos, num );
	g_videoView->redraw();
}
void Frei0rDialog::setPositionY( int num, double val )
{
	f0r_param_position_t pos;
	pos.x = m_infostack[num].x;
	pos.y = val;
	m_effect->setValue( &pos, num );
	g_videoView->redraw();
}


} /* namespace nle */
