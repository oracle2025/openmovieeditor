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

namespace nle
{

#define PIC_WIDTH 40 
#define PIC_HEIGHT 30

FilmStrip::FilmStrip( IVideoFile* vfile )
{
	unsigned char **rows  = new unsigned char*[PIC_HEIGHT];
	m_count = vfile->length() / 100;
	m_pics = new pic_struct[m_count];
	for ( unsigned int i = 0; i < m_count; i++ ) {
		vfile->seek( i * 100 );
		m_pics[i].data = new unsigned char[PIC_WIDTH * PIC_HEIGHT * 3];
		m_pics[i].w = PIC_WIDTH;
		m_pics[i].h = PIC_HEIGHT;
		for ( int j = 0; j < PIC_HEIGHT; j++ ) {
			rows[j] = m_pics[i].data + PIC_WIDTH * 3 * j;
		}
		vfile->read( rows, PIC_WIDTH, PIC_HEIGHT );
	}
}
FilmStrip::~FilmStrip()
{
	for ( unsigned int i = 0; i < m_count; i++ ) {
		delete [] m_pics[i].data;
	}
	delete [] m_pics;
}


} /* namespace nle */
