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
//#include "IVideoEffect.H"
//#include "Frei0rEffect.H"
//#include "Frei0rFactory.H"
#include "VideoFileFactory.H"
#include "TitleClip.H"
//#include "AudioFilter.H"
#include "MainFilterFactory.H"
#include "FilterBase.H"

namespace nle
{

Project* g_project;

Project::Project()
{
	g_project = this;
}
int Project::write( string filename, string name )
{
	char buffer[512];
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
	//TiXmlElement* automation;
	while ( node ) {
		track = new TiXmlElement( "track" );
		track->SetAttribute( "height", node->track->h() );
		track->SetAttribute( "name", node->track->name().c_str() );
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
			snprintf( buffer, sizeof(buffer), "%lld", cn->clip->position() );
			clip->SetAttribute( "position", buffer );
			snprintf( buffer, sizeof(buffer), "%lld", cn->clip->length() );
			clip->SetAttribute( "length", buffer );
			snprintf( buffer, sizeof(buffer), "%lld", cn->clip->trimA() );
			clip->SetAttribute( "trimA", buffer );
			snprintf( buffer, sizeof(buffer), "%lld", cn->clip->trimB() );
			clip->SetAttribute( "trimB", buffer );
			//TODO: Save Filter Params
			if ( FilterClip* fc = dynamic_cast<FilterClip*>(cn->clip) ) {
				filter_stack* filters;
				for ( filters = fc->getFilters(); filters; filters = filters->next ) {
					TiXmlElement* filter_xml = new TiXmlElement( "filter" );
					clip->LinkEndChild( filter_xml );
					filter_xml->SetAttribute( "name", filters->filter->name() );
					filter_xml->SetAttribute( "identifier", filters->filter->identifier() );
					filters->filter->writeXML( filter_xml );
				}
			}
			if ( VideoEffectClip* vc = dynamic_cast<VideoEffectClip*>(cn->clip) ) {
				if ( vc->def() ) {
					clip->SetAttribute( "render", "default" );
				} else if ( vc->crop() ) {
					clip->SetAttribute( "render", "crop" );
				} else if ( vc->fit() ) {
					clip->SetAttribute( "render", "fit" );
				} else if ( vc->stretch() ) {
					clip->SetAttribute( "render", "stretch" );
				}
				if ( VideoClip* vc = dynamic_cast<VideoClip*>(cn->clip) ) {
					clip->SetAttribute( "mute", (int)vc->m_mute );
				}
				if ( TitleClip* tc = dynamic_cast<TitleClip*>(cn->clip) ) {
					clip->SetAttribute( "text", tc->text() );
					clip->SetDoubleAttribute( "x", tc->x() );
					clip->SetDoubleAttribute( "y", tc->y() );
					clip->SetAttribute( "size", tc->size() );
					clip->SetAttribute( "font", tc->font() );
					clip->SetAttribute( "color", tc->color() );
				}
			}
			cn = cn->next;
		}
		node = node->next;
	}

	
	doc.SaveFile();
	
