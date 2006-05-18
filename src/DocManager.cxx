/*  DocManager.cxx
 *
 *  Copyright (C) 2006 Richard Spindler <richard.spindler AT gmail.com>
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

#include "sl/sl.h"

#include "DocManager.H"
#include "Command.H"


namespace nle
{

DocManager::DocManager()
{
	m_undoList = 0;
	m_redoList = 0;
}

void clear_command_list( command_node** l )
{
	command_node* n;
	while ( ( n = (command_node*)sl_pop( l ) ) ) {
		delete n->command;
		delete n;
	}
}

DocManager::~DocManager()
{
	clear_command_list( &m_undoList );
	clear_command_list( &m_redoList );
}

void DocManager::undo()
{
	if ( !canUndo() ) {
		return;
	}
	command_node* n = (command_node*)sl_pop( &m_undoList );
	n->command->undo();
	m_redoList = (command_node*)sl_push( m_redoList, n );
}

void DocManager::redo()
{
	if ( !canRedo() ) {
		return;
	}
	command_node* n = (command_node*)sl_pop( &m_redoList );
	n->command->redo();
	m_undoList = (command_node*)sl_push( m_undoList, n );
}

command_node* new_command_node( Command* cmd )
{
	command_node* n = new command_node;
	n->command = cmd;
	n->next = 0;
	return n;
}

void DocManager::submit( Command* cmd )
{
	cmd->doo();
	m_undoList = (command_node*)sl_push( m_undoList, new_command_node( cmd ) );
	clear_command_list( &m_redoList );
}

bool DocManager::canUndo()
{
	return ( m_undoList != 0 );
}

bool DocManager::canRedo()
{
	return ( m_redoList != 0 );
}

} /* namespace nle */

