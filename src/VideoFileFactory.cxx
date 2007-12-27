/*  VideoFileFactory.cxx
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

#include "VideoFileQT.H"
#include "VideoFileFfmpeg.H"
#include "VideoFileFactory.H"
#include "VideoFileMpeg3.H"
#include <FL/filename.H>
namespace nle
{

IVideoFile* VideoFileFactory::get( string filename )
{
	const char* ext = fl_filename_ext( filename.c_str() );
	if ( strcmp( ext, ".JPEG" ) == 0 ||strcmp( ext, ".JPG" ) == 0 ||strcmp( ext, ".jpg" ) == 0 || strcmp( ext, ".png" ) == 0 || strcmp( ext, ".jpeg" ) == 0 ) {
		return 0;
	}
	IVideoFile* vf;
#ifdef LIBMPEG3
	if ( strcmp( ext, ".TOC" ) == 0 || strcmp( ext, ".toc" ) == 0 ) {
		vf = new VideoFileMpeg3( filename );
		if ( vf->ok() ) {
			return vf;
		}
		delete vf;
	}
#endif /* LIBMPEG3 */
	vf = new VideoFileQT( filename );
	if ( vf->ok() ) {
		return vf;
	}
	delete vf;
#ifdef AVCODEC
	vf = new VideoFileFfmpeg( filename );
	if ( vf->ok() ) {
		return vf;
	}
	delete vf;
#endif /* AVCODEC */
	return 0;
}

} /* namespace nle */
