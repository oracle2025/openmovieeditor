# Copyright (C) 2009 Richard Spindler <richard.spindler AT gmail.com>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

noinst_LIBRARIES = libYoutubeRenderer.a
EXTRA_DIST = YoutubeDialog.fl
AM_CPPFLAGS = -ggdb -I$(top_srcdir)/src -I$(top_srcdir)/src/sl -I$(top_srcdir)/src/tinyxml -I$(top_srcdir)/src/ProgressDialog @CPPFLAGS@
libYoutubeRenderer_a_SOURCES = \
	VideoWriterMP4.cxx  VideoWriterMP4.H  YoutubeDialog.cxx  YoutubeDialog.H  \
	YoutubeUploader.cxx  YoutubeUploader.H

