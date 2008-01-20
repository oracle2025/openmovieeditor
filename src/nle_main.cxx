/*  nle_main.cxx
 *
 *  Copyright (C) 2005-2007 Richard Spindler <richard.spindler AT gmail.com>
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

#include "config.h"

#include <sys/stat.h>
#include <sys/types.h>

#include <FL/Fl.H>
#include <FL/Fl_Tooltip.H>
#include <FL/Fl_Shared_Image.H>

#include <lqt.h>
#include <lqt_version.h>

#ifdef AVCODEC
extern "C" {
  #include <ffmpeg/avformat.h>
}
#endif

#include "nle.h"
#include "Prefs.H"
#include "WavArtist.H"
#include "PortAudioPlaybackCore.H"
#include "JackPlaybackCore.H"
#include "globals.H"
#include "Timeline.H"
#include "VideoViewGL.H"
#include "LoadSaveManager/LoadSaveManager.H"
#include "ErrorDialog/FltkErrorHandler.H"
#include "FilmStripFactory.H"
#include "DocManager.H"

#include "Frei0rFactoryPlugin.H"
#include "FltkEffectMenu.H"
#include "Frei0rFactory.H"
#include "ColorCurveFactory.H"
#include "AudioVolumeFilterFactory.H"
#include "NodeFilterFactory.H"
#include "MainFilterFactory.H"

#include "NodeFilterFrei0rFactory.H"

#include <stdlib.h>
#include <stdio.h>
namespace nle 
{
	bool g_PREVENT_OFFSCREEN_CRASH;
	lqt_codec_info_t** g_audio_codec_info;
	lqt_codec_info_t** g_video_codec_info;
	NleUI* g_ui;
	bool g_SEEKING;
	char* g_homefolder;
	bool g_INTERLACING = false;
} /* namespace nle */

static void print_file_if_exists( const char* filename )
{
	FILE *stream;
	stream = fopen( filename, "r" );
	if ( stream ) {
		cout << filename << ":" << endl;
		int c;
		while ( ( c = fgetc( stream ) ) != EOF ) {
			putchar( c );
		}
	}
}

int main( int argc, char** argv )
{
	cout << "When submitting a BUG report, or SUPPORT request, please include the following information:" << endl;
	cout << "----8<-----------------------" << endl;
	cout << "Libquicktime Version: " << LQT_VERSION << endl;
	cout << "Libquicktime API Version: " << (LQT_CODEC_API_VERSION & 0xffff) << endl;
#ifdef AVCODEC
	cout << "libavcodec Version: " << LIBAVCODEC_IDENT<< endl;
	cout << "libavformat Version: " << LIBAVFORMAT_IDENT << endl;
#endif /* AVCODEC */
	print_file_if_exists( "/etc/SUSE-release" );
	print_file_if_exists( "/etc/redhat-release" );
	print_file_if_exists( "/etc/redhat_version" );
	print_file_if_exists( "/etc/fedora-release" );
	print_file_if_exists( "/etc/slackware-release" );
	print_file_if_exists( "/etc/slackware-version" );
	print_file_if_exists( "/etc/debian_release" );
	print_file_if_exists( "/etc/debian_version" );
	print_file_if_exists( "/etc/mandrake-release" );
	print_file_if_exists( "/etc/sun-release" );
	print_file_if_exists( "/etc/release" );
	print_file_if_exists( "/etc/gentoo-release" );
	print_file_if_exists( "/etc/yellowdog-release" );
	print_file_if_exists( "/etc/UnitedLinux-release" );
	print_file_if_exists( "/etc/lsb-release" );
	cout << "----8<-----------------------" << endl;
	/*

/etc/SUSE-release
/etc/redhat-release
/etc/redhat_version
/etc/fedora-release
/etc/slackware-release
/etc/slackware-version
/etc/debian_release
/etc/debian_version
/etc/mandrake-release
/etc/yellowdog-release
/etc/sun-release
/etc/release
/etc/gentoo-release
/etc/UnitedLinux-release
/etc/lsb-release

	*/
	
	nle::g_PREVENT_OFFSCREEN_CRASH = true;
	nle::g_homefolder = getenv( "HOME" );
#ifdef AVCODEC
	av_register_all();
#endif	
	fl_register_images();
	nle::g_SEEKING = false;
	srand( time( 0 ) );
	nle::FltkErrorHandler e;
	nle::Timeline timeline;
	nle::Prefs preferences;
	nle::WavArtist wavArtist;
	NleUI nui;
	nle::g_ui = &nui;
	nui.special_clips->add("Titles", nle::PL_VIDEO_SRC, "src:builtin:TitleClip" );
/*	nui.special_clips->add("Inkscape Title", nle::PL_VIDEO_SRC, "src:builtin:InkscapeClip" );*/
	nui.special_clips->add("Volume Automations", nle::PL_AUDIO_FILTER, "filter:builtin:VolumeAutomations" );
	nui.special_clips->add("Color Curves", nle::PL_VIDEO_EFFECT, "effect:builtin:ColorCurves" );
	nui.special_clips->add("Node Compositing", nle::PL_VIDEO_EFFECT, "effect:builtin:NodeFilter" );


	nle::MainFilterFactory fFactory;
	nle::Frei0rFactory effectFactory( nui.m_effectMenu );
	
	nle::ColorCurveFactory colorCurveFactory;
	nui.m_effectMenu->addEffect( &colorCurveFactory );
	fFactory.add( "effect:builtin:ColorCurves", &colorCurveFactory );

	nle::NodeFilterFactory nodeFilterFactory;
	nui.m_effectMenu->addEffect( &nodeFilterFactory );
	fFactory.add( "effect:builtin:NodeFilter", &nodeFilterFactory );

	NodeFilterFrei0rFactory nodeFilterFrei0rFactory;
/*
	nui.m_effectMenu->addEffect( &SCTFilterFactory );
	fFactory.add( SCTFilterFactory.identifier(), &SCTFilterFactory );
*/
	nle::AudioVolumeFilterFactory audioVolumeFilterFactory;
	nui.m_effectMenu->addEffect( &audioVolumeFilterFactory );
	fFactory.add( "filter:builtin:VolumeAutomations", &audioVolumeFilterFactory );
	
	nle::IPlaybackCore* playbackCore = new nle::JackPlaybackCore( nle::g_timeline, nle::g_timeline, nle::g_videoView );
	if ( !playbackCore->ok() ) {
		delete playbackCore;
		playbackCore = new nle::PortAudioPlaybackCore( nle::g_timeline, nle::g_timeline, nle::g_videoView );
		nui.portaudio();
	} else {
		nui.jack();
	}
	nle::LoadSaveManager lsm( nui.projectChoice, nui.projectNameInput );
	nle::FilmStripFactory filmStripFactory;
	nle::DocManager docManager;

	Fl::visual(FL_DOUBLE|FL_RGB);
	Fl::set_fonts(0);

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
	//lsm.shutdown();
	delete playbackCore;
	return r;
}
