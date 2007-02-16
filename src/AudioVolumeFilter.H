/*  AudioVolumeFilter.H
 *
 *  Copyright (C) 2007 Richard Spindler <richard.spindler AT gmail.com>
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

#ifndef _AUDIO_VOLUME_FILTER_H_
#define _AUDIO_VOLUME_FILTER_H_

#include <stdint.h>

namespace nle
{

class DragHandler;

class AudioVolumeFilter
{
	public:
		AudioVolumeFilter( AudioClipBase* clip );
		~AudioVolumeFilter();
		void onDraw();
		DragHandler* onMouseDown();
		int fillBuffer( float* input_output, unsigned long frames, int64_t position );
		//void writeXML( xml_node );
		//void readXML( xml_node );
		//void setParams();
		//void getParams();
		void reset();
		void trimA( int64_t trim );
		void trimB( int64_t trim );
	private:
		float getEnvelope( int64_t position );
		auto_node* m_automationPoints;
		auto_node* m_autoCache;
		AudioClipBase* m_audioClip;
};

} /* namespace nle */


#endif /* _AUDIO_VOLUME_FILTER_H_ */

