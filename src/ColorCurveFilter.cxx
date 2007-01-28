/* ColorCurveFilter.cxx
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

#include "ColorCurveFilter.H"
#include "ColorCurveDialog.H"


namespace nle
{

ColorCurveFilter::ColorCurveFilter( int w, int h )
{
	m_frame = new unsigned char[w * h * 4];
	m_rows = new unsigned char*[h];
	for (int i = 0; i < h; i++) {
                m_rows[i] = m_frame + w * 4 * i;
	}
	m_framestruct.x = 0;
	m_framestruct.y = 0;
	m_framestruct.w = w;
	m_framestruct.h = h;
	m_framestruct.RGB = m_frame;
	m_framestruct.YUV = 0;
	m_framestruct.rows = m_rows;
	m_framestruct.alpha = 1.0;
	m_framestruct.has_alpha_channel = true;
	m_framestruct.cacheable = false;
	for ( unsigned int i = 0; i < 256; i++ ) {
		m_values[i] = i;
	}
	m_dialog = 0;
}

ColorCurveFilter::~ColorCurveFilter()
{
	delete m_rows;
	delete m_frame;
	if ( m_dialog ) {
		delete m_dialog;
	}
}
frame_struct* ColorCurveFilter::getFrame( frame_struct* frame, int64_t )
{
	unsigned int len = m_framestruct.w * m_framestruct.h;
	unsigned char* dst = m_frame;
	unsigned char* src = frame->RGB;
	if ( frame->has_alpha_channel ) {
		m_framestruct.has_alpha_channel = true;
		while (len--)
		{
			*dst++ = m_values[*src++];
			*dst++ = m_values[*src++];
			*dst++ = m_values[*src++];
			*dst++ = *src++; // copy alpha
		}
	} else {
		m_framestruct.has_alpha_channel = false;
		while (len--)
		{
			*dst++ = m_values[*src++];
			*dst++ = m_values[*src++];
			*dst++ = m_values[*src++];
		}
	}
	return &m_framestruct;
}
const char* ColorCurveFilter::name()
{
	return "Color Curve";
}
IEffectDialog* ColorCurveFilter::dialog()
{
	if ( !m_dialog ) {
		m_dialog = new ColorCurveDialog( this );
	}
	return m_dialog;
}

} /* namespace nle */
