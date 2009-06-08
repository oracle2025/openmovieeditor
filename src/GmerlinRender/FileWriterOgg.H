/*  FileWriterOgg.H
 *  
 *  Copyright (C) 2009 Richard Spindler <richard.spindler AT gmail.com>
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

#ifndef _FILE_WRITER_OGG_H_
#define _FILE_WRITER_OGG_H_

extern "C" {
#include <gmerlin/plugin.h>
#include <gmerlin/pluginregistry.h>
}
#include "IVideoFileWriter.H"

namespace nle
{

class FileWriterOgg : public IVideoFileWriter
{
	public:
		FileWriterOgg( const char* filename );
		~FileWriterOgg();
		
		void videoQuality( int );
		void audioQuality( float );
		void resolution( int w, int h );
		void done();

		void encodeVideoFrame( LazyFrame* frame );
		void encodeAudioFrame( float* buffer, int frames );
		
		video_format* format();

		bool ok();
	private:
		video_format m_ome_format;
		bg_cfg_registry_t*    m_cfg_registry;
		bg_cfg_registry_t*    m_encoder_registry;
		bg_cfg_section_t*     m_plugin_cfg;
		bg_plugin_registry_t* m_plugin_registry;

		bg_plugin_handle_t*         m_plugin_handle;
		const bg_encoder_plugin_t*  m_encoder;
		void*                       m_instance;
		const bg_parameter_info_t*  m_audio_parameters;
		const bg_parameter_info_t*  m_video_parameters;
		gavl_video_format_t         m_video_format;
		gavl_audio_format_t         m_audio_format;
		gavl_audio_format_t         m_vorbis_audio_format;
		gavl_audio_frame_t*         m_audio_frame;
		gavl_audio_frame_t*         m_vorbis_audio_frame;
		gavl_audio_converter_t*     m_audio_converter;
		int64_t                     m_timestamp;
		int64_t                     m_sample_tics;
		bool                        m_ok;
};

} /* namespace nle */

#endif /* _FILE_WRITER_OGG_H_ */

