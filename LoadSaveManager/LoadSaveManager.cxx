/*  LoadSaveManager.cxx
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

#include <algorithm>

#include <FL/filename.H>
#include <FL/Fl_Choice.H>

#include <tinyxml.h>

#include "LoadSaveManager.H"
#include "Prefs.H"
#include "Timeline.H"
#include "VideoTrack.H"
#include "AudioTrack.H"
#include "globals.H"
#include "Project.H"
#include "SaveAsDialog.H"
#include "TimelineView.H"

namespace nle
{
static string name_to_filename( string name );
static string name_from_projectfile( string filename );

static string home( getenv( "HOME" ) );
LoadSaveManager* g_loadSaveManager;

LoadSaveManager::LoadSaveManager( Fl_Choice* projectChoice, Fl_Input* projectInput )
	: m_projectChoice( projectChoice ), m_projectInput( projectInput )
{
	g_loadSaveManager = this;
	int count = 0;
	dirent** list;
	m_video_projects = home + "/Video Projects";
	if ( fl_filename_isdir( m_video_projects.c_str() ) ) {
		count = fl_filename_list( m_video_projects.c_str(), &list );
		m_projectChoice->clear();
		for ( int i = 0; i < count; i++ ) {
			if ( !fl_filename_isdir( string( m_video_projects + "/" + list[i]->d_name ).c_str() ) ) {
				string name = name_from_projectfile( m_video_projects + "/" + list[i]->d_name );
				if ( name != "" ) {
					char* cname = strdup( list[i]->d_name );
					m_projectChoice->add( name.c_str(), 0, 0, cname );
				}
			}
		}
		for ( int i = count; i > 0; ) { // This is some bad ass hacking style from the fltk manual ;)
			free( (void*)( list[--i] ) );
		}
		free( (void*)list );
	}
//	cout << name_from_projectfile( "/home/oracle/.openme.project" ) << endl;
//	cout << name_to_filename( "Hello World 2" ) << endl;
}
LoadSaveManager::~LoadSaveManager()
{
}
static string name_from_projectfile( string filename )
{
	TiXmlDocument doc( filename.c_str() );
	if ( !doc.LoadFile() ) {
		return "";
	}
	TiXmlHandle docH( &doc );
	TiXmlText *name = docH.FirstChild( "open_movie_editor_project" ).FirstChild( "name" ).FirstChild().Text();
	if ( name ) {
		return name->Value();
	}
	return "";
}
static string name_to_filename( string name )
{
	char random_end[7];
	string filename = name;
	char* buffer = strdup( filename.c_str() );
	for ( int i = 0; i < 6; i++ ) {
		random_end[i] = '0' + rand() % 10;
	}
	random_end[6] = '\0';
	
	for ( char* p = buffer; *p != '\0'; p++ ) {
		*p = tolower( *p );
		if ( !( ( *p >= 'a' && *p <= 'z' ) || ( *p >= '0' && *p <= '9' ) ) ) {
			*p = '_';
		}
	}
	filename = buffer;
	filename += "_";
	filename += random_end;
	filename += ".vproj";
	free( buffer );
	return filename;
}
void LoadSaveManager::startup()
{
	m_currentFilename = g_preferences->lastProject();
	if ( m_currentFilename == "" ) {
		m_currentFilename = "default.vproj";
		m_currentName = "New Project";
		m_projectChoice->add( m_currentName.c_str(), 0, 0, strdup( m_currentFilename.c_str() ) );
	} else {
		g_project->read( m_video_projects + "/" + m_currentFilename );
		m_currentName = name_from_projectfile( m_video_projects + "/" + m_currentFilename );
	}
	/* vv  Put this in a function */
	m_projectInput->value( m_currentName.c_str() );
	const Fl_Menu_Item* item;
	item = m_projectChoice->find_item( m_currentName.c_str() );
	if ( item ) {
		m_projectChoice->value( item );
	}
}
void LoadSaveManager::shutdown()
{
	g_project->write( m_video_projects + "/" + m_currentFilename, m_currentName );
	g_preferences->lastProject( m_currentFilename );
	// free all strings from m_projectChoice;
	Fl_Menu_Item* item = (Fl_Menu_Item*)m_projectChoice->menu();
	int i = 0;
	int m = m_projectChoice->size() - 1;
	while ( item && i < m ) {
		if ( item->user_data() ) {
			char* buffer = (char*)item->user_data();
			item->user_data( 0 );
			free( buffer );
		}
		item++;
		i++;
	}
}
void LoadSaveManager::newProject()
{
	g_project->write( m_video_projects + "/" + m_currentFilename, m_currentName );
	m_currentFilename = "new_project.vproj";
	m_currentName = "New Project";
	g_timeline->clear();
	{
		VideoTrack *vt;
		AudioTrack *at;
		vt = new VideoTrack( 0 );
		g_timeline->addTrack( vt );
		vt = new VideoTrack( 1 );
		g_timeline->addTrack( vt );
		at = new AudioTrack( 2 );
		g_timeline->addTrack( at );
		at = new AudioTrack( 3 );
		g_timeline->addTrack( at );
	}
	char* cname = strdup( m_currentFilename.c_str() );
	m_projectChoice->add( m_currentName.c_str(), 0, 0, cname );
	m_projectInput->value( m_currentName.c_str() );
	const Fl_Menu_Item* item;
	item = m_projectChoice->find_item( m_currentName.c_str() );
	if ( item ) {
		m_projectChoice->value( item );
	}
	g_timelineView->redraw();
}
void LoadSaveManager::saveAs()
{
	g_project->write( m_video_projects + "/" + m_currentFilename, m_currentName );
	SaveAsDialog dlg;
	dlg.show();
	while ( dlg.shown() ) {
		Fl::wait();
	}
	if ( dlg.ok() ) {
		m_currentFilename = name_to_filename( dlg.projectName() );
		m_currentName = dlg.projectName();
		g_project->write( m_video_projects + "/" + m_currentFilename, m_currentName );
		char* cname = strdup( m_currentFilename.c_str() );
		m_projectChoice->add( m_currentName.c_str(), 0, 0, cname );
		m_projectInput->value( m_currentName.c_str() );
		const Fl_Menu_Item* item;
		item = m_projectChoice->find_item( m_currentName.c_str() );
		if ( item ) {
			m_projectChoice->value( item );
		}
	}
}
void LoadSaveManager::name( string v )
{
	string new_file = name_to_filename( v );
	if ( access( string( m_video_projects + "/" + m_currentFilename ).c_str(), R_OK | W_OK ) == 0 ) {
		rename( string( m_video_projects + "/" + m_currentFilename ).c_str(), string( m_video_projects + "/" + new_file).c_str() );
	}
	m_currentFilename = new_file;
	m_currentName = v;
	int n = m_projectChoice->value();
	m_projectChoice->replace( n, m_currentName.c_str() );
	m_projectChoice->redraw();
}
void LoadSaveManager::load( string v )
{
	g_project->write( m_video_projects + "/" + m_currentFilename, m_currentName );
	m_currentFilename = v;
	cout << "LOAD: " << m_currentFilename << endl;
	m_currentName = name_from_projectfile( m_video_projects + "/" + m_currentFilename );
	g_project->read( m_video_projects + "/" + m_currentFilename );
	const Fl_Menu_Item* item;
	m_projectInput->value( m_currentName.c_str() );
	item = m_projectChoice->find_item( m_currentName.c_str() );
	if ( item ) {
		m_projectChoice->value( item );
	}
}


} /* namespace nle */
