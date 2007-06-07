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

typedef struct
{
	int w;
	int h;
	int aspect_w;
	int aspect_h;
	float aspect;
	int analog_blank;
} render_frame_size;

typedef struct
{
	int frame_duration;
	int timescale;
	int audio_frames_per_chunk;
	int video_frames_per_chunk;
} render_fps_chunks;


extern render_fps_chunks fps25x48000;
extern render_fps_chunks fps29_97x48000;
extern render_fps_chunks fps24x48000;
extern render_fps_chunks fps15x48000;
extern render_fps_chunks fps50x48000;
extern render_fps_chunks fps60x48000;

extern render_frame_size fs720x576;
extern render_frame_size fs768x576;
extern render_frame_size fs384x288;
extern render_frame_size fs640x480;
extern render_frame_size fs320x240;

namespace nle
{

class IProgressListener;
class CodecParameters;
class Timeline;


class Renderer
{
	public:
		Renderer( string filename, render_frame_size* format, render_fps_chunks* framerate, int samplerate, CodecParameters* params = 0 );
		~Renderer();
		void go( IProgressListener* l = 0 );
		bool ok();
	private:
		string m_filename;
		int m_w;
		int m_h;
		int m_samplerate;
		Timeline* p_timeline;
		render_fps_chunks m_fps;
};

} /* namespace nle */
#endif /* _RENDERER_H_ */

