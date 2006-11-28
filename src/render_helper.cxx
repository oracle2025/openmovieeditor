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
	
/*
  optr is a pointer to a place to store the output image.
  iptr is a pointer to the input image.
  iw and ih are the width and height of the input image respectively.
*/

void halve_image ( unsigned char *optr,
                   unsigned char *iptr,
                   int iw, int ih )
{
/*	for ( int i = 0; i < ih >> 1; i++ ) {
		for ( int j = 0; j < iw; j++ ) {
			*optr++ = *iptr++;
		}
	}
	return;*/
  int ow   = iw >> 1 ;
  int oh   = ih >> 1 ;
  int iw3  = iw * 3 ;     /* Offset to get to the pixel below */
  int iw33 = iw * 3 + 3 ; /* Offset to get to the pixel below/right */

  for ( int i = 0 ; i < oh ; i++ )
  {
    for ( int j = 0 ; j < ow ; j++ )
    {
      /* Average red/green/blue for each pixel */
      *optr++ = ( *iptr + *(iptr + 3) + *(iptr + iw3) + *(iptr + iw33) ) >> 2; iptr++;
      *optr++ = ( *iptr + *(iptr + 3) + *(iptr + iw3) + *(iptr + iw33) ) >> 2; iptr++;
      *optr++ = ( *iptr + *(iptr + 3) + *(iptr + iw3) + *(iptr + iw33) ) >> 2; iptr++;
      iptr += 3 ;  /* Skip to the next pixel */
    }

    iptr += iw3 ;  /* Skip to the next row of pixels */
  }
}

