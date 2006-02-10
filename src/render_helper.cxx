/*  render_helper.cxx
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

#include <gavl/gavl.h>

#include "global_includes.H"
#include "render_helper.H"

namespace nle
{
void scale_it( frame_struct* src, frame_struct* dst )
{

	gavl_rectangle_t src_rect;
	gavl_rectangle_t dst_rect;
	gavl_video_format_t format_src;
	gavl_video_format_t format_dst;

	gavl_video_frame_t * frame_src, * frame_dst;

	frame_src = gavl_video_frame_create( 0 );
	frame_dst = gavl_video_frame_create( 0 );

	frame_src->strides[0] = src->w * 3;
	frame_src->planes[0] = src->RGB;
	
	frame_dst->strides[0] = dst->w * 3;
	frame_dst->planes[0] = dst->RGB;


	gavl_video_scaler_t *scaler;
	scaler = gavl_video_scaler_create();
	gavl_video_options_set_scale_mode( gavl_video_scaler_get_options( scaler ), GAVL_SCALE_AUTO );
	

	format_dst.frame_width  = dst->w;
	format_dst.frame_height = dst->h;
	format_dst.image_width  = dst->w;
	format_dst.image_height = dst->h;;
	format_dst.pixel_width = 1;
	format_dst.pixel_height = 1;
	format_dst.colorspace = GAVL_RGB_24;
	
	format_src.frame_width  = src->w;
	format_src.frame_height = src->h;
	format_src.image_width  = src->w;
	format_src.image_height = src->h;;
	format_src.pixel_width = 1;
	format_src.pixel_height = 1;
	format_src.colorspace = GAVL_RGB_24;

	src_rect.x = 0;
	src_rect.y = 0;
	src_rect.w = src->w;
	src_rect.h = src->h;

	dst_rect.x = 0;
	dst_rect.y = 0;
	dst_rect.w = dst->w;
	dst_rect.h = dst->h;

	
	if ( gavl_video_scaler_init( scaler, GAVL_RGB_24, &src_rect, &dst_rect, &format_src, &format_dst ) == -1 ) {
		cerr << "Video Scaler Init failed" << endl;
		return;
	}
	gavl_video_scaler_scale( scaler, frame_src, frame_dst );

	frame_src->planes[0] = 0;
	frame_dst->planes[0] = 0;
	gavl_video_frame_destroy( frame_src );
	gavl_video_frame_destroy( frame_dst );

	gavl_video_scaler_destroy( scaler );


}
// inspired by rasterman, but poorly done ;)
void blend( unsigned char* dst, unsigned char* src1, unsigned char* src2, float alpha, int len )
{
	unsigned char *ps1, *ps2, *pd, *pd_end;
	unsigned int a = (unsigned char)( alpha * 255 );
	unsigned char tmp;
	ps1 = src1;
	ps2 = src2;
	pd = dst;
	pd_end = dst + ( len * 3 );
	while ( pd < pd_end ) {
		pd[0] = ( ( ( ps1[0] - ps2[0] ) * a ) >> 8 ) + ps2[0];
		ps1++;
		ps2++;
		pd++;
	};
}
	
} /* namespace nle */

