/* NodeFilter.H
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

#ifndef _NODE_FILTER_H_
#define _NODE_FILTER_H_

#include "IVideoEffect.H"

namespace nle
{

class NodeFilter : public IVideoEffect
{
	public:
		NodeFilter( int w, int h );
		~NodeFilter();
		FilterData* getFilterData() { return 0; }
		void setFilterData( FilterData* ) {}
		void writeXML( TiXmlElement* );
		void readXML( TiXmlElement* );
		const char* identifier() { return "effect:builtin:ColorCurves"; }
		const char* name();
		frame_struct* getFrame( frame_struct* frame, int64_t position = -1 );
		int numParams() { return 1; }
		IEffectDialog* dialog();
		IEffectDialog* m_dialog;
	private:
};
	
} /* namespace nle */

#endif /* _NODE_FILTER_H_ */
