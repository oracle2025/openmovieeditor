/*  MasterEffectMenu.cxx
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

#include "MasterEffectMenu.H"
#include "global_includes.H"
#include "TimelineView.H"
#include "FilterFactory.H"
#include "FilterClip.H"
#include "FilterScroll.H"
#include <iostream>
#include "VideoEffectClip.H"

namespace nle
{

void effect_callback( Fl_Menu_* o, void* v ) {
	MasterEffectMenu* m = (MasterEffectMenu*)o;
	FilterFactory* effectFactory = (FilterFactory*)v;
	m->addMasterEffect( effectFactory );
}

MasterEffectMenu::MasterEffectMenu(  int x, int y, int w, int h, const char *l )
	: Fl_Menu_Button( x, y, w, h, l )
{
	m_filter_scroll = 0;
}	
MasterEffectMenu::~MasterEffectMenu()
{
}
void MasterEffectMenu::addEffect( FilterFactory* effectFactory )
{
	add( effectFactory->name(), 0, (Fl_Callback*)effect_callback, effectFactory, 0 );
}
void MasterEffectMenu::addMasterEffect( FilterFactory* effectFactory )
{
	if ( !m_filter_scroll ) {
		return;
	}
	FilterClip* c = m_filter_scroll->getClip();
	std::cout << c << std::endl;

	VideoEffectClip* effectClip = dynamic_cast<VideoEffectClip*>(c);

	if ( effectClip ) {
		std::cout << effectClip << " " << effectClip->w() << " " << effectClip->h() << std::endl;
	} else {
		std::cout << " SOMETHING SERIOSLY WRONG" << std::endl;
	}

	if ( !c ) {
		return;
	}
	c->appendFilter( effectFactory );
	m_filter_scroll->setClip( c );
}

} /* namespace nle */
