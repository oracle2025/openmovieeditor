/*  AudioFileSnd.cxx
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

#include <iostream>

#include "strlcpy.h"

#include "AudioFileSnd.H"

using namespace std;

namespace nle
{

AudioFileSnd::AudioFileSnd( const char* filename )
{
	m_ok = false;
	SF_INFO sfinfo;
	m_sndfile = sf_open( filename, SFM_READ, &sfinfo );	
	if ( SF_ERR_NO_ERROR != sf_error( m_sndfile ) ) {
		cerr << "Could not open Soundfile" << endl;
		sf_close( m_sndfile );
		m_sndfile = 0;
		return;
	}
	if ( sfinfo.channels != 2 ) {
		cerr << "Soundfile is not a stereo" << endl;
		sf_close( m_sndfile );
		m_sndfile = 0;
		return;
	}
	if ( sfinfo.frames==0 ) {
		cerr << "Soundfile has length zero" << endl;
		sf_close( m_sndfile );
		m_sndfile = 0;
		return;
	}
	if ( sfinfo.samplerate != 48000 ) {
		cerr << "Wrong Samplerate, only 48000 allowed" << endl;
		sf_close( m_sndfile );
		m_sndfile = 0;
		return;
	}
	m_length = sfinfo.frames;
	strlcpy( m_filename, filename, STR_LEN );
	m_ok = true;
}
AudioFileSnd::~AudioFileSnd()
{
	if ( m_sndfile ) {
		sf_close( m_sndfile );
		m_sndfile = 0;
	}
}

void AudioFileSnd::seek( int64_t sample )
{
	sf_seek( m_sndfile, sample, SEEK_SET );
}

int AudioFileSnd::fillBuffer( float* output, unsigned long frames )
{
	return sf_readf_float( m_sndfile, output, frames );
}





} /* namespace nle */