static void scale_it_raw( frame_struct* src, frame_struct* dst, gavl_pixelformat_t colorspace, gavl_pixelformat_t colorspace_dst, int bits )
{
	gavl_rectangle_i_t src_rect;
	gavl_rectangle_i_t dst_rect;
	gavl_video_format_t format_src;
	gavl_video_format_t format_dst;

	gavl_video_frame_t * frame_src, * frame_dst;

	frame_src = gavl_video_frame_create( 0 );
	frame_dst = gavl_video_frame_create( 0 );

	frame_src->strides[0] = src->w * bits;
	frame_src->planes[0] = src->RGB;
	
	frame_dst->strides[0] = dst->w * bits;
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
	format_dst.pixelformat = colorspace_dst;
	
	format_src.frame_width  = src->w;
	format_src.frame_height = src->h;
	format_src.image_width  = src->w;
	format_src.image_height = src->h;;
	format_src.pixel_width = 1;
	format_src.pixel_height = 1;
	format_src.pixelformat = colorspace;

	src_rect.x = 0;
	src_rect.y = 0;
	src_rect.w = src->w;
	src_rect.h = src->h;

	dst_rect.x = 0;
	dst_rect.y = 0;
	dst_rect.w = dst->w;
	dst_rect.h = dst->h;

	
	if ( gavl_video_scaler_init( scaler, &format_src, &format_dst ) == -1 ) {
		cerr << "Video Scaler Init failed" << endl;
		return;
	}
	gavl_video_scaler_scale( scaler, frame_src, frame_dst );

	gavl_video_frame_null( frame_src );
	gavl_video_frame_null( frame_dst );
	gavl_video_frame_destroy( frame_src );
	gavl_video_frame_destroy( frame_dst );

	gavl_video_scaler_destroy( scaler );

}
#if 0
static void scale_it_raw( frame_struct* src, frame_struct* dst, gavl_colorspace_t colorspace, int bits )
{
	gavl_rectangle_t src_rect;
	gavl_rectangle_t dst_rect;
	gavl_video_format_t format_src;
	gavl_video_format_t format_dst;

	gavl_video_frame_t * frame_src, * frame_dst;

	frame_src = gavl_video_frame_create( 0 );
	frame_dst = gavl_video_frame_create( 0 );

	frame_src->strides[0] = src->w * bits;
	frame_src->planes[0] = src->RGB;
	
	frame_dst->strides[0] = dst->w * bits;
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
	format_dst.colorspace = colorspace;
	
	format_src.frame_width  = src->w;
	format_src.frame_height = src->h;
	format_src.image_width  = src->w;
	format_src.image_height = src->h;;
	format_src.pixel_width = 1;
	format_src.pixel_height = 1;
	format_src.colorspace = colorspace;

	src_rect.x = 0;
	src_rect.y = 0;
	src_rect.w = src->w;
	src_rect.h = src->h;

	dst_rect.x = 0;
	dst_rect.y = 0;
	dst_rect.w = dst->w;
	dst_rect.h = dst->h;

	
	if ( gavl_video_scaler_init( scaler, colorspace, &src_rect, &dst_rect, &format_src, &format_dst ) == -1 ) {
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
#endif
void scale_it_alpha_opaque( frame_struct* src, frame_struct* dst )
{
	scale_it_raw( src, dst, GAVL_RGBA_32, GAVL_RGB_24, 4 );
}
void scale_it_alpha( frame_struct* src, frame_struct* dst )
{
	scale_it_raw( src, dst, GAVL_RGBA_32, GAVL_RGBA_32, 4 );
}
void scale_it( frame_struct* src, frame_struct* dst )
{
	scale_it_raw( src, dst, GAVL_RGB_24, GAVL_RGB_24, 3 );
}
// inspired by rasterman, but poorly done ;)
void blend( unsigned char* dst, unsigned char* src1, unsigned char* src2, float alpha, int len )
{
	unsigned char *ps1, *ps2, *pd, *pd_end;
	unsigned int a = (unsigned char)( alpha * 255 );
	ps1 = src1;
	ps2 = src2;
	pd = dst;
	pd_end = dst + ( len * 3 );
	while ( pd < pd_end ) {
		pd[0] = ( ( ( ps1[0] - ps2[0] ) * a ) >> 8 ) + ps2[0];
		ps1++;
		ps2++;
		pd++;
	}
}
void blend_alpha( unsigned char* dst, unsigned char* rgb, unsigned char* rgba, float alpha, int len )
{
	unsigned char *ps1, *ps2, *pd, *pd_end;
	unsigned int a = (unsigned char)( alpha * 255 );
	ps1 = rgba;
	ps2 = rgb;
	pd = dst;
	pd_end = dst + ( len * 3 );
	while ( pd < pd_end ) {
		pd[0] = ( ( ( ps1[0] - ps2[0] ) * a * ps1[3] ) >> 16 ) + ps2[0];
		pd[1] = ( ( ( ps1[1] - ps2[1] ) * a * ps1[3] ) >> 16 ) + ps2[1];
		pd[2] = ( ( ( ps1[2] - ps2[2] ) * a * ps1[3] ) >> 16 ) + ps2[2];
		ps1 += 4;
		ps2 += 3;
		pd += 3;
	}
}

unsigned int mixChannels( float *A, float *B, float* out, unsigned int frames )
{
	float *p_output = out;
	float *p_A = A;
	float *p_B = B;
	for ( unsigned int i = frames * 2; i > 0; i-- ){
		*p_output = *p_A + *p_B;
		p_output++;
		p_A++;
		p_B++;
	}
	return frames;
}
#define UINT8_TO_FLOAT(src, dst) dst = (float)src / 127.0 - 1.0
#define INT16_TO_FLOAT(src, dst) dst = (float)src / 32768.0
void decode_int16_to_float(void * _in, float ** out, int num_channels, int num_samples)
  {
  int i, j;
  int16_t * in;
  for(i = 0; i < num_channels; i++)
    {
    if(out[i])
      {
      in = ((int16_t*)_in) + i;
      for(j = 0; j < num_samples; j++)
        {
        INT16_TO_FLOAT((*in), out[i][j]);
        in += num_channels;
        }
      }
    }
  }

// useless, because it wont do interleaved sample, and this is what it's all
// about.
void decode_uint8_to_float(void * _in, float ** out, int num_channels, int num_samples)
  {
  int i, j;
  uint8_t * in;
  for(i = 0; i < num_channels; i++)
    {
    if(out[i])
      {
      in = ((uint8_t*)_in) + i;
      for(j = 0; j < num_samples; j++)
        {
        UINT8_TO_FLOAT((*in), out[i][j]);
        in += num_channels;
        }
      }
    }
  }

} /* namespace nle */

