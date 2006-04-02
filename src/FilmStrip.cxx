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


#include "FilmStrip.H"
#include "IVideoFile.H"
#include "TimelineView.H"

namespace nle
{

#define PIC_WIDTH 40 
#define PIC_HEIGHT 30

FilmStrip::FilmStrip( IVideoFile* vfile )
	: m_vfile( vfile )
{
	m_rows  = new unsigned char*[PIC_HEIGHT];
	m_countAll = m_vfile->length() / 100;
	m_pics = new pic_struct[m_countAll];
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
		return false;
	}
	m_vfile->seek( m_count * 100 );
	m_pics[m_count].data = new unsigned char[PIC_WIDTH * PIC_HEIGHT * 3];
	m_pics[m_count].w = PIC_WIDTH;
	m_pics[m_count].h = PIC_HEIGHT;
	for ( int j = 0; j < PIC_HEIGHT; j++ ) {
		m_rows[j] = m_pics[m_count].data + PIC_WIDTH * 3 * j;
	}
	m_vfile->read( m_rows, PIC_WIDTH, PIC_HEIGHT );
	if ( m_count % 20 == 0 ) {
		g_timelineView->redraw();
	}
	m_count++;
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
}


} /* namespace nle */
