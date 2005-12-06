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

#include <FL/filename.H>
#include <FL/Fl_Choice.H>

#include "LoadSaveManager.H"
#include "Prefs.H"
#include "Timeline.H"
#include "globals.H"

namespace nle
{

static string home( getenv( "HOME" ) );
LoadSaveManager* g_loadSaveManager;

LoadSaveManager::LoadSaveManager( Fl_Choice* projectChoice )
	: m_projectChoice( projectChoice )
{
	g_loadSaveManager = this;
	int count = 0;
	dirent** list;
	string video_projects = home + "/Video Projects";
	if ( fl_filename_isdir( video_projects.c_str() ) ) {
		count = fl_filename_list( video_projects.c_str(), &list );
		m_projectChoice->clear();
		for ( int i = 0; i < count; i++ ) {
			if ( !fl_filename_isdir( string( video_projects + "/" + list[i]->d_name ).c_str() ) ) {
				m_projectChoice->add( list[i]->d_name, 0, 0 );
			}
		}
		for ( int i = count; i > 0; ) { // This is some bad ass hacking style from the fltk manual ;)
			free( (void*)( list[--i] ) );
		}
		free( (void*)list );
	}
}
LoadSaveManager::~LoadSaveManager()
{
}
void LoadSaveManager::startup()
{
	m_currentFilename = g_preferences->lastProject();
	//g_project->read( m_currentFilename, m_currentName );
	//Find right m_projectChoice Entry
}
void LoadSaveManager::shutdown()
{
	//g_project->write( m_currentFilename, m_currentName );
	g_preferences->lastProject( m_currentFilename );
}
void LoadSaveManager::newProject()
{
	//g_project->write( m_currentFilename, m_currentName );
	m_currentFilename = "new_project";
	m_currentName = "New Project";
	g_timeline->clear();
}
void LoadSaveManager::saveAs()
{
	//g_project->write( m_currentFilename, m_currentName );
	//open dialog
	//m_currentFilename = new_filename;
	//m_currentName = new_name;
	//g_project->write( m_currentFilename, m_currentName );
}
void LoadSaveManager::name( string v )
{
	cout << v << endl;
	int n = m_projectChoice->value();
	m_currentName = v;
	m_projectChoice->replace( n, m_currentName.c_str() );
	m_projectChoice->value( m_projectChoice->value() );
	m_projectChoice->redraw();
}
void LoadSaveManager::load( string v )
{
	m_currentFilename = v;
	//g_project->read( m_currentFilename, m_currentName );
}


} /* namespace nle */
