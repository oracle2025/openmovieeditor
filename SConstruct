# pkg-config gavl --cflags
# pkg-config gavl --libs

PROF_FLAGS = '-pg'
DEBUG_FLAGS = '-ggdb -I./sexpr_1.0.0/src'
LIBS = ['sexp']
LIBPATH = ['./sexpr_1.0.0/lib']

env = Environment( CC = 'g++',
	CXXFLAGS = "-Iicons/ " + DEBUG_FLAGS,
	LINKFLAGS = '-lquicktime -lportaudio -ggdb',
	LIBS = LIBS,
	LIBPATH = LIBPATH
	)
env.ParseConfig( 'fltk-config --cxxflags --ldflags --use-gl' )
#env.ParseConfig( 'flu-config --cxxflags --ldflags' )

src_list = Split("""Clip.cxx
	nle.cxx
	Timeline.cxx
	VideoTrack.cxx
	nle_main.cxx
	SwitchBoard.cxx
	TimelineView.cxx
	VideoClip.cxx
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
	Flmm_Scalebar.cxx""")

env.Program('nle', src_list )
