/*  FileSystem.H
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

#ifndef _FILE_SYSTEM_H_
#define _FILE_SYSTEM_H_

#include "global_includes.H"

namespace nle
{

typedef enum {
	FS_FILE,
	FS_FOLDER,
} file_entry_type;
	
typedef struct _file_entry {
	string name;
	file_entry_type type;
} file_entry;

class FileSystem
{
	public:
		FileSystem();
		~FileSystem();
		int list( file_entry*** namelist );
		int listFiles( file_entry*** namelist );
		int listFolders( file_entry*** namelist );
		void cd( const char* folder );
		void up();
		void root();
		void home();
		bool isFolder( const char* name );
		bool isFile( const char* name );
		bool exists( const char* name );
	private:
		string m_current;
};

} /* namespace nle */

#endif /* _FILE_SYSTEM_H_ */
