/*  Renderer.H
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

#ifndef _RENDERER_H_
#define _RENDERER_H_

#include <lqt.h>
#include "global_includes.H"

namespace nle
{

class IProgressListener;

typedef struct _param_list_node {
	struct _param_list_node *next;
	char key[1024];
	lqt_parameter_value_t value;
} param_node;

class Renderer
{
	public:
		Renderer( string filename, int w, int h, int framerate, int samplerate );
		~Renderer();
		inline void setVideoCodec( lqt_codec_info_t* info ) { m_videoCodec = info }
		inline void setAudioCodec( lqt_codec_info_t* info ) { m_audioCodec = info }
		void setVideoParameter( const char* key, lqt_parameter_value_t* value );
		void setAudioParameter( const char* key, lqt_parameter_value_t* value );
		const lqt_parameter_value_t* getVideoParameter( const char* key );
		const lqt_parameter_value_t* getAudioParameter( const char* key );
		void go( IProgressListener* l = 0 );
	private:
		string m_filename;
		int m_w;
		int m_h;
		int m_framerate;
		int m_samplerate;
		param_node* m_audioParams;
		param_node* m_videoParams;
		lqt_codec_info_t* m_videoCodec;
		lqt_codec_info_t* m_audioCodec;
};

} /* namespace nle */
#endif /* _RENDERER_H_ */

