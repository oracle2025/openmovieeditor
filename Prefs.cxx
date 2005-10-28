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

#include <stdlib.h>
#include <strlcpy.h>

#include <tinyxml.h>

#include "Prefs.H"
#include "globals.H"

namespace nle
{

Prefs* g_preferences;
static char preferences_filename[1024];

Prefs::Prefs()
{
	strcpy( preferences_filename, "" );
	if ( !getenv("HOME") ) {
		return;
	}
	strlcpy( preferences_filename, getenv("HOME"), sizeof(preferences_filename) );
	strncat( preferences_filename, "/.openme.prefs", sizeof(preferences_filename) - strlen(preferences_filename) - 1 );
	g_preferences = this;
	strlcpy( m_browserFolder, getenv("HOME"), sizeof(m_browserFolder) );
	TiXmlDocument doc( preferences_filename );
	if ( !doc.LoadFile() ) {
		return;
	}

	TiXmlHandle docH( &doc );
	TiXmlText* text = docH.FirstChildElement( "browserFolder" ).Child( 0 ).Text();
	if ( text ) {
		strlcpy( m_browserFolder, text->Value(), sizeof(m_browserFolder) );
	}

}

Prefs::~Prefs()
{
	if ( strlen( preferences_filename ) == 0 ) {
		return;
	}
	TiXmlDocument doc( preferences_filename );
	TiXmlDeclaration *dec = new TiXmlDeclaration( "1.0", "", "no" );
	doc.LinkEndChild( dec );
	
	TiXmlElement *item = new TiXmlElement( "version" );
	doc.LinkEndChild( item );
	TiXmlText* text = new TiXmlText( OME_VERSION );
	item->LinkEndChild( text );
	
	item = new TiXmlElement( "browserFolder" );
	doc.LinkEndChild( item );
	text = new TiXmlText( m_browserFolder );
	item->LinkEndChild( text );

	doc.SaveFile();
}
const char* Prefs::getBrowserFolder()
{
	return m_browserFolder;
}
void Prefs::setBrowserFolder( const char* filename )
{
	strncpy( m_browserFolder, filename, sizeof(m_browserFolder) - 1 );
	m_browserFolder[sizeof(m_browserFolder)-1] = '\0';
	// ^ strncpy is obviously broken, get strlcpy !
}

} /* namespace nle */

