/*  Codecs.H
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


#ifndef _CODECS_H_
#define _CODECS_H_

#include "nle.h"

class CodecOptions;
class Fl_Choice;

namespace nle
{

void setAudioCodecMenu( Fl_Choice* menu );
void setVideoCodecMenu( Fl_Choice* menu );

void setCodecInfo( CodecOptions* dialog, void* data );

void setCodecParameter( CodecOptions* dialog, void* data );

typedef struct _param_list_node {
	struct _param_list_node *next;
	lqt_parameter_value_t value;
	lqt_parameter_info_t * info;
} param_node;

typedef struct _codec_list_node {
	struct _codec_list_node *next;
	lqt_codec_info_t* codecInfo;
	param_node* parameters;
} codec_node;

typedef enum {
	PV_INT,
	PV_STRING
} pv_type;

class ParameterValue
{
	public:
		ParameterValue( const char* v );
		ParameterValue( int v );
		~ParameterValue();
		void get( lqt_parameter_value_t& val );
	private:
		pv_type m_type;
		char* m_value_string;
		int m_value_int;
};


class CodecParameters
{
	public:
		CodecParameters( lqt_codec_info_t** audio, lqt_codec_info_t** video );
		CodecParameters( lqt_codec_info_t** audio, lqt_codec_info_t** video, CodecParameters* copy );
		~CodecParameters();
		void writeXML( TiXmlElement* );
		void readXML( TiXmlElement* );
		void setVideoCodec( lqt_codec_info_t* info );
		void setAudioCodec( lqt_codec_info_t* info );
		void setVideoParameter( const char* key, ParameterValue& value );
		void setAudioParameter( const char* key, ParameterValue& value );
		lqt_parameter_value_t getVideoParameter( const char* key );
		lqt_parameter_value_t getAudioParameter( const char* key );
		void set2( quicktime_t* qt );
		void set( quicktime_t* qt, int w, int h, render_fps_chunks* framerate );
	private:
		void init( lqt_codec_info_t** audio, lqt_codec_info_t** video );
		codec_node* m_audioCodecs;
		codec_node* m_videoCodecs;
		codec_node* m_currentAudioCodec;
		codec_node* m_currentVideoCodec;

};


} /* namespace nle */


#endif /* _CODECS_H_ */

