/*  Project.H
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

#ifndef __STDC_FORMAT_MACROS
  #define __STDC_FORMAT_MACROS
#endif

#include <cstring>
#include "strlcpy.h"

#include <tinyxml.h>

#include "Project.H"
#include "globals.H"
#include "timeline/Track.H"
#include "Timeline.H"
#include "timeline/Clip.H"
#include "AudioTrack.H"
#include "VideoTrack.H"
#include "VideoFileQT.H"
#include "AudioFileQT.H"
#include "AudioFileSnd.H"
#include "VideoClip.H"
#include "AudioClip.H"
#include "TimelineView.H"

namespace nle
{
//static char project_filename[1024];

Project* g_project;

Project::Project()
{
	g_project = this;
}
int Project::write( string filename, string name )
{
/*	strcpy( project_filename, "" );
	if ( !getenv( "HOME" ) ) {
		return;
	}
	strlcpy( project_filename, getenv( "HOME" ), sizeof(project_filename) );
	strncat( project_filename, "/.openme.project", sizeof(project_filename) - strlen( project_filename ) - 1 );
*/	
	TiXmlDocument doc( filename.c_str() );
	TiXmlDeclaration* dec = new TiXmlDeclaration( "1.0", "", "no" );
	doc.LinkEndChild( dec );

	TiXmlElement* project = new TiXmlElement( "open_movie_editor_project" );
	doc.LinkEndChild( project );

	TiXmlElement* item = new TiXmlElement( "version" );
	project->LinkEndChild( item );
	TiXmlText* text = new TiXmlText( OME_VERSION );
	item->LinkEndChild( text );

	item = new TiXmlElement( "name" );
	project->LinkEndChild( item );
	text = new TiXmlText( name.c_str() );
	item->LinkEndChild( text );

	item = new TiXmlElement( "zoom" );
	item->SetDoubleAttribute( "value", 1.0 );
	project->LinkEndChild( item );

	item = new TiXmlElement( "scroll" );
	item->SetAttribute( "value", 0 );
	project->LinkEndChild( item );

	item = new TiXmlElement( "stylus" );
	item->SetAttribute( "value", 0 );
	project->LinkEndChild( item );

	TiXmlElement* video_tracks = new TiXmlElement( "video_tracks" );
	project->LinkEndChild( video_tracks );
	
	TiXmlElement* audio_tracks = new TiXmlElement( "audio_tracks" );
	project->LinkEndChild( audio_tracks );

	track_node* node = g_timeline->getTracks();

	TiXmlElement* track;
	TiXmlElement* clip;
	while ( node ) {
		track = new TiXmlElement( "track" );
		if ( node->track->type() == TRACK_TYPE_VIDEO ) {
			video_tracks->LinkEndChild( track );
		} else if ( node->track->type() == TRACK_TYPE_AUDIO ) {
			audio_tracks->LinkEndChild( track );
		}
		clip_node* cn = node->track->getClips();
		while ( cn ) {
			clip = new TiXmlElement( "clip" );
			track->LinkEndChild( clip );
			clip->SetAttribute( "filename", cn->clip->filename().c_str() );
			clip->SetAttribute( "position", cn->clip->position() );
			clip->SetAttribute( "trimA", cn->clip->trimA() );
			clip->SetAttribute( "trimB", cn->clip->trimB() );
			cn = cn->next;
		}
		node = node->next;
	}

	
	doc.SaveFile();
	
	cout << "Write: " << filename << " | " << name << endl;
	return 1;
}
int Project::read( string filename )
{
/*	strcpy( project_filename, "" );
	if ( !getenv( "HOME" ) ) {
		return;
	}
	strlcpy( project_filename, getenv( "HOME" ), sizeof(project_filename) );
	strncat( project_filename, "/.openme.project", sizeof(project_filename) - strlen( project_filename ) - 1 );
*/	
	TiXmlDocument doc( filename.c_str() );
	if ( !doc.LoadFile() ) {
		return 0;
	}
	g_timeline->clear();
	TiXmlHandle docH( &doc );
	//TiXmlElement* video_tracks = docH.FirstChildElement( "open_movie_editor_project" ).FirstChildElement( "video_tracks" );
	TiXmlElement* track = docH.FirstChild( "open_movie_editor_project" ).FirstChild( "video_tracks" ).FirstChild( "track" ).Element();
	
	int i = 0;
	
	for ( ; track; track = track->NextSiblingElement( "track" ) ) {
		Track *tr = new VideoTrack( i );
		g_timeline->addTrack( tr );
		
		TiXmlElement* j = TiXmlHandle( track ).FirstChildElement( "clip" ).Element();
		for ( ; j; j = j->NextSiblingElement( "clip" ) ) {
			int position;
			int trimA;
			int trimB;
			char filename[1024];
			if ( ! j->Attribute( "position", &position ) )
				continue;
			if ( ! j->Attribute( "trimA", &trimA ) )
				continue;
			if ( ! j->Attribute( "trimB", &trimB ) )
				continue;
			strlcpy( filename, j->Attribute( "filename" ), sizeof(filename) );
			if ( ! filename )
				continue;
			VideoFileQT* vf = new VideoFileQT( filename );
			if ( !vf->ok() ) {
				delete vf;
				continue;
			}
			Clip* clip = new VideoClip( tr, position, vf );
			clip->trimA( trimA );
			clip->trimB( trimB );
			g_timeline->addClip( i, clip );
		}
		i++;
	}
	track = docH.FirstChild( "open_movie_editor_project" ).FirstChild( "audio_tracks" ).FirstChild( "track" ).Element();
	for ( ; track; track = track->NextSiblingElement( "track" ) ) {
		Track *tr = new AudioTrack( i );
		g_timeline->addTrack( tr );
		
		TiXmlElement* j = TiXmlHandle( track ).FirstChildElement( "clip" ).Element();
		for ( ; j; j = j->NextSiblingElement( "clip" ) ) {
			int position;
			int trimA;
			int trimB;
			char filename[1024];
			if ( ! j->Attribute( "position", &position ) )
				continue;
			if ( ! j->Attribute( "trimA", &trimA ) )
				continue;
			if ( ! j->Attribute( "trimB", &trimB ) )
				continue;
			strlcpy( filename, j->Attribute( "filename" ), sizeof(filename) );
			if ( ! filename )
				continue;
			IAudioFile *af = new AudioFileSnd( filename );
			if ( !af->ok() ) {
				delete af;
				af = new AudioFileQT( filename );
			}
			if ( !af->ok() ) {
				delete af;
				continue;
			}
			Clip* clip = new AudioClip( tr, position, af );
			clip->trimA( trimA );
			clip->trimB( trimB );
			g_timeline->addClip( i, clip );
		}
		i++;
	}
	g_timelineView->redraw();
	cout << "Read: " << filename << endl;
	return 1;
}
void Project::write_project()
{
}

void Project::read_project()
{



}

} /* namespace nle */
