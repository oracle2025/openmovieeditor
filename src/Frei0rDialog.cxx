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
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Return_Button.H>
#include <FL/Fl_Spinner.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Slider.H>
#include <FL/Fl_Pixmap.H>

#include "Frei0rDialog.H"
#include "Frei0rEffect.H"
#include "frei0r.h"
#include "globals.H"
#include "VideoViewGL.H"
#include "global_includes.H"
#include "frei0r.xpm"

namespace nle
{

static Fl_Pixmap icon_frei0r( frei0r_xpm );

static void doubleCallback( Fl_Widget* i, void* v )
{
	Fl_Slider* vi = dynamic_cast<Fl_Slider*>(i);
	callback_info* info = (callback_info*)v;
	info->dialog->setDouble( info->number, vi->value() );
}
static void boolCallback( Fl_Widget* i, void* v )
{
	Fl_Check_Button* vi = dynamic_cast<Fl_Check_Button*>(i);
	callback_info* info = (callback_info*)v;
	info->dialog->setDouble( info->number, vi->value() );
}
static void xCallback( Fl_Widget* i, void* v )
{
	Fl_Spinner* vi = dynamic_cast<Fl_Spinner*>(i);
	callback_info* info = (callback_info*)v;
	info->dialog->setPositionX( info->number, vi->value() );
}
static void yCallback( Fl_Widget* i, void* v )
{
	Fl_Spinner* vi = dynamic_cast<Fl_Spinner*>(i);
	callback_info* info = (callback_info*)v;
	info->dialog->setPositionY( info->number, vi->value() );
}
static void closeCallback( Fl_Widget*, void* data ) {
//	i->window()->hide();
	Frei0rDialog* dlg = (Frei0rDialog*)data;
	delete dlg;
}

Frei0rDialog::Frei0rDialog( Frei0rEffect* effect )
	: m_effect( effect )
{
	f0r_plugin_info_t* finfo;
	f0r_param_info_t pinfo;
	
	finfo = m_effect->getPluginInfo();
	
	int height = 30 * finfo->num_params + 5 + 60 + 35;

	m_infostack = new callback_info[finfo->num_params];
	
	m_dialog = new Fl_Double_Window( 340, height, "Frei0r Effect" );
	{
		Fl_Box* o = new Fl_Box( 0, 0, 340, 55, finfo->name );
		o->labelfont( 1 );
		o->labelsize( 16 );
	}
	{
		Fl_Box* o = new Fl_Box(5, 5, 44, 44);
		o->box(FL_DOWN_BOX);
		o->image(icon_frei0r);
		o->tooltip( finfo->explanation );
	}
	{
		Fl_Box* o = new Fl_Box( 5, 50, 330, 30 * finfo->num_params + 15 );
		o->box( FL_ENGRAVED_FRAME );
	}
	{
		Fl_Return_Button* o = new Fl_Return_Button( 5, 60 + ( 30 * finfo->num_params ) + 10, 330, 25, "Close" );
		o->callback( closeCallback, this );
		m_dialog->hotspot( o );
	}
	for ( int i = 0; i < finfo->num_params; i++ ) {
		m_effect->getParamInfo( &pinfo, i );
		m_infostack[i].dialog = this;
		m_infostack[i].number = i;
		int x, y, w, h;
		w = 225;
		h = 25;
		x = 100;
		y = 5 + ( i * 30 ) + 55;
		switch ( pinfo.type ) {
			case F0R_PARAM_DOUBLE: //Seems to be always between 0.0 and 1.0
				{
				Fl_Slider* o = new Fl_Slider( x, y, w, h, pinfo.name );
				o->type( 5 );
				o->callback( doubleCallback, &(m_infostack[i]) );
				o->align(FL_ALIGN_LEFT);
				o->tooltip( pinfo.explanation );
				f0r_param_double dvalue;
				m_effect->getValue( &dvalue, i );
				o->value( dvalue );
				break;
				}
			case F0R_PARAM_BOOL:
				{
				Fl_Check_Button* b = new Fl_Check_Button( x, y, w, h, pinfo.name );
				b->callback( boolCallback, &(m_infostack[i]) );
				b->tooltip( pinfo.explanation );
				f0r_param_bool bvalue;
				m_effect->getValue( &bvalue, i );
				b->value( ( bvalue >= 0.5 ) );
				break;
				}
			case F0R_PARAM_COLOR:
				break;
			case F0R_PARAM_POSITION:
				{
				Fl_Spinner* sx = new Fl_Spinner( x, y, 150, h, pinfo.name);
				Fl_Spinner* sy = new Fl_Spinner( x + 150, y, 150, h );
				sx->callback( xCallback, &(m_infostack[i]) );
				sy->callback( yCallback, &(m_infostack[i]) );
				break;
				}
			default:
				break;
		};
	}
	m_dialog->set_non_modal();
	m_dialog->end();
}

Frei0rDialog::~Frei0rDialog()
{
	m_effect->m_dialog = 0;
	m_dialog->hide();
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
