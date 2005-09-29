/*  Codecs.cxx
 *
 *  Copyright (C) 2003 Richard Spindler <richard.spindler AT gmail.com>
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

#include <FL/Fl.H>
#include <FL/Fl_Choice.H>

#include <lqt.h>

#include "Codecs.H"
#include "globals.H"

using namespace std;

namespace nle
{

void setAudioCodecMenu( Fl_Choice* menu )
{
	lqt_codec_info_t** info = g_audio_codec_info;
	menu->clear();
	for ( int i = 0; info[i]; i++ ) {
		menu->add( info[i]->long_name, 0, 0, info[i] );
		for ( int j = 0; j < info[i]->num_encoding_parameters; j++ ) {
			cout << info[i]->name << ": \"" << info[i]->encoding_parameters[j].real_name << "\"" << endl;
		}
	}
}
void setVideoCodecMenu( Fl_Choice* menu )
{
	lqt_codec_info_t** info = g_video_codec_info;
	menu->clear();
	for ( int i = 0; info[i]; i++ ) {
		menu->add( info[i]->long_name, 0, 0, info[i] );
		for ( int j = 0; j < info[i]->num_encoding_parameters; j++ ) {
			cout << info[i]->name << ": \"" << info[i]->encoding_parameters[j].real_name << "\"" << endl;
		}
	}
}

void setCodecParameters( CodecOptions* dialog, void* data )
{
	lqt_codec_info_t* info = (lqt_codec_info_t*)data;
	dialog->parameters_browser->clear();
	for ( int j = 0; j < info->num_encoding_parameters; j++ ) {
		dialog->parameters_browser->add( info->encoding_parameters[j].real_name );
	}

}

} /* namespace nle */

