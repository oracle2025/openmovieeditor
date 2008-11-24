# Copyright (C) 2008 Richard Spindler <richard.spindler AT gmail.com>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

noinst_LIBRARIES = libFloatNodes.a
AM_CPPFLAGS = -ggdb -I$(top_srcdir)/src -I$(top_srcdir)/src/sl -I$(top_srcdir)/src/tinyxml @CPPFLAGS@
libFloatNodes_a_SOURCES = \
  FloatNodeFilter.H FloatNodeFilter.cxx\
  FloatNodeFilterDialog.H FloatNodeFilterDialog.cxx\
	SrcFloatNode.H SrcFloatNode.cxx\
	SinkFloatNode.H SinkFloatNode.cxx\
  FloatNodeFilterFactory.H FloatNodeFilterFactory.cxx\
  Float0rGraphEditor.H Float0rGraphEditor.cxx\
	IFloatNode.H IFloatNodeFilterFactoryPlugin.H\
	Float0rBoolButton.cxx    Float0rDoubleSlider.H   NodeFilterFloat0rFactory.cxx\
	Float0rBoolButton.H      Float0rGraphEditor.cxx  NodeFilterFloat0rFactory.H\
	Float0rColorButton.cxx   Float0rGraphEditor.H    NodeFilterFloat0rFactoryPlugin.cxx\
	Float0rColorButton.H     Float0rNode.cxx         NodeFilterFloat0rFactoryPlugin.H\
	Float0rDoubleSlider.cxx  Float0rNode.H
