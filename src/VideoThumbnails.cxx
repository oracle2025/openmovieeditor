/*  VideoThumbnails.cxx
 *
 *  Copyright (C) 2006 Richard Spindler <richard.spindler AT gmail.com>
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

#include <FL/Fl_Shared_Image.H>

#include "VideoThumbnails.H"
#include "DiskCache.H"
#include "IVideoFile.H"
#include "VideoFileFactory.H"
#include "LazyFrame.H"

#include <cstring>

namespace nle
{

bool VideoThumbnails::get( const char* filename, unsigned char* rgb, int &w, int &h )
{
	DiskCache cache( filename, "thumb" );
	if ( cache.isEmpty() ) {
		IVideoFile* vf = VideoFileFactory::get( filename );
		if ( !vf ) {
			Fl_Shared_Image* image;
			image = Fl_Shared_Image::get( filename );
			if ( !image || image->d() != 3 ) {
				cache.clean();
				return false;
			}
			w = image->w();
			h = image->h();
			Fl_Image* image2 = image->copy( VIDEO_THUMBNAIL_WIDTH, VIDEO_THUMBNAIL_HEIGHT );
			image->release();
			char** d = (char**)image2->data();
			memcpy( rgb, d[0], VIDEO_THUMBNAIL_HEIGHT * VIDEO_THUMBNAIL_WIDTH * 3 );
			delete image2;
			cache.write( &w, sizeof(int) );
			cache.write( &h, sizeof(int) );
			cache.write( rgb, VIDEO_THUMBNAIL_HEIGHT * VIDEO_THUMBNAIL_WIDTH * 3 );
			cache.clean();
			return true;
		}
		LazyFrame* f = vf->read();
		f->set_rgb_target( VIDEO_THUMBNAIL_WIDTH, VIDEO_THUMBNAIL_HEIGHT );
		memcpy( rgb, f->get_target_buffer(), VIDEO_THUMBNAIL_WIDTH * VIDEO_THUMBNAIL_HEIGHT * 3 );
		w = vf->width();
		h = vf->height();
		cache.write( &w, sizeof(int) );
		cache.write( &h, sizeof(int) );
		cache.write( rgb, VIDEO_THUMBNAIL_HEIGHT * VIDEO_THUMBNAIL_WIDTH * 3 );
		cache.clean();
		delete vf;
		return true;
	} else {
		if ( cache.size() == 0 ) {
			return 0;
		}
		cache.read( &w, sizeof(int) );
		cache.read( &h, sizeof(int) );
		cache.read( rgb, VIDEO_THUMBNAIL_HEIGHT * VIDEO_THUMBNAIL_WIDTH * 3 );
		return true;
	}
}




} /* namespace nle */
