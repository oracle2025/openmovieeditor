/*  EnvelopeClip.H
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

#ifndef _ENVELOPE_CLIP_H_
#define _ENVELOPE_CLIP_H_

#include <stdint.h>


namespace nle
{

class AudioClipBase;

typedef struct _auto_node auto_node;
	

class EnvelopeClip
{
	public:
		EnvelopeClip( AudioClipBase* audioClip );
		~EnvelopeClip();
		inline auto_node* getAutoPoints() { return m_automationPoints; }
		inline void setAutoPoints( auto_node* a ) { m_automationPoints = a; }
		int fillBuffer( float* output, unsigned long frames, int64_t position );
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

#endif /* _ENVELOPE_CLIP_H_ */
