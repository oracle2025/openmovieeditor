/*  FilmStrip.cxx
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

#include <exception>
#include <cstring>
#include <cassert>

#include "FilmStrip.H"
#include "IVideoFile.H"
#include "TimelineView.H"
#include "SwitchBoard.H"
#include "DiskCache.H"

namespace nle
{

#define PIC_WIDTH 40 
#define PIC_HEIGHT 30

FilmStrip::FilmStrip( JobDoneListener* listener, IVideoFile* vfile )
	: Job( listener )
{
	m_decoder = 0;
	m_converter = 0;
	m_frame_src = 0;
	m_frame_dst = 0;
	m_filename = vfile->filename();
	m_cache = new DiskCache( vfile->filename(), "thumbs" );

	if ( m_cache->isEmpty() ) {
		m_decoder = bgav_create();
		bgav_options_t* decoder_options = bgav_get_options( m_decoder );
		bgav_options_set_sample_accurate( decoder_options, 1 );
		if ( !bgav_open( m_decoder, m_filename.c_str() ) ) {
			bgav_close( m_decoder );
			m_decoder = 0;
		} else if( bgav_is_redirector( m_decoder ) ) {
			bgav_close( m_decoder );
			m_decoder = 0;
		} else if ( bgav_num_video_streams( m_decoder, 0 ) == 0 ) {
			bgav_close( m_decoder );
			m_decoder = 0;
		} else if ( !bgav_can_seek_sample( m_decoder ) ) {
			bgav_close( m_decoder );
			m_decoder = 0;
		} else if ( !bgav_select_track( m_decoder, 0 ) ) {
			bgav_close( m_decoder );
			m_decoder = 0;
		} else if ( !bgav_set_video_stream( m_decoder, 0, BGAV_STREAM_DECODE ) ) {
			bgav_close( m_decoder );
			m_decoder = 0;
		} else if ( !bgav_start( m_decoder ) ) {
			bgav_close( m_decoder );
			m_decoder = 0;
		}
		assert( m_decoder );
		const gavl_video_format_t *format_src = m_format_src = bgav_get_video_format( m_decoder, 0 );
		gavl_video_format_t format_dst;
		gavl_video_format_copy( &format_dst, format_src );
		format_dst.pixelformat = GAVL_RGB_24;
		format_dst.interlace_mode = GAVL_INTERLACE_NONE;
		format_dst.image_height = format_dst.frame_height = PIC_HEIGHT;
		format_dst.image_width = format_dst.frame_width = PIC_WIDTH;
		m_frame_src = gavl_video_frame_create( format_src );
		m_frame_dst = gavl_video_frame_create( &format_dst );

		m_converter = gavl_video_converter_create();
		gavl_video_options_t* options = gavl_video_converter_get_options( m_converter );
		gavl_video_options_set_deinterlace_mode( options, GAVL_DEINTERLACE_SCALE );
		gavl_video_options_set_scale_mode( options, GAVL_SCALE_NEAREST );
		gavl_video_converter_init( m_converter, format_src, &format_dst );

		m_countAll = ( bgav_video_duration( m_decoder, 0 ) / format_src->timescale ) / 4;
	} else {
		m_countAll = m_cache->size() / ( 3 * PIC_WIDTH * PIC_HEIGHT );
	}
	
	try {
		m_pics = new pic_struct[m_countAll];
	} catch(std::exception &e) {
		cerr << "Out of Memory? " << e.what() << endl;
		m_pics = 0;
	}
	if ( !m_pics ) {
		m_countAll = 5;
		m_pics = new pic_struct[m_countAll];
	}
	m_count = 0;
}
void FilmStrip::delete_gmerlin_avdecoder()
{
	if ( m_decoder ) {
		bgav_close( m_decoder );
		m_decoder = 0;
	}
	if ( m_converter ) {
		gavl_video_converter_destroy( m_converter );
		m_converter = 0;
	}
	if ( m_frame_src ) {
		gavl_video_frame_destroy( m_frame_src );
		m_frame_src = 0;
	}
	if ( m_frame_dst ) {
		gavl_video_frame_destroy( m_frame_dst );
		m_frame_dst = 0;
	}
}
bool FilmStrip::done()
{
	if ( m_count == m_countAll ) {
		return true;
	}
	return false;
}
bool FilmStrip::process( double& percentage )
{
	if ( m_count == m_countAll ) {
		delete_gmerlin_avdecoder();
		if ( m_cache ) {
			m_cache->clean();
			delete m_cache;
			m_cache = 0;
		}
		g_timelineView->redraw();
		return false;
	}
	if ( m_cache->isEmpty() ) {
		bgav_seek_video( m_decoder, 0, m_count * 4 * m_format_src->timescale );
		m_pics[m_count].data = new unsigned char[PIC_WIDTH * PIC_HEIGHT * 3];
		m_pics[m_count].w = PIC_WIDTH;
		m_pics[m_count].h = PIC_HEIGHT;
		
		bgav_read_video( m_decoder, m_frame_src, 0 );
		gavl_video_convert( m_converter, m_frame_src, m_frame_dst );

		unsigned char* src = (unsigned char*)m_frame_dst->planes[0];
		int strides = m_frame_dst->strides[0];
		for ( int i = 0; i < PIC_HEIGHT; i++ ) {
			memcpy( &m_pics[m_count].data[PIC_WIDTH*i*3], &src[i*strides], PIC_WIDTH * 3 );
		}

		m_cache->write( m_pics[m_count].data, (PIC_WIDTH * PIC_HEIGHT * 3) );
		m_count++;
	} else {
		m_pics[m_count].data = new unsigned char[PIC_WIDTH * PIC_HEIGHT * 3];
		m_pics[m_count].w = PIC_WIDTH;
		m_pics[m_count].h = PIC_HEIGHT;
		int64_t c = m_cache->read( m_pics[m_count].data, (PIC_WIDTH * PIC_HEIGHT * 3) );
		if ( c < (PIC_WIDTH * PIC_HEIGHT * 3) ) {
			cout << "WARNING FilmStrip::process c < PIC_SIZE" << endl;
			m_count = m_countAll;
		}
		m_count++;
	}
	return true;
}
FilmStrip::~FilmStrip()
{
	for ( unsigned int i = 0; i < m_count; i++ ) {
		delete [] m_pics[i].data;
	}
	delete [] m_pics;
	delete_gmerlin_avdecoder();
	if ( m_cache ) {
		delete m_cache;
		m_cache = 0;
	}
}

const char* FilmStrip::filename()
{
	return m_filename.c_str();
}


} /* namespace nle */
