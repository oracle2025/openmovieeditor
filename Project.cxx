/*  Project.H
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

#ifndef __STDC_FORMAT_MACROS
  #define __STDC_FORMAT_MACROS
#endif

#include <iostream>
#include <cstring>
#include <inttypes.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <sexp.h>

#include "Project.H"
#include "globals.H"
#include "Track.H"
#include "Timeline.H"
#include "Clip.H"
#include "VideoTrack.H"
#include "AudioTrack.H"


#define STR(t) t, strlen(t)

#define APPEND_DQUOTE(tmp,s) \
	tmp->next = new_sexp_atom( STR(s) ); \
	tmp       = tmp->next; \
	tmp->aty  = SEXP_DQUOTE;

#define APPEND_ATTRIBUTE(var,name,value) \
	new_sexp_atom( STR(name) ); \
	var->next = new_sexp_atom( STR(value) ); \
	var->next->aty = SEXP_DQUOTE;

using namespace std;

namespace nle
{

class Clip;

static void append_clip( Clip* c, sexp_t* s )
{
	char buffer[STR_LEN];
	sexp_t* tmp   = new_sexp_atom( STR(c->filename()) );
	tmp->aty      = SEXP_DQUOTE;
	sexp_t* clip  = new_sexp_list( tmp );

	s->next       = clip;

	snprintf(buffer, STR_LEN, "%" PRId64 , c->position() );
	APPEND_DQUOTE(tmp, buffer) //position
	snprintf(buffer, STR_LEN, "%" PRId64 , c->trimA() );
	APPEND_DQUOTE(tmp, buffer)
	snprintf(buffer, STR_LEN, "%" PRId64 , c->trimB() );
	APPEND_DQUOTE(tmp, buffer)
}

static void append_track( Track* t, sexp_t* s )
{
	sexp_t* track      = new_sexp_atom( STR("track") );
	sexp_t* track_list = new_sexp_list( track );

	s->next            = track_list;

	list< Clip* >* cps = t->getClips();

	for ( list<Clip*>::iterator i = cps->begin(); i != cps->end(); i++ ) {
		Clip* current = *i;
		append_clip( current, track );
		track = track->next;
	}
}

void Project::write_project()
{

	sexp_t* nle_project        = new_sexp_atom( STR("nle_project") );
	sexp_t* nle_project_list   = new_sexp_list( nle_project );
	
	sexp_t* version            = APPEND_ATTRIBUTE(version, "version", "0.0.0")
	sexp_t* version_list       = new_sexp_list( version );

	nle_project->next          = version_list;

	sexp_t* zoom               = APPEND_ATTRIBUTE( zoom, "zoom", "1.0" )
	sexp_t* zoom_list          = new_sexp_list( zoom );

	version_list->next         = zoom_list;
	
	sexp_t* scroll             = APPEND_ATTRIBUTE(scroll, "scroll", "0")
	sexp_t* scroll_list        = new_sexp_list( scroll );

	zoom_list->next            = scroll_list;
	
	sexp_t* stylus             = APPEND_ATTRIBUTE(stylus, "stylus", "0" )
	sexp_t* stylus_list        = new_sexp_list( stylus );

	scroll_list->next          = stylus_list;

	sexp_t* video_tracks       = new_sexp_atom( STR("video_tracks") );
	sexp_t* video_tracks_list  = new_sexp_list( video_tracks );

	stylus_list->next          = video_tracks_list;

	list<VideoTrack*> *vt = g_timeline->getVideoTracks();

	for ( list<VideoTrack*>::iterator i = vt->begin(); i != vt->end(); i++ ) {
		VideoTrack* current = *i;
		append_track( current, video_tracks );
		video_tracks = video_tracks->next;
	}

	sexp_t* audio_tracks       = new_sexp_atom( STR("audio_tracks") );
	sexp_t* audio_tracks_list  = new_sexp_list( audio_tracks );

	video_tracks_list->next    = audio_tracks_list;

	list<AudioTrack*> *at = g_timeline->getAudioTracks();

	for ( list<AudioTrack*>::iterator i = at->begin(); i != at->end(); i++ ) {
		AudioTrack* current = *i;
		append_track( current, audio_tracks );
		audio_tracks = audio_tracks->next;
	}

	char buffer[STR_LEN];
	print_sexp( buffer, STR_LEN, nle_project_list );
	cout << buffer << endl;
	
	sexp_t_deallocate( nle_project_list );
	
}

void Project::read_project()
{
	const char filename[] = "/home/oracle/video_project_file";
	int fd;
	//char pstr[1024];
	//sexp_t *param;
	sexp_t *sx;
	sexp_iowrap_t *iow;

	fd = open(filename,O_RDONLY);
	iow = init_iowrap(fd);
	sx = read_one_sexp(iow);


}

} /* namespace nle */
