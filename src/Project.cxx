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
#include "IVideoEffect.H"
#include "Frei0rEffect.H"
#include "Frei0rFactory.H"
#include "VideoFileFactory.H"

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
			} else if ( VideoEffectClip* vc = dynamic_cast<VideoEffectClip*>(cn->clip) ) {
				if ( VideoClip* vc = dynamic_cast<VideoClip*>(cn->clip) ) {
					clip->SetAttribute( "mute", (int)vc->m_mute );
				}

				for ( effect_stack* p = vc->getEffects(); p; p = p->next ) {
					//TODO: Store Effects Settings
					TiXmlElement* effect = new TiXmlElement( "effect" );
					TiXmlElement* parameter;
					clip->LinkEndChild( effect );
					effect->SetAttribute( "name", p->effect->name() );
					Frei0rEffect* fe = dynamic_cast<Frei0rEffect*>( p->effect );
					f0r_plugin_info_t* finfo;
					f0r_param_info_t pinfo;
					finfo = fe->getPluginInfo();
					for ( int i = 0; i < finfo->num_params; i++ ) {
						fe->getParamInfo( &pinfo, i );
						parameter = new TiXmlElement( "parameter" );
						effect->LinkEndChild( parameter );
						parameter->SetAttribute( "name", pinfo.name );
						switch ( pinfo.type ) {
							case F0R_PARAM_DOUBLE: //Seems to be always between 0.0 and 1.0
								{
									f0r_param_double dvalue;
									fe->getValue( &dvalue, i );
									parameter->SetDoubleAttribute( "value", (double)dvalue );
									break;
								}
							case F0R_PARAM_BOOL:
								{
									f0r_param_bool bvalue;
									fe->getValue( &bvalue, i );
									parameter->SetAttribute( "value", (int)bvalue );
									break;
								}
							case F0R_PARAM_COLOR:
								break;
							case F0R_PARAM_POSITION:
								{
									f0r_param_position_t pos;
									fe->getValue( &pos, i );
									parameter->SetDoubleAttribute( "x", pos.x );
									parameter->SetDoubleAttribute( "y", pos.y );
									break;
								}
							default:
								break;
							
						}
					}
				}
			}
			cn = cn->next;
		}
		node = node->next;
	}

	
	doc.SaveFile();
	
	return 1;
}
extern Frei0rFactory* g_frei0rFactory;
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
		const char* name = track->Attribute( "name" );
		if ( name ) {
			tr->name( name );
		}
		
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
//			g_timeline->addFile( trackId, position, filename, trimA, trimB, mute, -1, length );
			//TODO: insert Effects
			IVideoFile* vf = VideoFileFactory::get( filename );
			if ( vf ) {
				VideoClip* c = new VideoClip( tr, position, vf, trimA, trimB, -1 );
				c->m_mute = mute;
				TiXmlElement* effectXml = TiXmlHandle( j ).FirstChildElement( "effect" ).Element();
				for( ; effectXml; effectXml = effectXml->NextSiblingElement( "effect" ) ) {
					//addClip would propably be a better Idea
					AbstractEffectFactory* ef = g_frei0rFactory->get( effectXml->Attribute( "name" ) );
					if ( ef ) {
						Frei0rEffect* effectObj = dynamic_cast<Frei0rEffect*>( c->appendEffect( ef ) );
						TiXmlElement* parameterXml = TiXmlHandle( effectXml ).FirstChildElement( "parameter" ).Element();
						for ( ; parameterXml; parameterXml = parameterXml->NextSiblingElement( "parameter" ) ) {
							string paramName = parameterXml->Attribute( "name" );
							f0r_plugin_info_t* finfo = effectObj->getPluginInfo();
							f0r_param_info_t pinfo;
							for ( int i = 0; i < finfo->num_params; i++ ) {
								effectObj->getParamInfo( &pinfo, i );
								if ( paramName == pinfo.name ) {
									switch ( pinfo.type ) {
										case F0R_PARAM_DOUBLE:
											{
												double dval;
												f0r_param_double dvalue;
												parameterXml->Attribute( "value", &dval );
												dvalue = dval;
												effectObj->setValue( &dvalue, i );
												break;
											}
										case F0R_PARAM_BOOL:
											{
												//int bval;
												break;
											}
									}
									break;
								}
							}
						}
					}
				}
				g_timeline->addClip( trackId, c );
			} else {
				ImageClip* ic = new ImageClip( tr, position, filename, length - trimA - trimB, -1 );
				if ( !ic->ok() ) {
					delete ic;
					if ( length > 0 ) {
						Clip* c = new DummyClip( tr, filename, position, length+trimA+trimB, trimA, trimB );
						g_timeline->addClip( trackId, c );
					}
				} else {

					//TODO: This is copy and paste
					TiXmlElement* effectXml = TiXmlHandle( j ).FirstChildElement( "effect" ).Element();
					for( ; effectXml; effectXml = effectXml->NextSiblingElement( "effect" ) ) {
						//addClip would propably be a better Idea
						AbstractEffectFactory* ef = g_frei0rFactory->get( effectXml->Attribute( "name" ) );
						if ( ef ) {
							Frei0rEffect* effectObj = dynamic_cast<Frei0rEffect*>( ic->appendEffect( ef ) );
							TiXmlElement* parameterXml = TiXmlHandle( effectXml ).FirstChildElement( "parameter" ).Element();
							for ( ; parameterXml; parameterXml = parameterXml->NextSiblingElement( "parameter" ) ) {
								string paramName = parameterXml->Attribute( "name" );
								f0r_plugin_info_t* finfo = effectObj->getPluginInfo();
								f0r_param_info_t pinfo;
								for ( int i = 0; i < finfo->num_params; i++ ) {
									effectObj->getParamInfo( &pinfo, i );
									if ( paramName == pinfo.name ) {
										switch ( pinfo.type ) {
											case F0R_PARAM_DOUBLE:
												{
													double dval;
													f0r_param_double dvalue;
													parameterXml->Attribute( "value", &dval );
													dvalue = dval;
													effectObj->setValue( &dvalue, i );
													break;
												}
											case F0R_PARAM_BOOL:
												{
													//int bval;
													break;
												}
										}
										break;
									}
								}
							}
						}
					}
					
					g_timeline->addClip( trackId, ic );
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
	return 1;
}
void Project::write_project()
{
}

void Project::read_project()
{



}

} /* namespace nle */
