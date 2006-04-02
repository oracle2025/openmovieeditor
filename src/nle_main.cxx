/*  nle_main.cxx
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

#include <FL/Fl.H>
#include <FL/Fl_Tooltip.H>
#include <FL/Fl_Shared_Image.H>

#include <lqt.h>

#include "nle.h"
#include "Prefs.H"
#include "WavArtist.H"
#include "Project.H"
#include "SimplePlaybackCore.H"
#include "globals.H"
#include "Timeline.H"
#include "VideoViewGL.H"
#include "LoadSaveManager/LoadSaveManager.H"
#include "Project.H"
#include "ErrorDialog/FltkErrorHandler.H"
#include "FilmStripFactory.H"

namespace nle 
{
	lqt_codec_info_t** g_audio_codec_info;
	lqt_codec_info_t** g_video_codec_info;
	NleUI* g_ui;
	bool g_SEEKING;
} /* namespace nle */

int main( int argc, char** argv )
{
	fl_register_images();
	nle::g_SEEKING = false;
	srand( time( 0 ) );
	nle::FltkErrorHandler e;
	nle::Project project;
	nle::Timeline timeline;
	nle::Prefs preferences;
	nle::WavArtist wavArtist;
	NleUI nui;
	nle::g_ui = &nui;
	nle::SimplePlaybackCore playbackCore( nle::g_timeline, nle::g_timeline, nle::g_videoView );
	//nle::PlaybackCore playbackCore( nle::g_timeline, nle::g_timeline, nle::g_videoView );
	nle::LoadSaveManager lsm( nui.projectChoice, nui.projectNameInput );
	nle::FilmStripFactory filmStripFactory;

	Fl::visual(FL_DOUBLE|FL_RGB);

	Fl_Tooltip::color( fl_rgb_color( 0, 0, 1 ) );
	Fl::background2( 34, 52, 103 );
	Fl::background( 93, 93, 114 );
	Fl::foreground( 255, 255, 255 );
	Fl::set_color( FL_BLACK, 200, 200, 200 );

//	Fl::lock();
	
	nle::g_audio_codec_info = lqt_query_registry( 1, 0, 1, 0 );
	nle::g_video_codec_info = lqt_query_registry( 0, 1, 1, 0 );

	nui.show( argc, argv );
	lsm.startup();
	int r = Fl::run();
	lsm.shutdown();
	return r;
}
