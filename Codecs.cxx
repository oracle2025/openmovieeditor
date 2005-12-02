/*  Codecs.cxx
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
	}
}
void setVideoCodecMenu( Fl_Choice* menu )
{
	lqt_codec_info_t** info = g_video_codec_info;
	menu->clear();
	for ( int i = 0; info[i]; i++ ) {
		menu->add( info[i]->long_name, 0, 0, info[i] );
	}
}

void setCodecInfo( CodecOptions* dialog, void* data )
{
	lqt_codec_info_t* info = (lqt_codec_info_t*)data;
	dialog->codec_label->label( info->long_name );
	dialog->parameters_browser->clear();
	for ( int j = 0; j < info->num_encoding_parameters; j++ ) {
		if ( info->encoding_parameters[j].type == LQT_PARAMETER_SECTION )
			continue;
		dialog->parameters_browser->add( info->encoding_parameters[j].real_name, &(info->encoding_parameters[j]) );
	}

}

void setCodecParameter( CodecOptions* dialog, void* data )
{
	lqt_parameter_info_t* info = (lqt_parameter_info_t*)data;
//	g_renderer->getVideoParameter( info->name );
	switch ( info->type ) {
		case LQT_PARAMETER_INT:
		{
			dialog->parameter_string_input->deactivate();
			dialog->parameter_stringlist_input->deactivate();
			Fl_Value_Input* o = dialog->parameter_int_input;
			o->activate();
			o->minimum( info->val_min );
			o->maximum( info->val_max );
			if ( info->val_min == info->val_max && info->val_max == 0 ) {
				o->step(0);
			} else {
				o->step(1);
			}
			o->value( info->val_default.val_int );
		}
			break;
		case LQT_PARAMETER_STRING:
		{
			dialog->parameter_int_input->deactivate();
			dialog->parameter_stringlist_input->deactivate();
			Fl_Input* o = dialog->parameter_string_input;
			o->activate();
			o->value( info->val_default.val_string );
		}
			break;
		case LQT_PARAMETER_STRINGLIST:
		{
			dialog->parameter_int_input->deactivate();
			dialog->parameter_string_input->deactivate();
			Fl_Choice* o = dialog->parameter_stringlist_input;
			o->activate();
			o->clear();
			for ( int i = 0; i < info->num_stringlist_options; i++ ) {
				o->add( info->stringlist_options[i] );
				if ( strcmp( info->stringlist_options[i], info->val_default.val_string ) == 0 ) {
					o->value( i );
				}
			}
		}
			break;
		case LQT_PARAMETER_SECTION:
		default:
			dialog->parameter_int_input->deactivate();
			dialog->parameter_string_input->deactivate();
			dialog->parameter_stringlist_input->deactivate();
			break;
	}
	
}

} /* namespace nle */

