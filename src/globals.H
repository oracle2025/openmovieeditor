/*  globals.H
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

#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#define STR_LEN 512
#define OME_VERSION "0.0.0"

#include <lqt.h>

class NleUI;

namespace nle
{

class TimelineView;
class Timeline;
class Flmm_Scalebar;
class Ruler;
class VideoViewGL;
class Sound;
class Prefs;
class WavArtist;
class LoadSaveManager;
class Project;

extern TimelineView* g_timelineView;
extern Timeline* g_timeline;
//extern Flmm_Scalebar* g_scrollBar;
extern Ruler* g_ruler;
extern VideoViewGL* g_videoView;
extern Sound* g_sound;
extern Prefs* g_preferences;
extern WavArtist* g_wavArtist;
extern LoadSaveManager* g_loadSaveManager;
extern Project* g_project;

extern lqt_codec_info_t** g_audio_codec_info;
extern lqt_codec_info_t** g_video_codec_info;
extern NleUI* g_ui;
extern bool g_SEEKING;

} /* namespace nle */

extern float g_fps;

#endif /* _GLOBALS_H_ */

