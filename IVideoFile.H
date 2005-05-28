/*  IVideoFile.H
 *
 *  Copyright (C) 2003 Richard Spindler <richard.spindler AT gmail.com>
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

#ifndef _I_VIDEO_FILE_H_
#define _I_VIDEO_FILE_H_

#include "frame_struct.h"
#include "globals.H"

namespace nle
{

class IVideoFile
{
	public:
		virtual ~IVideoFile(){}
		virtual int64_t length() = 0;
		inline int width() { return m_width; }
		inline int height() { return m_height; }
		virtual int fps() = 0;
		virtual frame_struct* read() = 0;
		virtual void read( unsigned char** rows, int w, int h ) = 0; //For Timeline Picture Preview
		virtual void seek( int64_t frame ) = 0;
		const char* filename() { return m_filename; }
	protected:
		int m_width;
		int m_height;
		char m_filename[STR_LEN];
};

} /* namespace nle */

#endif /* _I_VIDEO_FILE_H_ */
