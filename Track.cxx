/*  Track.cxx
 *
 *  Copyright (C) 2003 Richard Spindler <richard.spindler AT gmail.com>
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

#include <algorithm>
#include <iostream>

using namespace std;

#include "Track.H"
#include "Clip.H"




namespace nle
{

Track::Track( int num )
{
	m_num = num;
	m_clips = 0;
}
Track::~Track()
{
	clip_node* node;
	while ( ( node = (clip_node*)sl_pop( &m_clips ) ) ) {
		delete node->clip;
		delete node;
	}
}
void Track::add( Clip* clip )
{
	clip_node* node = new clip_node;
	node->clip = clip;
	m_clips = (clip_node*)sl_push( m_clips, node );
}
static int find_clip( void* p, void* data )
{
	Clip* clip = (Clip*)data;
	clip_node* node = (clip_node*)p;
	if ( node->next && node->next->clip == clip ) {
		return 1;
	}
	return 0;
}
void Track::remove( Clip* clip )
{
	clip_node* node = (clip_node*)sl_map( m_clips, find_clip, clip );
	if ( node ) {
		clip_node* p = node->next;
		delete p->clip;
		node->next = p->next;
		delete p;
	}
}
static int reset_clip( void* p, void* data )
{
	clip_node* node = (clip_node*)p;
	node->clip->reset();
	return 0;
}
static int cmp_clip(void *p1, void *p2)
{
        clip_node *q1 = (clip_node*)p1, *q2 = (clip_node*)p2;
        return q1->clip->position() - q2->clip->position();
}       
void Track::reset()
{
	m_clips = (clip_node*)sl_mergesort( m_clips, cmp_clip );
	sl_map( m_clips, reset_clip, 0 );
	for ( clip_node* p = m_clips; p; p = p->next ) {
		cout << "POSITION: " << p->clip->position() << endl;
	}
}
#define SNAP_TOLERANCE 10
static bool is_in_tolerance( int64_t A, int64_t B, unsigned int tolerance )
{
	return ( B - tolerance <= A && B + tolerance >= A );
}
int64_t Track::getSnapA( Clip* clip, int64_t A )
{
	for ( clip_node *p = m_clips; p; p = p->next ) {
		Clip* current = p->clip;
		int64_t B = current->B();
		if ( current != clip && is_in_tolerance( A, B, (unsigned int)(SNAP_TOLERANCE * stretchFactor()) ) ) {
			return B + 1;
		}
	}
	return A;
}
int64_t Track::getSnapB( Clip* clip, int64_t B )
{
	for ( clip_node *p = m_clips; p; p = p->next ) {
		Clip* current = p->clip;
		int64_t A = current->position();
		if ( current != clip && is_in_tolerance( B + clip->length(), A, (unsigned int)(SNAP_TOLERANCE * stretchFactor()) ) ) {
			return A - 1 - clip->length();
		}
	}
	return B;
}

	
} /* namespace nle */
