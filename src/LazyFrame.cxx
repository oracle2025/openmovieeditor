/*  LazyFrame.cxx
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

#include "LazyFrame.H"

namespace nle
{

LazyFrame::LazyFrame( gavl_video_format_t *format )
{
	m_cacheable = false;
	m_converter_RGBA = 0;
	m_rgba_frame = 0;
	m_converter_target = 0;
	m_target_frame = 0;
	m_frame = 0;
	gavl_video_format_copy( &m_src_format, format );

	gavl_video_format_t rgba_format;
	gavl_video_format_copy( &rgba_format, format );
	rgba_format.pixelformat = GAVL_RGBA_32;

	if ( gavl_video_formats_equal( &rgba_format, format ) ) {
		return;
	}
	m_converter_RGBA = gavl_video_converter_create();

	gavl_video_options_t* options = gavl_video_converter_get_options( m_converter_RGBA );
	
	gavl_video_options_set_deinterlace_mode( options, GAVL_DEINTERLACE_SCALE );

	gavl_video_converter_init( m_converter_RGBA, &m_src_format, &rgba_format );

	m_rgba_frame = gavl_video_frame_create( &rgba_format );
}
LazyFrame::LazyFrame( int w, int h )
{
	m_cacheable = false;
	m_converter_RGBA = 0;
	m_rgba_frame = 0;
	m_converter_target = 0;
	m_target_frame = 0;
	m_frame = 0;

	m_src_format.frame_width  = w;
	m_src_format.frame_height = h;
	m_src_format.image_width  = w;
	m_src_format.image_height = h;
	m_src_format.pixel_width = 1;
	m_src_format.pixel_height = 1;
	m_src_format.pixelformat = GAVL_RGBA_32;
	m_src_format.interlace_mode = GAVL_INTERLACE_NONE;

}
LazyFrame::~LazyFrame()
{
	if ( m_converter_RGBA ) {
		gavl_video_converter_destroy( m_converter_RGBA );
	}
	if ( m_rgba_frame ) {
		gavl_video_frame_destroy( m_rgba_frame );
	}
	if ( m_converter_target ) {
		gavl_video_converter_destroy( m_converter_target );
	}
	if ( m_target_frame ) {
		gavl_video_frame_destroy( m_target_frame );
	}
}
gavl_video_frame_t* LazyFrame::RGBA()
{
	if ( !m_converter_RGBA ) {
		return m_frame;
	}
	gavl_video_convert( m_converter_RGBA, m_frame, m_rgba_frame );
	return m_rgba_frame;
}
gavl_video_frame_t* LazyFrame::native()
{
	return m_frame;
}
gavl_video_frame_t* LazyFrame::target()
{
	if ( !m_converter_target ) {
		return m_frame;
	}
	gavl_video_convert( m_converter_target, m_frame, m_target_frame );
	return m_target_frame;

}
gavl_video_format_t* LazyFrame::format()
{
	return &m_src_format;
}
void LazyFrame::set_target( gavl_video_format_t *format )
{
	if ( format && m_converter_target && gavl_video_formats_equal( &m_target_format, format ) ) {
		return;
	}
	if ( m_converter_target ) {
		gavl_video_converter_destroy( m_converter_target );
		m_converter_target = 0;
	}
	if ( m_target_frame ) {
		gavl_video_frame_destroy( m_target_frame );
		m_target_frame = 0;
	}
	if ( !format ) {
		return;
	}
	if ( gavl_video_formats_equal( &m_src_format, format ) ) {
		return;
	}

	gavl_video_format_copy( &m_target_format, format );

	m_converter_target = gavl_video_converter_create();

	gavl_video_options_t* options = gavl_video_converter_get_options( m_converter_target );
	
	gavl_video_options_set_deinterlace_mode( options, GAVL_DEINTERLACE_SCALE );
	
	gavl_video_converter_init( m_converter_target, &m_src_format, &m_target_format );

	m_target_frame = gavl_video_frame_create( &m_target_format );
}
float LazyFrame::alpha()
{
	return m_alpha;
}
void LazyFrame::alpha( float value )
{
	m_alpha = value;
}
void LazyFrame::put_data( gavl_video_frame_t* frame )
{
	m_frame = frame;
}


} /* namespace nle */
