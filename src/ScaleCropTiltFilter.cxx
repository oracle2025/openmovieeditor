/* ScaleCropTiltFilter.cxx
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

#include "ScaleCropTiltFilter.H"
#include "ScaleCropTiltDialog.H"

namespace nle
{

ScaleCropTiltFilter::ScaleCropTiltFilter()
{
	m_scale_x = 0;
	m_scale_y = 0;
	m_crop_left = 0;
	m_crop_right = 0;
	m_crop_top = 0;
	m_crop_bottom = 0;
	m_tilt_x = 0;
	m_tilt_y = 0;
	m_dialog = 0;
}
ScaleCropTiltFilter::~ScaleCropTiltFilter()
{
	if ( m_dialog ) {
		delete m_dialog;
	}
}

frame_struct* ScaleCropTiltFilter::getFrame( frame_struct* frame, int64_t )
{
	frame->tilt_x = m_tilt_x;
	frame->tilt_y = m_tilt_y;
	frame->scale_x = m_scale_x;
	frame->scale_y = m_scale_y;
	frame->crop_left = m_crop_left;
	frame->crop_right = m_crop_right;
	frame->crop_top = m_crop_top;
	frame->crop_bottom = m_crop_bottom;
	return frame;
}

IEffectDialog* ScaleCropTiltFilter::dialog()
{
	if ( !m_dialog ) {
		m_dialog = new ScaleCropTiltDialog( this );
	}
	return m_dialog;
}
	
} /* namespace nle */
