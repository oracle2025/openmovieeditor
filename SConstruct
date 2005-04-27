env = Environment( CC = 'g++', CXXFLAGS='-ggdb', LINKFLAGS='-lquicktime -lportaudio' )
env.ParseConfig( 'fltk-config --cxxflags --ldflags' )

src_list = Split("""Clip.cxx
	nle.cxx
	Timeline.cxx
	VideoTrack.cxx
	nle_main.cxx
	SwitchBoard.cxx
	TimelineView.cxx
	VideoClip.cxx
	VideoView.cxx
	Draw.cxx
	Track.cxx
	Rect.cxx
	MoveDragHandler.cxx
	TrimDragHandler.cxx
	VideoFileQT.cxx
	AudioTrack.cxx
	AudioClip.cxx
	AudioFileQT.cxx
	Sound.cxx""")

env.Program('nle', src_list )
