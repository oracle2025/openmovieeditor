/*  FilmStrip.H
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

#ifndef _FILM_STRIP_H_
#define _FILM_STRIP_H_

#include <stdint.h>

namespace nle
{

typedef struct
{
	unsigned char* data;
	int w;
	int h;
} pic_struct;

class IVideoFile;

class FilmStrip
{
	public:
		FilmStrip( IVideoFile* vfile );
		~FilmStrip();
		inline unsigned int count() { return m_count; }
		inline pic_struct* get_pic( unsigned int index ) { return index < m_count ? &m_pics[index] : 0; }
	private:
		unsigned int m_count;
		pic_struct *m_pics;
};


} /* namespace nle */

#endif /* _FILM_STRIP_H_ */

