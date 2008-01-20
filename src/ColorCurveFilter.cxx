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

#include <tinyxml.h>

#include "ColorCurveFilter.H"
#include "ColorCurveDialog.H"
#include "helper.H"

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
	m_framestruct.tilt_x = 0;
	m_framestruct.tilt_y = 0;
	m_framestruct.scale_x = 1.0;
	m_framestruct.scale_y = 1.0;
	m_framestruct.crop_left = 0;
	m_framestruct.crop_right = 0;
	m_framestruct.crop_top = 0;
	m_framestruct.crop_bottom = 0;
	for ( unsigned int i = 0; i < 256; i++ ) {
		m_values[i] = i;
		m_values_r[i] = i;
		m_values_g[i] = i;
		m_values_b[i] = i;
	}
	m_dialog = 0;

	m_parameters.r.p1.x = 0;
	m_parameters.r.p1.y = 255;
	m_parameters.r.p2.x = 255;
	m_parameters.r.p2.y = 0;

	m_parameters.g = m_parameters.b = m_parameters.m = m_parameters.r;
	m_bypass = false;
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
	if ( m_bypass ) {
		return frame;
	}
	copy_frame_struct_props( frame, &m_framestruct );
	unsigned int len = m_framestruct.w * m_framestruct.h;
	unsigned char* dst = m_frame;
	unsigned char* src = frame->RGB;
	if ( frame->has_alpha_channel ) {
		m_framestruct.has_alpha_channel = true;
		while (len--)
		{
			*dst++ = m_values_r[*src++];
			*dst++ = m_values_g[*src++];
			*dst++ = m_values_b[*src++];
			*dst++ = *src++; // copy alpha
		}
	} else {
		m_framestruct.has_alpha_channel = false;
		while (len--)
		{
			*dst++ = m_values_r[*src++];
			*dst++ = m_values_g[*src++];
			*dst++ = m_values_b[*src++];
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
void ColorCurveFilter::calculate_values( unsigned char* values, struct color_curve_desc* desc )
{
	for ( int i = 0; i < desc->p1.x && i < 256; i++ ) {
		values[i] = 255 - desc->p1.y;
	}
	int A = desc->p2.x - desc->p1.x;
	int B = desc->p1.y - desc->p2.y;
	float C = (float)B / (float)A;
	for ( int i = desc->p1.x; i < desc->p2.x && i < 256; i++ ) {
		values[i] = 255 - desc->p1.y + (int)( (i-desc->p1.x) * C );
	}
	for ( int i = desc->p2.x; i < 256; i++ ) {
		values[i] = 255 - desc->p2.y;
	}
}
void ColorCurveFilter::calculate_values()
{
	unsigned char val_r[256];
	unsigned char val_g[256];
	unsigned char val_b[256];
	unsigned char val_m[256];
	
	calculate_values( val_r, &(m_parameters.r) );
	calculate_values( val_g, &(m_parameters.g) );
	calculate_values( val_b, &(m_parameters.b) );
	calculate_values( val_m, &(m_parameters.m) );
	
	unsigned char* red = val_r;
	unsigned char* green = val_g;
	unsigned char* blue = val_b;
	unsigned char* master = val_m;

	unsigned char* out_r = m_values_r;
	unsigned char* out_g = m_values_g;
	unsigned char* out_b = m_values_b;


	for ( int i = 0; i < 256; i++ ) {
		out_r[i] = master[red[i]];
		out_g[i] = master[green[i]];
		out_b[i] = master[blue[i]];
	}


}
void ColorCurveFilter::writeXML( TiXmlElement* xml_node )
{
	xml_node->SetAttribute( "r_p1_x", m_parameters.r.p1.x );
	xml_node->SetAttribute( "r_p1_y", m_parameters.r.p1.y );
	xml_node->SetAttribute( "r_p2_x", m_parameters.r.p2.x );
	xml_node->SetAttribute( "r_p2_y", m_parameters.r.p2.y );

	xml_node->SetAttribute( "g_p1_x", m_parameters.g.p1.x );
	xml_node->SetAttribute( "g_p1_y", m_parameters.g.p1.y );
	xml_node->SetAttribute( "g_p2_x", m_parameters.g.p2.x );
	xml_node->SetAttribute( "g_p2_y", m_parameters.g.p2.y );

	xml_node->SetAttribute( "b_p1_x", m_parameters.b.p1.x );
	xml_node->SetAttribute( "b_p1_y", m_parameters.b.p1.y );
	xml_node->SetAttribute( "b_p2_x", m_parameters.b.p2.x );
	xml_node->SetAttribute( "b_p2_y", m_parameters.b.p2.y );

	xml_node->SetAttribute( "m_p1_x", m_parameters.m.p1.x );
	xml_node->SetAttribute( "m_p1_y", m_parameters.m.p1.y );
	xml_node->SetAttribute( "m_p2_x", m_parameters.m.p2.x );
	xml_node->SetAttribute( "m_p2_y", m_parameters.m.p2.y );
	
}
void ColorCurveFilter::readXML( TiXmlElement* xml_node )
{
	xml_node->Attribute( "r_p1_x", &m_parameters.r.p1.x );
	xml_node->Attribute( "r_p1_y", &m_parameters.r.p1.y );
	xml_node->Attribute( "r_p2_x", &m_parameters.r.p2.x );
	xml_node->Attribute( "r_p2_y", &m_parameters.r.p2.y );

	xml_node->Attribute( "g_p1_x", &m_parameters.g.p1.x );
	xml_node->Attribute( "g_p1_y", &m_parameters.g.p1.y );
	xml_node->Attribute( "g_p2_x", &m_parameters.g.p2.x );
	xml_node->Attribute( "g_p2_y", &m_parameters.g.p2.y );

	xml_node->Attribute( "b_p1_x", &m_parameters.b.p1.x );
	xml_node->Attribute( "b_p1_y", &m_parameters.b.p1.y );
	xml_node->Attribute( "b_p2_x", &m_parameters.b.p2.x );
	xml_node->Attribute( "b_p2_y", &m_parameters.b.p2.y );

	xml_node->Attribute( "m_p1_x", &m_parameters.m.p1.x );
	xml_node->Attribute( "m_p1_y", &m_parameters.m.p1.y );
	xml_node->Attribute( "m_p2_x", &m_parameters.m.p2.x );
	xml_node->Attribute( "m_p2_y", &m_parameters.m.p2.y );
	calculate_values();
}

} /* namespace nle */
