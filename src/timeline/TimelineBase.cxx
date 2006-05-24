/*  TimelineBase.cxx
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

#include <sl.h>

#include "TimelineBase.H"
#include "Track.H"

using namespace std;


namespace nle
{


TimelineBase::TimelineBase()
{
	m_allTracks = 0;
}
TimelineBase::~TimelineBase()
{
	clear();
}
void TimelineBase::clear()
{
	track_node* node;
	while ( ( node = (track_node*)sl_pop( &m_allTracks ) ) ) {
		delete node->track;
		delete node;
	}
}

static int sort_track_helper( void* p, void* data )
{
	track_node* node = (track_node*) p;
	node->track->sort();
	return 0;
}
void TimelineBase::sort()
{
	sl_map( m_allTracks, sort_track_helper, 0 );
}

void TimelineBase::addClip( int track, Clip *clip )
{
	Track *t = getTrack( track );
	if ( t ) {
		t->addClip( clip );
	} else {
		cerr << "No such track" << endl;
	}
}
void TimelineBase::addFile( int track, int64_t position, std::string filename, int64_t trimA, int64_t trimB, int mute, int id )
{
	Track *t = getTrack( track );
	if ( t ) {
		t->addFile( position, filename, trimA, trimB, mute, id );
	} else {
		cerr << "No such track" << endl;
	}
}

void TimelineBase::removeClip( int track, Clip *clip )
{
	Track *t = getTrack( track );
	if ( t ) {
		t->removeClip( clip );
	} else {
		cerr << "No such track" << endl;
	}
}

void TimelineBase::addTrack( Track* track )
{
	track_node* node = new track_node;
	node->track = track;
	node->next = 0;
	m_allTracks = (track_node*)sl_unshift( m_allTracks, node );
}
static int remove_track_helper( void* p, void* data )
{
	// This is a little evil
	// it's a countdown to find the right track
	int* track = (int*)data;
	if ( *track > 0 ) {
		(*track)--;
		return 0;
	} else {
		return 1;
	}
}
void TimelineBase::removeTrack( int track )
{
	track_node* node = (track_node*)sl_remove( &m_allTracks, remove_track_helper, &track );
	delete node->track;
	delete node;
}

Clip* TimelineBase::find( int track, int64_t position )
{
	Track* t = getTrack( track );
	if ( t ) {
		return t->find( position );
	} else {
		cerr << "No such track" << endl;
	}
	return 0;
}

static int track_length_helper( void* p, void* data )
{
	int64_t l;
	int64_t* max = (int64_t*)data;
	track_node* node = (track_node*)p;
	l = node->track->length();
	if ( l > *max ) {
		*max = l;
	}
	return 0;
}
int64_t TimelineBase::length()
{
	sort();
	int64_t max = 0;
	sl_map( m_allTracks, track_length_helper, &max );
	return max;
}

Track* TimelineBase::getTrack( int track )
{
	track_node* node = (track_node*)m_allTracks;
	while ( node && node->track->num() != track ) {
		node = node->next;
	}
	if ( node ) {
		return node->track;
	}
	return 0;
}
Clip* TimelineBase::getClip( int track, int clip )
{
	Track* t = getTrack( track );
	if ( !t ) { return 0; }
	return t->getClip( clip );
}


} /* namespace nle */
