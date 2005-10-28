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

#include <iostream>
#include <cstring>
#include <inttypes.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "Project.H"
#include "globals.H"
#include "timeline/Track.H"
#include "Timeline.H"
#include "timeline/Clip.H"
#include "VideoTrack.H"
#include "AudioTrack.H"


using namespace std;

namespace nle
{

void Project::write_project()
{
	
	
}

void Project::read_project()
{

}

} /* namespace nle */
