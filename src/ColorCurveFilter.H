/* ColorCurveFilter.H
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

#ifndef _COLOR_CURVE_FILTER_H_
#define _COLOR_CURVE_FILTER_H_

#include "IVideoEffect.H"

namespace nle
{

class ColorCurveFilter : public IVideoEffect
{
	public:
		ColorCurveFilter( int w, int h );
		~ColorCurveFilter();
		frame_struct* getFrame( frame_struct* frame, int64_t position = -1 );
		const char* name();
		int numParams() { return 1; }
		IEffectDialog* dialog();
		IEffectDialog* m_dialog;
		unsigned char m_values[256];
	private:
		unsigned char *m_frame;
		unsigned char **m_rows;
		frame_struct m_framestruct;
};

	
} /* namespace nle */

#endif /* _COLOR_CURVE_FILTER_H_ */
