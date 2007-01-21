/*  IVideoFile.H
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

#ifndef _I_VIDEO_FILE_H_
#define _I_VIDEO_FILE_H_

#include "frame_struct.h"
#include "globals.H"
#include "global_includes.H"
#include "IVideoReader.H"

namespace nle
{

class IVideoFile : public IVideoReader
{
	public:
		virtual ~IVideoFile() {}
		virtual int64_t length() = 0; //TODO: return length in ticks ( length * 1200 )
		inline int width() { return m_width; }
		inline int height() { return m_height; }
//		virtual double fps() = 0; //TODO: remove this
		virtual frame_struct* read() = 0;
		//======== From IVideoReader
		frame_struct* getFrame( int64_t position ) { //TODO: use ticks ( position / 1200 )
			int64_t frameNum = position / ticksPerFrame();
			if ( m_lastFramePosition + 1 == frameNum ) {
				m_lastFramePosition = frameNum;
				return read();
			} else {
				m_lastFramePosition = frameNum;
				seekToFrame( frameNum );
				return read();
			}
		}
		//========
		virtual void read( unsigned char** rows, int w, int h ) = 0; //For Timeline Picture Preview
		virtual void seek( int64_t position ) = 0; //TODO: use ticks
		string filename() { return m_filename; }
		virtual bool ok() = 0;
	protected:
		virtual int64_t ticksPerFrame() = 0;
		virtual void seekToFrame( int64_t frame ) = 0;
		IVideoFile() { m_lastFramePosition = -1; }
		int m_width;
		int m_height;
		string m_filename;
		int64_t m_lastFramePosition; //TODO: is this ticks or frames? frames is propable more useful
};

} /* namespace nle */

#endif /* _I_VIDEO_FILE_H_ */
