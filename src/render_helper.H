/*  render_helper.H
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

#ifndef _RENDER_HELPER_H_
#define _RENDER_HELPER_H_

#include "frame_struct.h"

namespace nle
{

void halve_image( unsigned char *optr, unsigned char *iptr, int iw, int ih );

void scale_it( frame_struct* src, frame_struct* dst );
void scale_it_alpha( frame_struct* src, frame_struct* dst );
void scale_it_alpha_opaque( frame_struct* src, frame_struct* dst );
	
void blend( unsigned char* dst, unsigned char* src1, unsigned char* src2, float alpha, int len );
void blend_alpha( unsigned char* dst, unsigned char* rgb, unsigned char* rgba, float alpha, int len );

unsigned int mixChannels( float *A, float *B, float* out, unsigned int frames );

//shamelessly extracted from libquicktime
void decode_int16_to_float(void * _in, float ** out, int num_channels, int num_samples);
void decode_uint8_to_float(void * _in, float ** out, int num_channels, int num_samples);

} /* namespace nle */

#endif /* _RENDER_HELPER_H_ */