	return 1;
}
int Project::read( string filename )
{
	TiXmlDocument doc( filename.c_str() );
	if ( !doc.LoadFile() ) {
		return 0;
	}
	g_timeline->clear();
	TiXmlHandle docH( &doc );
	TiXmlElement* track = docH.FirstChild( "open_movie_editor_project" ).FirstChild( "video_tracks" ).FirstChild( "track" ).Element();
	
	int trackId = 0;
	
	for ( ; track; track = track->NextSiblingElement( "track" ) ) {
		trackId = getTrackId();
		VideoTrack *tr = new VideoTrack( trackId );
		g_timeline->addTrack( tr );
		const char* name = track->Attribute( "name" );
		if ( name ) {
			tr->name( name );
		}
		int height = 30;
		if ( track->Attribute( "height", &height ) ) {
			tr->h( height );
		}
		
		TiXmlElement* j = TiXmlHandle( track ).FirstChildElement( "clip" ).Element();
		for ( ; j; j = j->NextSiblingElement( "clip" ) ) {
			int64_t position; //TODO: int64_t problem
			int64_t trimA;
			int64_t trimB;
			int mute = 0;
			int64_t length;
			char filename[1024];
			const char* position_str;
			if ( ! ( position_str = j->Attribute( "position" ) ) )
				continue;
			position = atoll( position_str );
			if ( ! ( position_str = j->Attribute( "trimA" ) ) )
				continue;
			trimA = atoll( position_str );
			if ( ! ( position_str = j->Attribute( "trimB" ) ) )
				continue;
			trimB = atoll( position_str );
			if ( ! ( position_str = j->Attribute( "length" ) ) )
				continue;
			length = atoll( position_str );
			
			strlcpy( filename, j->Attribute( "filename" ), sizeof(filename) );
			if ( ! filename ) //TODO is this correct?
				continue;
			j->Attribute( "mute", &mute );
			VideoEffectClip* vec = 0;
			if ( strcmp( filename, "TitleClip" ) == 0 ) {
				TitleClip* c = new TitleClip( tr, position, length - trimA - trimB, -1 );
				vec = c;
				const char* textp;
				double x;
				double y;
				int size;
				int font;
				int color;
				if ( ( textp = j->Attribute( "text" ) ) ) {
					c->text( textp );
				}
				if ( j->Attribute( "x", &x ) ) {
					c->x( x );
				}
				if ( j->Attribute( "y", &y ) ) {
					c->y( y );
				}
				if ( j->Attribute( "size", &size ) ) {
					c->size( size );
				}
				if ( j->Attribute( "font", &font ) ) {
					c->font( font );
				}
				if ( j->Attribute( "color", &color ) ) {
					c->color( (Fl_Color)color );
				}
				TiXmlElement* effectXml = TiXmlHandle( j ).FirstChildElement( "filter" ).Element();
				for( ; effectXml; effectXml = effectXml->NextSiblingElement( "filter" ) ) {
					FilterFactory* ef = g_mainFilterFactory->get( effectXml->Attribute( "identifier" ) );
					if ( ef ) {
						FilterBase* effectObj = c->appendFilter( ef );
						effectObj->readXML( effectXml );
						
					}
				}
				g_timeline->addClip( trackId, c );
			} else {
				IVideoFile* vf = VideoFileFactory::get( filename );
				if ( vf ) {
					VideoClip* c = new VideoClip( tr, position, vf, trimA, trimB, -1 );
					vec = c;
					c->m_mute = mute;
					TiXmlElement* filterXml = TiXmlHandle( j ).FirstChildElement( "filter" ).Element();
					for ( ; filterXml; filterXml = filterXml->NextSiblingElement( "filter" ) ) {
						FilterFactory* ff = g_mainFilterFactory->get( filterXml->Attribute( "identifier" ) );//g_audioVolumeFilterFactory;
						if ( ff ) {
							FilterBase* filter = c->appendFilter( ff );
							filter->readXML( filterXml );
						}
					}
					g_timeline->addClip( trackId, c );
				} else {
					ImageClip* ic = new ImageClip( tr, position, filename, length - trimA - trimB, -1 );
					vec = ic;
					if ( !ic->ok() ) {
						delete ic;
						if ( length > 0 ) {
							Clip* c = new DummyClip( tr, filename, position, length+trimA+trimB, trimA, trimB );
							g_timeline->addClip( trackId, c );
						}
					} else {

						//TODO: This is copy and paste
						TiXmlElement* effectXml = TiXmlHandle( j ).FirstChildElement( "filter" ).Element();
						for( ; effectXml; effectXml = effectXml->NextSiblingElement( "filter" ) ) {
							FilterFactory* ef = g_mainFilterFactory->get( effectXml->Attribute( "identifier" ) );
							if ( ef ) {
								FilterBase* effectObj = ic->appendFilter( ef );
								effectObj->readXML( effectXml );
							}
						}

						g_timeline->addClip( trackId, ic );
					}
				}
			}
			if ( vec ) {
				const char* render;
				render = j->Attribute( "render" );
				if ( !render ) {
				} else if ( strcmp( render, "default" ) == 0 ) {
					vec->def( true );
				} else if ( strcmp( render, "crop" ) == 0 ) {
					vec->crop( true );
				} else if ( strcmp( render, "fit" ) == 0 ) {
					vec->fit( true );
				} else if ( strcmp( render, "stretch" ) == 0 ) {
					vec->stretch( true );
				}
			}

		}
		tr->reconsiderFadeOver();
	}
	track = docH.FirstChild( "open_movie_editor_project" ).FirstChild( "audio_tracks" ).FirstChild( "track" ).Element();
	for ( ; track; track = track->NextSiblingElement( "track" ) ) {
		trackId = getTrackId();
		Track *tr = new AudioTrack( trackId );
		g_timeline->addTrack( tr );
		const char* name = track->Attribute( "name" );
		if ( name ) {
			tr->name( name );
		}
		int height = 30;
		if ( track->Attribute( "height", &height ) ) {
			tr->h( height );
		}
		
		TiXmlElement* j = TiXmlHandle( track ).FirstChildElement( "clip" ).Element();
		for ( ; j; j = j->NextSiblingElement( "clip" ) ) {
			int64_t position;
			int64_t trimA;
			int64_t trimB;
			int64_t length;
			char filename[1024];
			const char* position_str;
			if ( ! ( position_str = j->Attribute( "position" ) ) )
				continue;
			position = atoll( position_str );
			if ( ! ( position_str = j->Attribute( "trimA" ) ) )
				continue;
			trimA = atoll( position_str );
			if ( ! ( position_str = j->Attribute( "trimB" ) ) )
				continue;
			trimB = atoll( position_str );
			if ( ! ( position_str = j->Attribute( "length" ) ) )
				continue;
			length = atoll( position_str );

			
			
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

			TiXmlElement* filterXml = TiXmlHandle( j ).FirstChildElement( "filter" ).Element();
			for ( ; filterXml; filterXml = filterXml->NextSiblingElement( "filter" ) ) {
				FilterFactory* ff = g_mainFilterFactory->get( filterXml->Attribute( "identifier" ) );
				if ( ff ) {
					FilterBase* filter = ac->appendFilter( ff );
					filter->readXML( filterXml );
				}
			}
			g_timeline->addClip( trackId, clip );
		}
	}
	g_timelineView->redraw();
	g_timelineView->adjustScrollbar();
	return 1;
}
void Project::write_project()
{
}

void Project::read_project()
{



}

} /* namespace nle */
