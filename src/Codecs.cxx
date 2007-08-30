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
#include <cassert>
#include <sstream>

#include <FL/Fl.H>
#include <FL/Fl_Choice.H>

#include <lqt.h>
#include <lqt_version.h>

#include "sl/sl.h"

#include "Codecs.H"
#include "globals.H"
#include "helper.H"
#include <tinyxml.h>

using namespace std;

namespace nle
{

string escape_slash( string s ) {
	stringstream r;
	int l = s.length();
	for ( int i = 0; i < l; i++ ) {
		if ( s[i] != '/' ) {
			r << s[i];
		} else {
			r << "\\/";
		}
	}
	return r.str();
}

void setAudioCodecMenu( Fl_Choice* menu )
{
	lqt_codec_info_t** info = g_audio_codec_info;
	menu->clear();
	for ( int i = 0; info[i]; i++ ) {
		menu->add( escape_slash(info[i]->long_name).c_str(), 0, 0, info[i] );
	}
}
void setVideoCodecMenu( Fl_Choice* menu )
{
	lqt_codec_info_t** info = g_video_codec_info;
	menu->clear();
	for ( int i = 0; info[i]; i++ ) {
		menu->add( escape_slash(info[i]->long_name).c_str(), 0, 0, info[i] );
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
		string n = "";
		n+=info->encoding_parameters[j].real_name;
		n+=" (";
		n+=info->encoding_parameters[j].name;
		n+=")";
		dialog->parameters_browser->add( n.c_str(), &(info->encoding_parameters[j]) );
	}

}

/*void updateCodecParameter( CodecOptions* dialog, void* data )
{
}*/
void setCodecParameter( CodecOptions* dialog, void* data )
{
	lqt_parameter_info_t* info = (lqt_parameter_info_t*)data;
//	g_renderer->getVideoParameter( info->name );
	lqt_parameter_value_t val;
	if ( dialog->m_audio ) {
		val = dialog->m_codecParams->getAudioParameter( info->name );
	} else {
		val = dialog->m_codecParams->getVideoParameter( info->name );
	}
	switch ( info->type ) {
		case LQT_PARAMETER_INT:
		{
			dialog->parameter_string_input->deactivate();
			dialog->parameter_stringlist_input->deactivate();
			Fl_Value_Input* o = dialog->parameter_int_input;
			o->activate();
#if (LQT_CODEC_API_VERSION & 0xffff) > 6
			o->minimum( info->val_min.val_int );
			o->maximum( info->val_max.val_int );
			if ( info->val_min.val_int == info->val_max.val_int && info->val_max.val_int == 0 ) {
#else
			o->minimum( info->val_min );
			o->maximum( info->val_max );
			if ( info->val_min == info->val_max && info->val_max == 0 ) {
#endif
				o->step(0);
			} else {
				o->step(1);
			}
			o->value( val.val_int );
		}
			break;
		case LQT_PARAMETER_STRING:
		{
			dialog->parameter_int_input->deactivate();
			dialog->parameter_stringlist_input->deactivate();
			Fl_Input* o = dialog->parameter_string_input;
			o->activate();
			o->value( val.val_string );
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
				if ( strcmp( info->stringlist_options[i], val.val_string ) == 0 ) {
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

ParameterValue::ParameterValue( const char* v )
{
	m_type = PV_STRING;
	m_value_string = strdup( v );
	m_value_int = 0;
}
ParameterValue::ParameterValue( int v )
{
	m_type = PV_INT;
	m_value_int = v;
	m_value_string = 0;
}
ParameterValue::~ParameterValue()
{
	if ( PV_STRING == m_type ) {
		free( m_value_string );
	}
}
void ParameterValue::get( lqt_parameter_value_t& val )
{
	if ( PV_STRING == m_type ) {
		free( val.val_string );
		val.val_string = strdup( m_value_string );
	} else {
		val.val_int = m_value_int;
	}
}

CodecParameters::CodecParameters( lqt_codec_info_t** audio, lqt_codec_info_t** video )
{
	init( audio, video );
}
void CodecParameters::init( lqt_codec_info_t** audio, lqt_codec_info_t** video )
{
	m_audioCodecs       = 0;
	m_videoCodecs       = 0;
	m_currentAudioCodec = 0;
	m_currentVideoCodec = 0;
	lqt_codec_info_t*   p;
	codec_node*         q;
	param_node*         r;
	for ( int i = 0; audio[i]; i++ ) {
		p = audio[i];
		q = new codec_node;
		q->next = 0;
		q->codecInfo = p;
		q->parameters = 0;
		m_audioCodecs = (codec_node*)sl_push( m_audioCodecs, q );

		for ( int i = 0; i < p->num_encoding_parameters; i++ ) {
			if ( q->codecInfo->encoding_parameters[i].type != LQT_PARAMETER_INT
					&& q->codecInfo->encoding_parameters[i].type != LQT_PARAMETER_STRING
					&& q->codecInfo->encoding_parameters[i].type != LQT_PARAMETER_STRINGLIST )
				continue;
			r = new param_node;
			r->next = 0;
			r->value = q->codecInfo->encoding_parameters[i].val_default;
			r->info = &( q->codecInfo->encoding_parameters[i] );
			if ( r->info->type == LQT_PARAMETER_STRING ) {
				r->value.val_string = strdup( r->value.val_string );
			}
			q->parameters = (param_node*)sl_push( q->parameters, r );
		}
		
	};
	for ( int i = 0; video[i]; i++ ) {
		p = video[i];
		q = new codec_node;
		q->next = 0;
		q->codecInfo = p;
		q->parameters = 0;
		m_videoCodecs = (codec_node*)sl_push( m_videoCodecs, q );

		for ( int i = 0; i < p->num_encoding_parameters; i++ ) {
			if ( q->codecInfo->encoding_parameters[i].type != LQT_PARAMETER_INT
					&& q->codecInfo->encoding_parameters[i].type != LQT_PARAMETER_STRING
					&& q->codecInfo->encoding_parameters[i].type != LQT_PARAMETER_STRINGLIST )
				continue;
			r = new param_node;
			r->next = 0;
			r->value = q->codecInfo->encoding_parameters[i].val_default;
			r->info = &( q->codecInfo->encoding_parameters[i] );
			if ( r->info->type == LQT_PARAMETER_STRING ) {
				r->value.val_string = strdup( r->value.val_string );
			}
			q->parameters = (param_node*)sl_push( q->parameters, r );
		}
		
	};
}
CodecParameters::CodecParameters( lqt_codec_info_t** audio, lqt_codec_info_t** video, CodecParameters* copy )
{
	init( audio, video );
	if ( !copy->m_currentAudioCodec || !copy->m_currentVideoCodec ) {
		return;
	} 
	for ( codec_node* n = m_audioCodecs; n; n = n->next ) {
		if ( strcmp( n->codecInfo->name, copy->m_currentAudioCodec->codecInfo->name ) == 0 ) {
			m_currentAudioCodec = n;
			for ( param_node* p = copy->m_currentAudioCodec->parameters; p; p = p->next ) {
				cout << "set Audio: " << p->info->name << endl;
				if ( p->info->type == LQT_PARAMETER_STRING || p->info->type == LQT_PARAMETER_STRINGLIST ) {
					ParameterValue pval( p->value.val_string );
					setAudioParameter( p->info->name, pval );
				} else {
					ParameterValue pval( p->value.val_int );
					setAudioParameter( p->info->name, pval );
				}
			}
			break;
		}
	}
	for ( codec_node* n = m_videoCodecs; n; n = n->next ) {
		if ( strcmp( n->codecInfo->name, copy->m_currentVideoCodec->codecInfo->name ) == 0 ) {
			m_currentVideoCodec = n;
			for ( param_node* p = copy->m_currentVideoCodec->parameters; p; p = p->next ) {
				cout << "set Video: " << p->info->name << endl;
				if ( p->info->type == LQT_PARAMETER_STRING || p->info->type == LQT_PARAMETER_STRINGLIST ) {
					ParameterValue pval( p->value.val_string );
					setVideoParameter( p->info->name, pval );
				} else {
					ParameterValue pval( p->value.val_int );
					setVideoParameter( p->info->name, pval );
				}
			}
			break;
		}
	}
}

CodecParameters::~CodecParameters()
{
	codec_node* q;
	param_node* r;
	while ( ( q = (codec_node*)sl_pop( &m_audioCodecs ) ) ) {
		while ( ( r = (param_node*)sl_pop( &( q->parameters ) ) ) ) {
		 	if ( r->info->type == LQT_PARAMETER_STRING ) {
				free( r->value.val_string );
			}
			delete r; //FIXME: Seqfault here.
		}
		delete q;
	}
	while ( ( q = (codec_node*)sl_pop( &m_videoCodecs ) ) ) {
		while ( ( r = (param_node*)sl_pop( &( q->parameters ) ) ) ) {
		 	if ( r->info->type == LQT_PARAMETER_STRING ) {
				free( r->value.val_string );
			}
			delete r;
		}
		delete q;
	}
}
static int find_a_codec( void* p, void* data )
{
	lqt_codec_info_t* info = (lqt_codec_info_t*)data;
	codec_node* node = (codec_node*)p;
	return ( node->codecInfo == info );
}
void CodecParameters::setVideoCodec( lqt_codec_info_t* info )
{
	codec_node* p = (codec_node*)sl_map( m_videoCodecs, find_a_codec, info );
	m_currentVideoCodec = p;
}
void CodecParameters::setAudioCodec( lqt_codec_info_t* info )
{
	codec_node* p = (codec_node*)sl_map( m_audioCodecs, find_a_codec, info );
	m_currentAudioCodec = p;
}
static int find_a_parameter( void* p, void* data )
{
	param_node* node = (param_node*)p;
	char* key = (char*)data;
	return ( strcmp( node->info->name, key ) == 0 );
}
void CodecParameters::setVideoParameter( const char* key, ParameterValue& value )
{
	param_node* p = (param_node*)sl_map( m_currentVideoCodec->parameters, find_a_parameter, (void*)key );
	if ( p ) {
		value.get( p->value );
	}
}
void CodecParameters::setAudioParameter( const char* key, ParameterValue& value )
{
	param_node* p = (param_node*)sl_map( m_currentAudioCodec->parameters, find_a_parameter, (void*)key );
	if ( p ) {
		value.get( p->value );
	}
}
lqt_parameter_value_t CodecParameters::getVideoParameter( const char* key )
{
	param_node* p = (param_node*)sl_map( m_currentVideoCodec->parameters, find_a_parameter, (void*)key );
	assert( p );
	return p->value;
}
lqt_parameter_value_t CodecParameters::getAudioParameter( const char* key )
{
	param_node* p = (param_node*)sl_map( m_currentAudioCodec->parameters, find_a_parameter, (void*)key );
	assert( p );
	return p->value;
}
void CodecParameters::set( quicktime_t* qt, int w, int h, render_fps_chunks* framerate )
{
	lqt_add_audio_track( qt, 2, 48000, 16, m_currentAudioCodec->codecInfo );
	lqt_add_video_track( qt, w, h, framerate->frame_duration, framerate->timescale, m_currentVideoCodec->codecInfo ); // 30000 / 1200 == 25
	int pixel_w;
	int pixel_h;
	guess_aspect( w, h, 0, 0, 0, 0, &pixel_w, &pixel_h );
	lqt_set_pixel_aspect( qt, 0, pixel_w, pixel_h );

	param_node* p;
	for ( p = m_currentAudioCodec->parameters; p; p = p->next ) {
		if ( p->info->type == LQT_PARAMETER_INT ) {
			lqt_set_audio_parameter( qt, 0, p->info->name, &(p->value.val_int) );
		} else {
			lqt_set_audio_parameter( qt, 0, p->info->name, p->value.val_string );
		}
	}
	
	for ( p = m_currentVideoCodec->parameters; p; p = p->next ) {
		if ( p->info->type == LQT_PARAMETER_INT ) {
			lqt_set_video_parameter( qt, 0, p->info->name, &(p->value.val_int) );
		} else {
			lqt_set_video_parameter( qt, 0, p->info->name, p->value.val_string );
		}
	}
	
}
void CodecParameters::writeXML( TiXmlElement* xml_node )
{
	if ( !m_currentVideoCodec || !m_currentAudioCodec ) {
		return;
	}
	TiXmlElement* video_codec = new TiXmlElement( "video_codec_parameters" );
	TiXmlElement* parameter;
	xml_node->LinkEndChild( video_codec );
	for ( param_node* p = m_currentVideoCodec->parameters; p; p = p->next ) {
		if ( p->info->type == LQT_PARAMETER_INT || p->info->type == LQT_PARAMETER_FLOAT || p->info->type == LQT_PARAMETER_STRING || p->info->type == LQT_PARAMETER_STRINGLIST ) {
			parameter = new TiXmlElement( "parameter" );
			video_codec->LinkEndChild( parameter );
			parameter->SetAttribute( "name", p->info->name );
			switch ( p->info->type ) {
				case LQT_PARAMETER_INT:
					parameter->SetAttribute( "type", "int" );
					parameter->SetAttribute( "value", p->value.val_int );
					break;
				case LQT_PARAMETER_FLOAT:
					parameter->SetAttribute( "type", "float" );
					parameter->SetDoubleAttribute( "value", (double)p->value.val_float );
					break;
				case LQT_PARAMETER_STRING:
				case LQT_PARAMETER_STRINGLIST:
					parameter->SetAttribute( "type", "string" );
					parameter->SetAttribute( "value", p->value.val_string );
					break;
				case LQT_PARAMETER_SECTION:
					break;
			}
		}
	}
	TiXmlElement* audio_codec = new TiXmlElement( "audio_codec_parameters" );
	xml_node->LinkEndChild( audio_codec );
	for ( param_node* p = m_currentAudioCodec->parameters; p; p = p->next ) {
		if ( p->info->type == LQT_PARAMETER_INT || p->info->type == LQT_PARAMETER_FLOAT || p->info->type == LQT_PARAMETER_STRING || p->info->type == LQT_PARAMETER_STRINGLIST ) {
			parameter = new TiXmlElement( "parameter" );
			audio_codec->LinkEndChild( parameter );
			parameter->SetAttribute( "name", p->info->name );
			switch ( p->info->type ) {
				case LQT_PARAMETER_INT:
					parameter->SetAttribute( "type", "int" );
					parameter->SetAttribute( "value", p->value.val_int );
					break;
				case LQT_PARAMETER_FLOAT:
					parameter->SetAttribute( "type", "float" );
					parameter->SetDoubleAttribute( "value", (double)p->value.val_float );
					break;
				case LQT_PARAMETER_STRING:
				case LQT_PARAMETER_STRINGLIST:
					parameter->SetAttribute( "type", "string" );
					parameter->SetAttribute( "value", p->value.val_string );
					break;
				case LQT_PARAMETER_SECTION:
					break;
			}
		}
	}

}
void CodecParameters::readXML( TiXmlElement* xml_node )
{
	TiXmlElement* parameter = TiXmlHandle( xml_node ).FirstChildElement( "video_codec" ).Element();
	const char* str;
	if ( parameter ) {
		str = parameter->Attribute( "value" );
		if ( str ) {
			for ( codec_node* n = m_videoCodecs; n; n = n->next ) {
				if ( strcmp( n->codecInfo->name, str ) == 0 ) {
					m_currentVideoCodec = n;
					break;
				}
			}
		} 
	}
	parameter = TiXmlHandle( xml_node ).FirstChildElement( "audio_codec" ).Element();
	if ( parameter ) {
		str = parameter->Attribute( "value" );
		if ( str ) {
			for ( codec_node* n = m_audioCodecs; n; n = n->next ) {
				if ( strcmp( n->codecInfo->name, str ) == 0 ) {
					m_currentAudioCodec = n;
					break;
				}
			}
		} 
	}
	TiXmlHandle x_node(xml_node);
	if ( m_currentVideoCodec ) {
		parameter = x_node.FirstChild( "video_codec_parameters" ).FirstChild( "parameter" ).Element();

		for ( ; parameter; parameter = parameter->NextSiblingElement( "parameter" ) ) {
			const char* name = parameter->Attribute( "name" );
			const char* type = parameter->Attribute( "type" );
			if ( !name || !type ) {
				continue;
			}
			if ( strcmp( type, "int" ) == 0 ) {
				int v = 0;
				if ( parameter->Attribute( "value", &v ) ) {
					ParameterValue pval( v );
					setVideoParameter( name, pval );
				}
			} else if ( strcmp( type, "string" ) == 0 ) {
				const char *v = parameter->Attribute( "value" );
				if ( v ) {
					ParameterValue pval( v );
					setVideoParameter( name, pval );
				}

			}
		}
	}
	if ( m_currentAudioCodec ) {
		parameter = x_node.FirstChild( "audio_codec_parameters" ).FirstChild( "parameter" ).Element();

		for ( ; parameter; parameter = parameter->NextSiblingElement( "parameter" ) ) {
			const char* name = parameter->Attribute( "name" );
			const char* type = parameter->Attribute( "type" );
			if ( !name || !type ) {
				continue;
			}
			if ( strcmp( type, "int" ) == 0 ) {
				int v = 0;
				if ( parameter->Attribute( "value", &v ) ) {
					ParameterValue pval( v );
					setAudioParameter( name, pval );
				}
			} else if ( strcmp( type, "string" ) == 0 ) {
				const char *v = parameter->Attribute( "value" );
				if ( v ) {
					ParameterValue pval( v );
					setAudioParameter( name, pval );
				}

			}
		}
	}

}

} /* namespace nle */

