/*  AudioClipBase.H
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

#ifndef _AUDIO_CLIP_BASE_H_
#define _AUDIO_CLIP_BASE_H_

#include "timeline/Clip.H"

#include "global_includes.H"
#include "Rect.H"

namespace nle
{

class AudioFilter;
class FilterFactory;
class DragHandler;

typedef struct _filter_stack {
	struct _filter_stack* next;
	AudioFilter* filter;
} filter_stack;
	
/*typedef struct _auto_node {
	struct _auto_node *next;
	int64_t x;
	float y;
} auto_node;
*/
class IAudioFile;
/*
 * This is the base class for both, Audio and Video Clips,
 * because Video Clips can handle Audio as well, since most
 * Video Files contain Audio as well, and this should be played
 * back seamlessly.
 */
class AudioClipBase : public Clip
{
	friend class EnvelopeClip;
	public:
		virtual ~AudioClipBase();
		
		virtual int fillBufferRaw( float* output, unsigned long frames, int64_t position );
		virtual int fillBuffer( float* output, unsigned long frames, int64_t position );

		DragHandler* onMouseDown( Rect& rect, int x, int y, bool shift );
		
		virtual void reset();
		bool m_mute;
		virtual int64_t audioTrimA() = 0;
		virtual int64_t audioTrimB() = 0;
		int64_t audioLength();
		virtual int64_t audioPosition() = 0;
		using Clip::trimA;
		using Clip::trimB;
		int64_t trimA( int64_t trim );
		int64_t trimB( int64_t trim );
		
		AudioFilter* appendFilter( FilterFactory* factory );
		void pushFilter( FilterFactory* factory );
		filter_stack* getFilters() { return m_filters; }
		
	protected:
		AudioClipBase( Track *track, int64_t position, IAudioFile* af = 0, int id = -1 );
		string audioFilename();
		IAudioFile* m_audioFile;
	private:
		int fillBufferForEnvelope( float* output, unsigned long frames, int64_t position );
		int64_t m_lastSamplePosition;
		filter_stack* m_filters;
};

} /* namespace nle */


#endif /* _AUDIO_CLIP_BASE_H_ */
