/* DocManager.cxx
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

#include "DocManager.H"
#include "Command.H"
#include <sl.h>

using namespace std;

namespace gone
{

DocManager::DocManager()
{
	m_undoList = 0;
	m_redoList = 0;
}
DocManager::~DocManager()
{
	Reset();
}
void DocManager::SubmitCommand( Command *command )
{
	if ( ! command )
		return;
	command->Do();
	if ( command->GetError() ) {
		delete command;
		return;
	}
	command_list_node* node = new command_list_node;
	node->command = command;
	m_undoList = (command_list_node*)sl_push( m_undoList, node );
	while ( ( node = (command_list_node*)sl_pop( &m_redoList ) ) ) {
		delete node->command;
		delete node;
	}
}
void DocManager::Undo()
{
	command_list_node* node = (command_list_node*)sl_pop( &m_undoList );
	if ( ! node )
		return;
	node->command->Undo();
	m_redoList = (command_list_node*)sl_push( m_redoList, node );
}
void DocManager::Redo()
{
	command_list_node* node = (command_list_node*)sl_pop( &m_redoList );
	if ( ! node )
		return;
	node->command->Undo();
	m_undoList = (command_list_node*)sl_push( m_undoList, node );
}
bool DocManager::CanUndo()
{
	return m_undoList != 0;
}
bool DocManager::CanRedo()
{
	return m_redoList != 0;
}
void DocManager::Reset()
{
	command_list_node* node;
	while ( ( node = (command_list_node*)sl_pop( &m_undoList ) ) ) {
		delete node->command;
		delete node;
	}
	while ( ( node = (command_list_node*)sl_pop( &m_redoList ) ) ) {
		delete node->command;
		delete node;
	}
}

} /* namespace gone */

