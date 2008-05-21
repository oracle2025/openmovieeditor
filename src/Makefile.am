# Copyright (C) 2005 Richard Spindler <richard.spindler AT gmail.com>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

SUBDIRS = tinyxml timeline ProgressDialog sl LoadSaveManager ErrorDialog FloatNodes
EXTRA_DIST = nle.fl LiftGammaGainWidget.fl NodeFilterDialog.fl
#AM_CPPFLAGS = -ggdb -Wall -Werror @CPPFLAGS@ -I$(top_srcdir)/icons -I$(top_srcdir)/src -I$(top_srcdir)/src/tinyxml -I$(top_srcdir)/src/sl
AM_CPPFLAGS =  -D__STDC_CONSTANT_MACROS -ggdb @CPPFLAGS@ -DINSTALL_PREFIX="\"$(prefix)\"" -I$(top_srcdir)/icons -I$(top_srcdir)/src -I$(top_srcdir)/src/tinyxml -I$(top_srcdir)/src/sl

bin_PROGRAMS = openmovieeditor
openmovieeditor_LDADD = tinyxml/libtinyxml.a timeline/libtimeline.a\
	ProgressDialog/libProgressDialog.a\
	sl/libsl.a\
	LoadSaveManager/libLoadSaveManager.a\
	FloatNodes/libFloatNodes.a\
	ErrorDialog/libErrorDialog.a

