/* ScaleCropTiltFilter.H
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

#ifndef _SCALE_CROP_TILT_FILTER_H_
#define _SCALE_CROP_TILT_FILTER_H_

#include "IVideoEffect.H"

namespace nle
{

class ScaleCropTiltFilter : public IVideoEffect
{
	public:
		ScaleCropTiltFilter();
		~ScaleCropTiltFilter();
		frame_struct* getFrame( frame_struct* frame, int64_t position = -1 );
		const char* name() { return "Scale Crop Tilt"; }
		int numParams() { return 1; }
		IEffectDialog* dialog();
		IEffectDialog* m_dialog;
		const char* identifier() { return "effect:builtin:ScaleCropTilt"; }
		FilterData* getFilterData() { return 0; }
		void setFilterData( FilterData* ) {}
		int m_scale_x;
		int m_scale_y;
		int m_crop_left;
		int m_crop_right;
		int m_crop_top;
		int m_crop_bottom;
		int m_tilt_x;
		int m_tilt_y;

/*		void writeXML( TiXmlElement* );
		void readXML( TiXmlElement* );*/
};
	
} /* namespace nle */

#endif /* _SCALE_CROP_TILT_FILTER_H_ */

