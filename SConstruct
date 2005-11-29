# pkg-config gavl --cflags
# pkg-config gavl --libs


PROF_FLAGS = '-pg'
DEBUG_FLAGS = '-ggdb '
LIBS = []
LIBPATH = []


env = Environment( CC = 'g++',
	CXXFLAGS = "-Wall -Werror -Iicons/ -Isl/ -Itinyxml/ -I./ " + DEBUG_FLAGS,
	LINKFLAGS = '-ggdb',
	LIBS = LIBS,
	LIBPATH = LIBPATH
	)
env.ParseConfig( 'pkg-config gavl --cflags --libs' )
env.ParseConfig( 'pkg-config sndfile --cflags --libs' )
env.ParseConfig( 'pkg-config libquicktime --cflags --libs' )
#env.ParseConfig( 'lqt-config --cflags --libs' )
env.ParseConfig( 'fltk-config --cxxflags --ldflags --use-gl' )
env.ParseConfig( 'Magick++-config --cppflags --ldflags --libs' )
env.ParseConfig( 'pkg-config jack --cflags --libs' )
#env.ParseConfig( 'flu-config --cxxflags --ldflags' )

src_list = Split("""nle.cxx
	helper.cxx
	Timeline.cxx
	VideoTrack.cxx
	nle_main.cxx
	SwitchBoard.cxx
	WavArtist.cxx
	TimelineView.cxx
	VideoClip.cxx
	Codecs.cxx
	VideoViewGL.cxx
	FilmStrip.cxx
	Ruler.cxx
	Rect.cxx
	Prefs.cxx
	Project.cxx
	MoveDragHandler.cxx
	TrimDragHandler.cxx
	FileBrowser.cxx
	VideoFileQT.cxx
	TrackBase.cxx
	AudioTrack.cxx
	AudioClip.cxx
	AudioClipBase.cxx
	AudioFileSnd.cxx
	AudioFileQT.cxx
	PlaybackCore.cxx
	Renderer.cxx
	Flmm_Scalebar.cxx
	Texter.cxx""")

src_list.append( SConscript(['portaudio/SConscript']) )
#src_list.append( SConscript(['sexpr/SConscript']) )

Export( 'env' )
src_list.append( SConscript(['timeline/SConscript']) )
src_list.append( SConscript(['ProgressDialog/SConscript']) )
src_list.append( SConscript(['sl/SConscript']) )
src_list.append( SConscript(['tinyxml/SConscript']) )

strlcpy_env = Environment( CFLAGS ="-Wall -Werror -ggdb" )
strlcpy = strlcpy_env.Library( 'strlcpy', "strlcpy.c" )
src_list.append( strlcpy )



env.Program('nle', src_list )
