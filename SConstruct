PROF_FLAGS = '-pg'
DEBUG_FLAGS = '-ggdb'

env = Environment( CC = 'g++', CXXFLAGS=DEBUG_FLAGS, LINKFLAGS='-lquicktime -lportaudio -ggdb' )
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
	Flmm_Scalebar.cxx""")

env.Program('nle', src_list )
