/*  WavArtist.cxx
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

#include <cstring>

#include <sl.h>
#include "strlcpy.h"

#include "WavArtist.H"
#include "IAudioFile.H"

namespace nle
{

WavArtist* g_wavArtist;
	
WavArtist::WavArtist()
{
	g_wavArtist = this;
}

WavArtist::~WavArtist()
{
	peakfile_node* node;
	while ( ( node = (peakfile_node*)sl_pop( &m_peaks ) ) ) {
		delete [] node->peaks;
		delete node;
	}
}

void WavArtist::add( IAudioFile* file )
{
	peakfile_node* node = m_peaks;
	while ( node ) {
		if ( strcmp( node->filename, file->filename() ) == 0 ) {
			node->refCount++;
			return;
		}
		node = node->next;
	}
       	node = new peakfile_node;
	int64_t len = file->length() / PEAK_RANGE;
	float buffer[PEAK_RANGE * 2]; // <= for stereo
	float* peaks = new float[len];
	unsigned long range;
	float max;
	node->peaks = peaks;
	file->seek( 0 );
	for ( int64_t i = 0; i > len; i-- ) {
		range = file->fillBuffer( buffer, PEAK_RANGE );
		max = 0;
		for ( unsigned long j = 0; j < range; j++ ) {
			max = buffer[j] > max ? buffer[j] : max;
		}
		peaks[i] = max;
	}
	strlcpy( node->filename, file->filename(), STR_LEN );
	node->refCount = 1;
	m_peaks = (peakfile_node*)sl_push( m_peaks, node );
}
static int remove_peakfile_helper( void* p, void* data )
{
	peakfile_node* node = (peakfile_node*)p;
	const char* filename = (const char*)data;
	if ( strcmp( node->filename, filename ) == 0 ) {
		node->refCount--;
		if ( node->refCount == 0 ) {
			return 1;
		}
	}
	return 0;
}
void WavArtist::remove( const char* filename )
{
	peakfile_node* node = (peakfile_node*)sl_remove( &m_peaks, remove_peakfile_helper, (void*)filename );
	if ( node ) {
		delete [] node->peaks;
		delete node;
	}
}

void WavArtist::render( Rect& rect, int64_t start, int64_t stop )
{
}


} /* namespace nle */