openmovieeditor_SOURCES = nle.cxx\
	helper.cxx\
	Timeline.cxx\
	VideoTrack.cxx\
	nle_main.cxx\
	SwitchBoard.cxx\
	WavArtist.cxx\
	TimelineView.cxx\
	VideoClip.cxx\
	VideoViewGL.cxx\
	FilmStrip.cxx\
	Ruler.cxx\
	Rect.cxx\
	Prefs.cxx\
	MoveDragHandler.cxx\
	TrimDragHandler.cxx\
	VideoFileQT.cxx\
	TrackBase.cxx\
	AudioTrack.cxx\
	AudioClip.cxx\
	AudioFileSnd.cxx\
	AudioFileQT.cxx\
	PortAudio19PlaybackCore.cxx\
	PortAudio19PlaybackCore.H\
	JackPlaybackCore.cxx\
	JackPlaybackCore.H\
	Renderer.cxx\
	Flmm_Scalebar.cxx\
	SaveAsDialog.cxx\
	strlcpy.c\
	strlcpy.h\
	FilmStrip.H\
	IVideoFile.H\
	RenderDialog.H\
	VideoClip.H\
	AudioClip.H\
	Flmm_Scalebar.H\
	IVideoReader.H\
	Renderer.H\
	VideoFileQT.H\
	AudioFileQT.H\
	IVideoWriter.H\
	Ruler.H\
	VideoTrack.H\
	AudioFileSnd.H\
	global_includes.H\
	MoveDragHandler.H\
	SaveAsDialog.H\
	VideoViewGL.H\
	AudioTrack.H\
	globals.H\
	nle.h\
	SwitchBoard.H\
	WavArtist.H\
	helper.H\
	Timeline.H\
	DragHandler.H\
	IAudioFile.H\
	Prefs.H\
	TimelineView.H\
	IAudioReader.H\
	TrackBase.H\
	Rect.H\
	TrimDragHandler.H\
	IClipArtist.H\
	AudioClipArtist.H\
	AudioClipArtist.cxx\
	VideoClipArtist.H\
	VideoClipArtist.cxx\
	render_helper.cxx\
	render_helper.H\
	ImageClipArtist.H\
	ImageClipArtist.cxx\
	VideoFileFactory.cxx\
	VideoFileFactory.H\
	IdleHandler.cxx\
	IdleHandler.H\
	FilmStripFactory.H\
	FilmStripFactory.cxx\
	AudioFileFfmpeg.cxx\
	AudioFileFfmpeg.H\
	AudioFileFactory.cxx\
	AudioFileFactory.H\
	WaveForm.H\
	WaveForm.cxx\
	TimelineScroll.H\
	TimelineScroll.cxx\
	DocManager.cxx\
	DocManager.H\
	Command.H\
	Command.cxx\
	MoveCommand.cxx\
	MoveCommand.H\
	RemoveCommand.cxx\
	RemoveCommand.H\
	TrimCommand.cxx\
	TrimCommand.H\
	AddCommand.cxx\
	AddCommand.H\
	SplitCommand.cxx\
	SplitCommand.H\
	RemoveTrackCommand.H\
	RemoveTrackCommand.cxx\
	AddTrackCommand.H\
	AddTrackCommand.cxx\
	Commands.H\
	DiskCache.H\
	DiskCache.cxx\
	SelectDragHandler.H\
	SelectDragHandler.cxx\
	MoveSelectionCommand.H\
	MoveSelectionCommand.cxx\
	MediaPanel.cxx\
	MediaPanel.H\
	FolderBrowser.H\
	FolderBrowser.cxx\
	MediaBrowser.cxx\
	MediaBrowser.H\
	VideoThumbnails.H\
	VideoThumbnails.cxx\
	Resampler.cxx\
	Resampler.H\
	CompositeCommand.cxx\
	CompositeCommand.H\
	RemoveSelectionCommand.cxx\
	RemoveSelectionCommand.H\
	DummyClip.H\
	DummyClip.cxx\
	DummyClipArtist.H\
	DummyClipArtist.cxx\
	IEffectMenu.H\
	FltkEffectMenu.H\
	FltkEffectMenu.cxx\
	VideoEffectClip.cxx\
	VideoEffectClip.H\
	IVideoEffect.H\
	frei0r.h\
	IPlaybackCore.cxx\
	IPlaybackCore.H\
	PasteSelectionCommand.cxx\
	PasteSelectionCommand.H\
	PasteCommand.cxx\
	PasteCommand.H\
	SpecialClipsBrowser.H\
	SpecialClipsBrowser.cxx\
	TitleClip.cxx\
	TitleClip.H\
	Fl_Split.cpp\
	Fl_Split.h\
	fl_font_browser.cpp\
	fl_font_browser.h\
	IEffectDialog.H\
	ColorCurveFactory.cxx\
	ColorCurveFactory.H\
	ColorCurveFilter.cxx\
	ColorCurveFilter.H\
	CurveEditor2.cxx\
	CurveEditor2.H\
	ColorCurveDialog.cxx\
	ColorCurveDialog.H\
	FilterBase.H\
	FilterFactory.H\
	AudioFilter.H\
	AudioVolumeFilter.cxx\
	AudioVolumeFilter.H\
	AudioVolumeFilterFactory.cxx\
	AudioVolumeFilterFactory.H\
	AutomationDragHandler.H\
	AutomationDragHandler.cxx\
	FilterClip.cxx\
	FilterClip.H\
	MainFilterFactory.cxx\
	MainFilterFactory.H\
	FilterAddCommand.H\
	FilterAddCommand.cxx\
	FilterRemoveCommand.H\
	FilterRemoveCommand.cxx\
	timebase.H\
	ShiftAutomationDragHandler.H\
	int_float_scale.H\
	video_format.H\
	IVideoFileWriter.H\
	VideoWriterQT.H\
	VideoWriterQT.cxx\
	EncodingPreset.H\
	EncodingPreset.cxx\
	AutoTrack.H\
	AutoTrack.cxx\
	AutoDragHandler.H\
	AutoDragHandler.cxx\
	lqt_bitrate_params.h\
	XmlClipData.H\
	XmlClipData.cxx\
	color_schemes.cxx\
	color_schemes.H\
	AudioThreadedRingbuffer.H\
	AudioThreadedRingbuffer.cxx\
	EffectStackBrowser.H\
	EffectStackBrowser.cxx\
	lqtgavl.h\
	lqtgavl.c\
	ColorGrader2.h\
	ColorGrader2.cxx\
	LiftGammaGainFilter.H\
	LiftGammaGainFilter.cxx\
	LiftGammaGainFactory.H\
	LiftGammaGainFactory.cxx\
	LiftGammaGainDialog.H\
	LiftGammaGainDialog.cxx\
	LiftGammaGainWidget.H\
	LiftGammaGainWidget.cxx\
	Frei0rFactoryPlugin.cxx\
	Frei0rFactoryPlugin.H\
	Frei0rEffect.cxx\
	Frei0rEffect.H\
	Frei0rDialog.H\
	Frei0rDialog.cxx\
	Frei0rFactory.cxx\
	Frei0rFactory.H\
	LazyFrame.H\
	LazyFrame.cxx\
	PreviewNode.cxx\
	PreviewNode.H\
	NodeFilterPreviewFactoryPlugin.H\
	NodeFilterPreviewFactoryPlugin.cxx\
	NodeFilterFactory.H\
	NodeFilterFactory.cxx\
	NodeFilter.H\
	NodeFilter.cxx\
	NodeFilterDialog.H\
	NodeFilterDialog.cxx\
	Frei0rGraphEditor.cxx\
	Frei0rGraphEditor.H\
	NodeFilterFrei0rFactory.H\
	NodeFilterFrei0rFactory.cxx\
	NodeFilterFrei0rFactoryPlugin.H\
	NodeFilterFrei0rFactoryPlugin.cxx\
	SinkNode.H\
	SinkNode.cxx\
	SrcNode.H\
	SrcNode.cxx\
	Frei0rNode.H\
	Frei0rNode.cxx\
	INode.H\
	Frei0rDoubleSlider.H\
	Frei0rDoubleSlider.cxx\
	Frei0rColorButton.H\
	Frei0rColorButton.cxx\
	Frei0rBoolButton.H\
	Frei0rBoolButton.cxx\
	INodeFilterFactoryPlugin.H\
	NodeFilterBezierCurveFactoryPlugin.H\
	NodeFilterBezierCurveFactoryPlugin.cxx\
	BezierCurveNode.H\
	BezierCurveNode.cxx\
	CurveEditorBezier.H\
	CurveEditorBezier.cxx\
	ImageClip.cxx\
	ImageClip.H\
	InkscapeClip.cxx\
	InkscapeClip.H\
	ImageNode.H\
	ImageNode.cxx\
	VideoFileFfmpeg.cxx\
	VideoFileFfmpeg.H\
	NodeFilterImageFactoryPlugin.H\
	NodeFilterImageFactoryPlugin.cxx\
	frame_struct.h\
	Flmm_Cursor_Shape.cxx\
	Flmm_Cursor_Shape.H\
	Fl_Hsv_Color_Chooser.H\
	Fl_Hsv_Color_Chooser.cxx
#	VideoFileGmerlin.H\
#	VideoFileGmerlin.cxx
#	ColorWheel.H\
#	ColorWheel.cxx
#	FileBrowser.cxx\
#	FileBrowser.H


#	ShiftAutomationDragHandler.cxx\
#	AutomationMoveCommand.H\
#	AutomationMoveCommand.cxx\
#	AutomationDuoMoveCommand.H\
#	AutomationDuoMoveCommand.cxx\
#	AutomationAddCommand.H\
#	AutomationAddCommand.cxx\
#	AutomationRemoveCommand.H\
#	AutomationRemoveCommand.cxx\
#	EnvelopeClip.cxx\
#	EnvelopeClip.H
#	CurveViewRGB.cxx\
#	CurveViewRGB.H



