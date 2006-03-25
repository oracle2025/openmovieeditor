/* UniPlaybackCore.cxx
 *
 *  Copyright (C) 2006 Richard Spindler <richard.spindler AT gmail.com>
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


#include <FL/Fl.H>
#include <FL/Fl_Button.H>

#include "global_includes.H"
#include "globals.H"
#include "UniPlaybackCore.H"
#include "IAudioReader.H"
#include "IVideoReader.H"
#include "IVideoWriter.H"
#include "Timeline.H"
#include "nle_audio.h"

#define FRAMES 2048

namespace nle
{

static void audio_callback( int p, void* data )
{
	UniPlaybackCore* pc = (UniPlaybackCore*)data;
	pc->writeAudio();
}
static void flip_video_callback( void* data )
{
	UniPlaybackCore* pc = (UniPlaybackCore*)data;
	pc->flipVideo();
}
static void read_video_callback( void* data )
{
	UniPlaybackCore* pc = (UniPlaybackCore*)data;
	pc->readVideo();
}



UniPlaybackCore* g_uniPlaybackCore;
UniPlaybackCore::UniPlaybackCore( IAudioReader* audioReader, IVideoReader* videoReader, IVideoWriter* videoWriter )
	: m_audioReader(audioReader), m_videoReader(videoReader), m_videoWriter(videoWriter)
{
	m_active = false;
	m_handle = prepare_audio();
	m_fd = 0;
	m_fs = 0;
	g_uniPlaybackCore = this;
	if ( !m_fd ) {
		m_fd = get_audio_poll_fd( m_handle );
	}
}
UniPlaybackCore::~UniPlaybackCore()
{
	end_audio( m_handle );
}
void UniPlaybackCore::play()
{
	if ( m_active ) {
		return;
	}
	m_videoPosition = g_timeline->m_seekPosition;
	m_nextVideoFrame = g_timeline->m_seekPosition;
	m_audioPosition = m_videoPosition * ( 48000 / 25 );
	play_audio( m_handle );
	if ( !m_fd ) {
		m_fd = get_audio_poll_fd( m_handle );
	}
	cout << "FD: " << m_fd << endl;
	m_active = true;
	Fl::add_fd( m_fd, audio_callback, this );
	while ( 1 )
		writeAudio();
}
void UniPlaybackCore::stop()
{
	if ( !m_active ) {
		return;
	}
	stop_audio( m_handle );
	Fl::remove_fd( m_fd );
	m_active = false;
}
void UniPlaybackCore::writeAudio()
{
	static float buffer[2 * FRAMES];
	unsigned long r = FRAMES;
	int avail = get_available_frames( m_handle );
	avail = 16384;
	while ( avail >= FRAMES && r == FRAMES ) {
		r = m_audioReader->fillBuffer( buffer, FRAMES );
		m_audioPosition += r;
		write_audio_frames( m_handle, buffer, FRAMES );
		avail -= r;
	}
	if ( m_audioPosition / ( 48000 / 25 ) > m_videoPosition ) {
		m_nextVideoFrame++;
		void* a = (void*)flip_video_callback;
		a = 0;
		//Fl::add_check( flip_video_callback, this );
	}
	if ( r < FRAMES ) {
		stop();
		g_playButton->label( "@>" );
	}
}
void UniPlaybackCore::flipVideo()
{
	if ( m_fs ) {
		m_videoWriter->pushFrameStack( m_fs );
	}
	Fl::add_check( read_video_callback, this );
}
void UniPlaybackCore::readVideo()
{
	Fl::remove_check( read_video_callback, this );
	if ( m_nextVideoFrame == m_videoPosition ) {
		return;
	}
	m_fs = m_videoReader->getFrameStack( m_nextVideoFrame );
	m_videoPosition = m_nextVideoFrame;
}

} /* namespace nle */
