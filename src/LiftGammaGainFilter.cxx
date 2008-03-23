/* LiftGammaGainFilter.cxx
 *
 *  Copyright (C) 2008 Richard Spindler <richard.spindler AT gmail.com>
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

#include "LiftGammaGainFilter.H"
#include "LiftGammaGainDialog.H"
#include "helper.H"

#include <tinyxml.h>
#include <cmath>

namespace nle
{

LiftGammaGainFilter::LiftGammaGainFilter( int w, int h )
{
	m_frame = new unsigned char[w * h * 4];
	m_rows = new unsigned char*[h];
	for (int i = 0; i < h; i++) {
                m_rows[i] = m_frame + w * 4 * i;
	}
	init_frame_struct( &m_framestruct, w, h );
	m_framestruct.RGB = m_frame;
	m_framestruct.YUV = 0;
	m_framestruct.rows = m_rows;
	m_framestruct.has_alpha_channel = true;
	m_framestruct.cacheable = false;
	for ( unsigned int i = 0; i < 256; i++ ) {
		m_red[i] = i;
		m_green[i] = i;
		m_blue[i] = i;
	}
	m_lift[0]  = m_lift[1]  = m_lift[2]  = 1.0;
	m_gamma[0] = m_gamma[1] = m_gamma[2] = 1.0;
	m_gain[0]  = m_gain[1]  = m_gain[2]  = 1.0;
	m_dialog = 0;
	m_bypass = false;

	m_parameters.lift.x  = m_parameters.lift.y  = 128.0;
	m_parameters.gamma.x = m_parameters.gamma.y = 128.0;
	m_parameters.gain.x  = m_parameters.gain.y  = 128.0;
}
LiftGammaGainFilter::~LiftGammaGainFilter()
{
	delete m_rows;
	delete m_frame;
	if ( m_dialog ) {
		delete m_dialog;
	}
}
frame_struct* LiftGammaGainFilter::getFrame( frame_struct* frame, int64_t position )
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
			*dst++ = m_red[*src++];
			*dst++ = m_green[*src++];
			*dst++ = m_blue[*src++];
			*dst++ = *src++; // copy alpha
		}
	} else {
		m_framestruct.has_alpha_channel = false;
		while (len--)
		{
			*dst++ = m_red[*src++];
			*dst++ = m_green[*src++];
			*dst++ = m_blue[*src++];
		}
	}
	return &m_framestruct;
}
const char* LiftGammaGainFilter::name()
{
	return "Lift Gamma Gain";
}
IEffectDialog* LiftGammaGainFilter::dialog()
{
	if ( !m_dialog ) {
		m_dialog = new LiftGammaGainDialog( this );
	}
	return m_dialog;

}
void LiftGammaGainFilter::writeXML( TiXmlElement* xml_node )
{
	xml_node->SetDoubleAttribute( "lift_x", m_parameters.lift.x );
	xml_node->SetDoubleAttribute( "lift_y", m_parameters.lift.y );
	xml_node->SetDoubleAttribute( "gamma_x", m_parameters.gamma.x );
	xml_node->SetDoubleAttribute( "gamma_y", m_parameters.gamma.y );
	xml_node->SetDoubleAttribute( "gain_x", m_parameters.gain.x );
	xml_node->SetDoubleAttribute( "gain_y", m_parameters.gain.y );
}
void LiftGammaGainFilter::readXML( TiXmlElement* xml_node )
{
	xml_node->Attribute( "lift_x", &m_parameters.lift.x );
	xml_node->Attribute( "lift_y", &m_parameters.lift.y );
	xml_node->Attribute( "gamma_x", &m_parameters.gamma.x );
	xml_node->Attribute( "gamma_y", &m_parameters.gamma.y );
	xml_node->Attribute( "gain_x", &m_parameters.gain.x );
	xml_node->Attribute( "gain_y", &m_parameters.gain.y );
}

void LiftGammaGainFilter::lift( float r, float g, float b )
{
	m_lift[0] = r;
	m_lift[1] = g;
	m_lift[2] = b;
}
void LiftGammaGainFilter::gamma( float r, float g, float b )
{
	m_gamma[0] = r;
	m_gamma[1] = g;
	m_gamma[2] = b;
}
void LiftGammaGainFilter::gain( float r, float g, float b )
{
	m_gain[0] = r;
	m_gain[1] = g;
	m_gain[2] = b;
}


static float red( float* a ) {
	return a[0];
}
static float green( float* a ) {
	return a[1];
}
static float blue( float* a ) {
	return a[2];
}
static int f_to_i( float in ) {
	return (int)( in * 255 );
}
static float i_to_f( int in ) {
	return (float)in / 255.0;
}
static int clamp_255( int in ) {
	int out;
	if ( in < 0 ) {
		return 0;
	}
	if ( in > 255 ) {
		return 255;
	}
	return in;
}

void LiftGammaGainFilter::calculate_values()
{
	// r goes from 0.0 to 1.0
	float gain, lift, gamma;
	bool m_constant_green = false;
	if ( m_constant_green ) {
		for ( unsigned int i = 0; i < 256; i++ ) {
			/* Red */
			/* Cyan => Rv */
			/* Magenta => R^ B^ */
			gamma = 1.0 / ( 1.0 - ( green( m_gamma ) - red( m_gamma ) ) );
			gain = 1.0 - (green( m_gain ) - red( m_gain ));
			lift = red( m_lift ) - green( m_lift );
			m_red[i] = clamp_255(f_to_i(  pow( (( i_to_f(i) * gain ) + lift ), gamma ) ) );

			/* Blue */
			/* Magenta => R^ B^ */
			/* Yellow => Bv */
			gamma = 1.0 / ( 1.0 - ( green( m_gamma ) - blue( m_gamma ) ) );
			gain = 1.0 - (green( m_gain ) - blue( m_gain ));
			lift = blue( m_lift ) - green( m_lift );
			m_blue[i] = clamp_255(f_to_i(  pow( (( i_to_f(i) * gain ) + lift ), gamma ) ) );
		}
	} else {
		for ( unsigned int i = 0; i < 256; i++ ) {
			/* Red */
			/* Cyan => Rv */
			/* Magenta => R^ B^ */
			gamma = 1.0 / red( m_gamma );
			gain = red( m_gain );
			lift = red( m_lift ) - 1.0;
			m_red[i] = clamp_255(f_to_i(  pow( (( i_to_f(i) * gain ) + lift ), gamma ) ) );

			/* Blue */
			/* Magenta => R^ B^ */
			/* Yellow => Bv */
			gamma = 1.0 / blue( m_gamma );
			gain = blue( m_gain );
			lift = blue( m_lift ) - 1.0;
			m_blue[i] = clamp_255(f_to_i(  pow( (( i_to_f(i) * gain ) + lift ), gamma ) ) );

			gamma = 1.0 / green( m_gamma );
			gain = green( m_gain );
			lift = green( m_lift ) - 1.0;
			m_green[i] = clamp_255(f_to_i(  pow( (( i_to_f(i) * gain ) + lift ), gamma ) ) );
		}
	}
}

} /* namespace nle */
