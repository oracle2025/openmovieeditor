/*  NodeFilterBezierCurveFactoryPlugin.H
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

#ifndef _NODE_FILTER_BEZIER_CURVE_FACTORY_PLUGIN_H_
#define _NODE_FILTER_BEZIER_CURVE_FACTORY_PLUGIN_H_

#include "INodeFilterFactoryPlugin.H"

class NodeFilterBezierCurveFactoryPlugin : public INodeFilterFactoryPlugin
{
	public:
		NodeFilterBezierCurveFactoryPlugin();
		~NodeFilterBezierCurveFactoryPlugin();
		INode* get_i_node( int w, int h );
		const char* name() { return "Bezier Color Curve"; }
		const char* identifier() { return "effect:builtin:BezierCurveFilter"; }
};

#endif /* _NODE_FILTER_BEZIER_CURVE_FACTORY_PLUGIN_H_ */
