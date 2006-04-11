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

#include "WaveForm.H"
#include "AudioFileFactory.H"
#include "IAudioFile.H"
#include "TimelineView.H"
#include "SwitchBoard.H"
#include "globals.H"

#define PEAK_RANGE 1000

namespace nle
{

WaveForm::WaveForm( string filename )
{
	m_peaks = 0;
	m_peakLength = 0;
	m_finalLength = 0;
	m_af = AudioFileFactory::get( filename );

	m_finalLength = m_af->length() / PEAK_RANGE;
	m_peaks = new float[m_finalLength];
	m_af->seek( 0 );
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
}

bool WaveForm::process()
{
	if ( m_peakLength == m_finalLength ) {
		if ( m_af ) {
			delete m_af;
			m_af = 0;
		}
		return false;
	}
	static float buffer[PEAK_RANGE * 2];
	unsigned long range;
	float max;
	range = m_af->fillBuffer( buffer, PEAK_RANGE );
	max = 0.0;
	for ( int j = 0; j < (int)(range - 1) * 2; j++ ) {
		max = buffer[j] > max ? buffer[j] : max;
	}
	m_peaks[m_peakLength] = max;
	m_peakLength++;
	int64_t inc = 1 + (int64_t)( 0.5 / SwitchBoard::i()->zoom() );
	if ( m_peakLength % inc == 0 ) {
		g_timelineView->redraw();
	}
	return true;
}

} /* namespace nle */
