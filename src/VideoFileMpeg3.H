/*  VideoFileMpeg3.H
 *
 *  Copyright (C) 2007 Richard Spindler <richard.spindler AT gmail.com>
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

#ifndef _VIDEO_FILE_MPEG3_H
#define _VIDEO_FILE_MPEG3_H

#ifdef LIBMPEG3

#include <libmpeg3.h>

#include "global_includes.H"
#include "IVideoFile.H"
#include "frame_struct.h"

namespace nle
{

class VideoFileMpeg3 : public IVideoFile
{
	public:
		VideoFileMpeg3( string filename );
		~VideoFileMpeg3();
		bool ok();
		int64_t length();
		frame_struct* read();
		void read( unsigned char** rows, int w, int h );
		void seek( int64_t position );
	protected:
		int64_t ticksPerFrame();
		void seekToFrame( int64_t frame );
	private:
		mpeg3_t* m_file;
		unsigned char *m_frame;
		unsigned char **m_rows;
		frame_struct m_framestruct;
		bool m_ok;
		int64_t m_ticksPerFrame;
};

} /* namespace nle */

#endif /* LIBMPEG3 */

#endif /* _VIDEO_FILE_MPEG3_H */
