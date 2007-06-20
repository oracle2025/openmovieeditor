/*  frame_struct.h
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

#ifndef _FRAME_STRUCT_H_
#define _FRAME_STRUCT_H_

#include <stdint.h>

namespace nle
{
	
typedef enum {
	RENDER_FIT = 0,
	RENDER_CROP,
	RENDER_STRETCH,
	RENDER_DEFAULT
} render_strategy_t;

typedef struct
{
	int x, y, w, h;
	unsigned char *RGB;
	unsigned char *YUV;
	unsigned char **rows;
	int64_t nr;
	float alpha;
	bool has_alpha_channel;
	bool cacheable;
	render_strategy_t render_strategy;
	float aspect;
	int pixel_w;
	int pixel_h;
	int analog_blank; // Should be 10 pixels for 720x576
	int interlace_mode;
	bool first_field; // should be true if first field is to be displayed
	int scale_x;
	int scale_y;
	int crop_left;
	int crop_right;
	int crop_top;
	int crop_bottom;
	int tilt_x;
	int tilt_y;
} frame_struct;

} /* namespace nle */

#endif /* _FRAME_STRUCT_H_ */

