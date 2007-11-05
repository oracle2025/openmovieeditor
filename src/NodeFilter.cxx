/* NodeFilter.cxx
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

#include "NodeFilter.H"
#include "NodeFilterDialog.H"

namespace nle {

NodeFilter::NodeFilter( int w, int h )
{
	m_dialog = 0;
}
NodeFilter::~NodeFilter()
{
	if ( m_dialog ) {
		delete m_dialog;
	}
}
void NodeFilter::writeXML( TiXmlElement* )
{
}
void NodeFilter::readXML( TiXmlElement* )
{
}
const char* NodeFilter::name()
{
	return "Node Compositor";
}
frame_struct* NodeFilter::getFrame( frame_struct* frame, int64_t position )
{
	return frame;
}
IEffectDialog* NodeFilter::dialog()
{
	if ( !m_dialog ) {
		m_dialog = new NodeFilterDialog( this );
	}
	return m_dialog;
}

} /* namespace nle */

