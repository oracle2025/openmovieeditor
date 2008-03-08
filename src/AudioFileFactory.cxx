/*  AudioFileFactory.cxx
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

#include "AudioFileFactory.H"
#include "AudioFileQT.H"
#include "AudioFileSnd.H"
#include "AudioFileFfmpeg.H"
#include "AudioFileMpeg3.H"
#include "Resampler.H"
#include <FL/filename.H>

#include <cstring>

namespace nle
{

IAudioFile* AudioFileFactory::get( string filename )
{
	IAudioFile *af = 0;
#ifdef LIBMPEG3
	const char* ext = fl_filename_ext( filename.c_str() );
	if ( strcmp( ext, ".TOC" ) == 0 || strcmp( ext, ".toc" ) == 0 ) {
		af = new AudioFileMpeg3( filename );
	}
#endif /* LIBMPEG3 */

	if ( !af || !af->ok() ) {
		if ( af ) {
			delete af;
		}
		af = new AudioFileSnd( filename );
	}
	if ( !af->ok() ) {
		delete af;
		af = new AudioFileQT( filename );
	}
	if ( !af->ok() ) {
		delete af;
		af = 0;
#ifdef AVCODEC
		af = new AudioFileFfmpeg( filename );
#endif /* AVCODEC */
	}
#ifdef AVCODEC
	if ( !af->ok() ) {
		delete af;
		af = 0;
	}
#endif /* AVCODEC */
	if ( af && af->samplerate() != 48000 ) {
		af = new Resampler( af );
	}
	return af;
}


} /* namespace nle */
