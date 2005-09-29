# pkg-config gavl --cflags
# pkg-config gavl --libs


PROF_FLAGS = '-pg'
DEBUG_FLAGS = '-ggdb -I./sexpr'
LIBS = []
LIBPATH = []


env = Environment( CC = 'g++',
	CXXFLAGS = "-Wall -Werror -Iicons/ " + DEBUG_FLAGS,
	LINKFLAGS = '-ggdb',
	LIBS = LIBS,
	LIBPATH = LIBPATH
	)
#env.ParseConfig( 'pkg-config gavl --cflags --libs' )
env.ParseConfig( 'pkg-config libquicktime --cflags --libs' )
env.ParseConfig( 'fltk-config --cxxflags --ldflags --use-gl' )
env.ParseConfig( 'Magick++-config --cppflags --ldflags --libs' )
#env.ParseConfig( 'flu-config --cxxflags --ldflags' )

src_list = Split("""Clip.cxx
	nle.cxx
	helper.cxx
	Timeline.cxx
	VideoTrack.cxx
	nle_main.cxx
	SwitchBoard.cxx
	TimelineView.cxx
	VideoClip.cxx
	Codecs.cxx
	VideoViewGL.cxx
	FilmStrip.cxx
	Draw.cxx
	Project.cxx
	Ruler.cxx
	Track.cxx
	Rect.cxx
	MoveDragHandler.cxx
	TrimDragHandler.cxx
	FileBrowser.cxx
	VideoFileQT.cxx
	AudioTrack.cxx
	AudioClip.cxx
	AudioFileQT.cxx
	Sound.cxx
	Renderer.cxx
	Flmm_Scalebar.cxx
	Texter.cxx
	AutomationPoint.cxx
	AutomationDragHandler.cxx""")

src_list.append( SConscript(['portaudio/SConscript']) )
src_list.append( SConscript(['sexpr/SConscript']) )
src_list.append( SConscript(['sl/SConscript']) )

env.Program('nle', src_list )
