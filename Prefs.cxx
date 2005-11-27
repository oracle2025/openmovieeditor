/* Prefs.cxx
 *
 *  Copyright (C) 2005 Richard Spindler <richard.spindler AT gmail.com>
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

#include <cstdlib>
#include "strlcpy.h"

#include <tinyxml.h>

#include "Prefs.H"
#include "globals.H"

namespace nle
{

Prefs* g_preferences;
static string preferences_filename;

Prefs::Prefs()
{
	preferences_filename = "";
	if ( !getenv("HOME") ) {
		return;
	}
	preferences_filename += getenv( "HOME" );
	preferences_filename += "/.openme.prefs";
	g_preferences = this;
	m_browserFolder = getenv( "HOME" );
	TiXmlDocument doc( preferences_filename.c_str() );
	if ( !doc.LoadFile() ) {
		return;
	}

	TiXmlHandle docH( &doc );
	TiXmlText* text = docH.FirstChildElement( "browserFolder" ).Child( 0 ).Text();
	if ( text ) {
		m_browserFolder = text->Value();
	}

}

Prefs::~Prefs()
{
	if ( preferences_filename.length() == 0 ) {
		return;
	}
	TiXmlDocument doc( preferences_filename.c_str() );
	TiXmlDeclaration *dec = new TiXmlDeclaration( "1.0", "", "no" );
	doc.LinkEndChild( dec );
	
	TiXmlElement *item = new TiXmlElement( "version" );
	doc.LinkEndChild( item );
	TiXmlText* text = new TiXmlText( OME_VERSION );
	item->LinkEndChild( text );
	
	item = new TiXmlElement( "browserFolder" );
	doc.LinkEndChild( item );
	text = new TiXmlText( m_browserFolder.c_str() );
	item->LinkEndChild( text );

	doc.SaveFile();
}
string Prefs::getBrowserFolder()
{
	return m_browserFolder;
}
void Prefs::setBrowserFolder( string filename )
{
	m_browserFolder = filename;
}

} /* namespace nle */

