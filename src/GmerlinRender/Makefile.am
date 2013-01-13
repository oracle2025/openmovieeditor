# Copyright (C) 2012 Richard Spindler <richard.spindler AT gmail.com>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

EXTRA_DIST = OggDialog.fl
noinst_LIBRARIES = libOggRender.a
AM_CPPFLAGS = -ggdb -I$(top_srcdir)/src -I$(top_srcdir)/src/sl -I$(top_srcdir)/src/tinyxml -I$(top_srcdir)/src/ProgressDialog @CPPFLAGS@
libOggRender_a_SOURCES = \
FileWriterOgg.cxx  FileWriterOgg.H OggRenderer.cxx  OggRenderer.H \
OggDialog.cxx OggDialog.H
