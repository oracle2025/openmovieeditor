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

#include "FilmStrip.H"
#include "IVideoFile.H"
#include "TimelineView.H"
#include "SwitchBoard.H"
#include "VideoFileFactory.H"
#include "DiskCache.H"


namespace nle
{

#define PIC_WIDTH 40 
#define PIC_HEIGHT 30

FilmStrip::FilmStrip( IVideoFile* vfile )
{
	m_vfile = 0;
	m_cache = new DiskCache( vfile->filename(), "thumbs" );
	m_rows  = new unsigned char*[PIC_HEIGHT];

	if ( m_cache->isEmpty() ) {
		m_vfile = VideoFileFactory::get( vfile->filename() );
		m_countAll = m_vfile->length() / 100;
	} else {
		m_countAll = m_cache->size() / ( 3 * PIC_WIDTH * PIC_HEIGHT );
	}
	
/*	m_vfile = VideoFileFactory::get( vfile->filename() );
	m_rows  = new unsigned char*[PIC_HEIGHT];
	m_countAll = m_vfile->length() / 100;*/
	try {
		m_pics = new pic_struct[m_countAll];
	} catch(std::exception &e) {
		cerr << "Out of Memory? " << e.what() << endl;
	}
	m_count = 0;
	start();
}
bool FilmStrip::process()
{
	if ( m_count == m_countAll ) {
		if ( m_rows ) {
			delete [] m_rows;
			m_rows = 0;
		}
		if ( m_vfile ) {
			delete m_vfile;
			m_vfile = 0;
		}
		if ( m_cache ) {
			m_cache->clean();
			delete m_cache;
			m_cache = 0;
		}
		g_timelineView->redraw();
		return false;
	}
	if ( m_cache->isEmpty() ) {
		m_vfile->seek( m_count * 100 );
		m_pics[m_count].data = new unsigned char[PIC_WIDTH * PIC_HEIGHT * 3];
		m_pics[m_count].w = PIC_WIDTH;
		m_pics[m_count].h = PIC_HEIGHT;
		for ( int j = 0; j < PIC_HEIGHT; j++ ) {
			m_rows[j] = m_pics[m_count].data + PIC_WIDTH * 3 * j;
		}
		m_vfile->read( m_rows, PIC_WIDTH, PIC_HEIGHT );
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
	stop();
	for ( unsigned int i = 0; i < m_count; i++ ) {
		delete [] m_pics[i].data;
	}
	delete [] m_pics;
	if ( m_rows ) {
		delete [] m_rows;
	}
	if ( m_vfile ) {
		delete m_vfile;
	}
	if ( m_cache ) {
		delete m_cache;
		m_cache = 0;
	}
}


} /* namespace nle */
