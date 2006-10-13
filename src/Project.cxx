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

#include "config.h"
#include "Project.H"
#include "globals.H"
#include "timeline/Track.H"
#include "Timeline.H"
#include "timeline/Clip.H"
#include "AudioTrack.H"
#include "VideoTrack.H"
#include "VideoFileQT.H"
#include "AudioFileFactory.H"
#include "VideoClip.H"
#include "AudioClip.H"
#include "TimelineView.H"
#include "ImageClip.H"
#include "DummyClip.H"

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
	TiXmlText* text = new TiXmlText( VERSION );
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
	TiXmlElement* automation;
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
			clip->SetAttribute( "length", cn->clip->length() );
			clip->SetAttribute( "trimA", cn->clip->trimA() );
			clip->SetAttribute( "trimB", cn->clip->trimB() );
			clip->SetAttribute( "trimB", cn->clip->trimB() );
			if ( AudioClip* ac = dynamic_cast<AudioClip*>(cn->clip) ) {
				auto_node* q = ac->getAutoPoints();
				for ( ; q; q = q->next ) {
					automation = new TiXmlElement( "automation" );
					clip->LinkEndChild( automation );
					automation->SetAttribute( "x", q->x );
					automation->SetDoubleAttribute( "y", q->y );
				}
			} else if ( VideoClip* vc = dynamic_cast<VideoClip*>(cn->clip) ) {
				clip->SetAttribute( "mute", (int)vc->m_mute );
			}
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
	
	int trackId = 0;
	
	for ( ; track; track = track->NextSiblingElement( "track" ) ) {
		trackId = getTrackId();
		VideoTrack *tr = new VideoTrack( trackId );
		g_timeline->addTrack( tr );
		
		TiXmlElement* j = TiXmlHandle( track ).FirstChildElement( "clip" ).Element();
		for ( ; j; j = j->NextSiblingElement( "clip" ) ) {
			int position; //TODO: int64_t problem
			int trimA;
			int trimB;
			int mute = 0;
			int length;
			char filename[1024];
			if ( ! j->Attribute( "position", &position ) )
				continue;
			if ( ! j->Attribute( "trimA", &trimA ) )
				continue;
			if ( ! j->Attribute( "trimB", &trimB ) )
				continue;
			if ( ! j->Attribute( "length", &length ) ) {
				length = -1;
			}
			strlcpy( filename, j->Attribute( "filename" ), sizeof(filename) );
			if ( ! filename )
				continue;
			j->Attribute( "mute", &mute );
			g_timeline->addFile( trackId, position, filename, trimA, trimB, mute, -1, length );
		}
		tr->reconsiderFadeOver();
	}
	track = docH.FirstChild( "open_movie_editor_project" ).FirstChild( "audio_tracks" ).FirstChild( "track" ).Element();
	for ( ; track; track = track->NextSiblingElement( "track" ) ) {
		trackId = getTrackId();
		Track *tr = new AudioTrack( trackId );
		g_timeline->addTrack( tr );
		
		TiXmlElement* j = TiXmlHandle( track ).FirstChildElement( "clip" ).Element();
		for ( ; j; j = j->NextSiblingElement( "clip" ) ) {
			int position;
			int trimA;
			int trimB;
			int length;
			char filename[1024];
			if ( ! j->Attribute( "position", &position ) )
				continue;
			if ( ! j->Attribute( "trimA", &trimA ) )
				continue;
			if ( ! j->Attribute( "trimB", &trimB ) )
				continue;
			if ( ! j->Attribute( "length", &length ) ) {
				length = -1;
			}
			strlcpy( filename, j->Attribute( "filename" ), sizeof(filename) );
			if ( ! filename )
				continue;
			IAudioFile *af = AudioFileFactory::get( filename );
			if ( !af ) {
				Track *t = g_timeline->getTrack( trackId );
				Clip* clip = new DummyClip( t, filename, position, length+trimA+trimB, trimA, trimB );
				g_timeline->addClip( trackId, clip );
				continue;
			}
			AudioClip* ac = new AudioClip( tr, position, af, trimA, trimB );
			Clip* clip = ac;
			//read and process Automations
			TiXmlElement* automation = TiXmlHandle( j ).FirstChildElement( "automation" ).Element();
			auto_node* autonode = ac->getAutoPoints();
			int x;
			double y;
			bool fff = false;
			for ( ; automation; automation = automation->NextSiblingElement( "automation" ) ) {
				if ( fff ) {
					if ( !autonode->next ) {
						autonode->next = new auto_node;
						autonode->next->next = 0;
						autonode->next->x = autonode->x + 1;
						autonode->next->y = 1.0;
					}
					autonode = autonode->next;
				} else {
					fff = true;
				}
				if ( ! automation->Attribute( "x", &x ) )
					continue;
				if ( ! automation->Attribute( "y", &y ) )
					continue;
				autonode->x = x;
				autonode->y = y;
			}
			g_timeline->addClip( trackId, clip );
		}
	}
	g_timelineView->redraw();
	g_timelineView->adjustScrollbar();
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
