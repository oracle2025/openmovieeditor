/*  WaveForm.cxx
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

#include <cstdio>
#include <cassert>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "WaveForm.H"
#include "AudioFileFactory.H"
#include "IAudioFile.H"
#include "TimelineView.H"
#include "SwitchBoard.H"
#include "globals.H"
#include "helper.H"

#define PEAK_RANGE 1000
#define BUFFER_LEN 1024
namespace nle
{

WaveForm::WaveForm( string filename )
{
	m_peaks = 0;
	m_peakLength = 0;
	m_finalLength = 0;
	char buffer[BUFFER_LEN];
	struct stat statbuf;
	findpath( filename.c_str(), buffer, BUFFER_LEN );
	string cachefile;
	string cachepath;
	cachepath = "/home/oracle/.openme";
	cachepath += buffer;
	cachefile = "/home/oracle/.openme";
	cachefile += filename;
	m_waveformCache = fopen( cachefile.c_str(), "r" );
	m_af = 0;
	if ( m_waveformCache ) {
		cout << "WaveForm::WaveForm: Read From Cache: " << filename << endl;
		m_readFromCache = true;
		int r = stat( cachefile.c_str(), &statbuf );
		assert( r != -1 );
		m_finalLength = statbuf.st_size / sizeof(float);
	} else {
		cout << "WaveForm::WaveForm: Read From File: " << filename << endl;
		m_readFromCache = false;
		mkdirp( cachepath.c_str() );
		m_waveformCache = fopen( cachefile.c_str(), "w" );
		assert( m_waveformCache );
		m_af = AudioFileFactory::get( filename );
		m_finalLength = m_af->length() / PEAK_RANGE;
		m_af->seek( 0 );
	}
	m_peaks = new float[m_finalLength];
	start();
}

WaveForm::~WaveForm()
{
	stop();
	if ( m_af ) {
		delete m_af;
	}
	if ( m_peaks ) {
		delete [] m_peaks;
	}
	if ( m_waveformCache ) {
		fclose( m_waveformCache );
		m_waveformCache = 0;
	}
}

bool WaveForm::process()
{
	if ( m_peakLength >= m_finalLength ) {
		if ( m_af ) {
			delete m_af;
			m_af = 0;
		}
		if ( m_waveformCache ) {
			fclose( m_waveformCache );
			m_waveformCache = 0;
		}
		g_timelineView->redraw();
		return false;
	}
	if ( m_readFromCache ) {
		size_t c = fread( &m_peaks[m_peakLength], 1, 1000, m_waveformCache );
		m_peakLength += (c/sizeof(float));
		if ( c == 0 && feof( m_waveformCache ) ) {
			m_peakLength = m_finalLength;
		}
	} else {
		static float buffer[PEAK_RANGE * 2];
		unsigned long range;
		float max;
		range = m_af->fillBuffer( buffer, PEAK_RANGE );
		max = 0.0;
		for ( int j = 0; j < (int)(range - 1) * 2; j++ ) {
			max = buffer[j] > max ? buffer[j] : max;
		}
		m_peaks[m_peakLength] = max;
		fwrite( &m_peaks[m_peakLength], sizeof(float), 1, m_waveformCache ); //TODO: Could be optimized
		m_peakLength++;
	}
	return true;
}

} /* namespace nle */
