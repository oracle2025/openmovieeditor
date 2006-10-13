# Copyright (C) 2005 Richard Spindler <richard.spindler AT gmail.com>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

AC_PREREQ(2.5)
AC_INIT(openmovieeditor, [0.0.20061013], [richard.spindler@gmail.com])
AC_CONFIG_HEADER([config.h])

AM_INIT_AUTOMAKE(AC_PACKAGE_NAME, AC_PACKAGE_VERSION)

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_INSTALL

AC_PROG_RANLIB

# Checks for libraries.

# Checks for header files.
AC_HEADER_STDC
AC_LANG(C++)
AC_CHECK_HEADERS([fcntl.h memory.h stddef.h stdint.h stdlib.h string.h sys/ioctl.h sys/time.h unistd.h GL/gl.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_HEADER_TIME
AC_C_VOLATILE
AC_CHECK_TYPES([ptrdiff_t])

# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_PROG_GCC_TRADITIONAL
AC_FUNC_MALLOC
AC_FUNC_SELECT_ARGTYPES
AC_CHECK_FUNCS([gettimeofday memset select strchr strdup strstr])

AC_SYS_LARGEFILE
LARGEFILE_FLAGS=`getconf LFS_CFLAGS`
CPPFLAGS="$CPPFLAGS $LARGEFILE_FLAGS"
CFLAGS="$CFLAGS $LARGEFILE_FLAGS"

 AC_CHECK_HEADER(sys/soundcard.h,,
  AC_MSG_ERROR([*** sys/soundcard.h not found - this program requires the Open Sound System (http://4front-tech.com) or compatible (http://www.alsa-project.com) ***]))

PKG_CHECK_MODULES(SAMPLERATE, samplerate >= 0.1.1,,
  AC_MSG_ERROR([*** libsamplerate not found - this program requires libsamplerate >= 0.1.1 http://www.mega-nerd.com/SRC/ ***]))
CPPFLAGS="$CPPFLAGS $SAMPLERATE_CFLAGS"
LDFLAGS="$LDFLAGS $SAMPLERATE_LIBS"

PKG_CHECK_MODULES(SNDFILE, sndfile >= 1.0.0,,
  AC_MSG_ERROR([*** libsndfile 1.0.0 not found - this program requires libsndfile >= 1.0.0 http://www.mega-nerd.com/libsndfile/ ***]))
CPPFLAGS="$CPPFLAGS $SNDFILE_CFLAGS"
LDFLAGS="$LDFLAGS $SNDFILE_LIBS"

PKG_CHECK_MODULES(GAVL, gavl >= 0.2.3,,
  AC_MSG_ERROR([*** gavl 0.2.3 not found - this program requires the gmerlin audio
  video library >= 0.2.3 http://gmerlin.sourceforge.net/ ***]))
CPPFLAGS="$CPPFLAGS $GAVL_CFLAGS"
LDFLAGS="$LDFLAGS $GAVL_LIBS"

PKG_CHECK_MODULES(LIBQUICKTIME, libquicktime >= 0.9.7,,
  AC_MSG_ERROR([*** libquicktime 0.9.7 not found - this program requires libquicktime >= 0.9.7 http://libquicktime.sourceforge.net/ ***]))
CPPFLAGS="$CPPFLAGS $LIBQUICKTIME_CFLAGS"
LDFLAGS="$LDFLAGS $LIBQUICKTIME_LIBS"

HAVE_AVCODEC=false
PKG_CHECK_MODULES(AVCODEC, libavcodec, [HAVE_AVCODEC=true], [true])
if test x$HAVE_AVCODEC = xfalse; then
  AC_MSG_WARN([*** libavcodec not found])
else
  CPPFLAGS="$CPPFLAGS $AVCODEC_CFLAGS -DAVCODEC"
  LDFLAGS="$LDFLAGS $AVCODEC_LIBS"
fi

if test x$HAVE_AVCODEC = xfalse; then
  HAVE_AVCODEC_ACL=false
  PKG_CHECK_MODULES(AVCODEC_ACL, avcodec_acl, [HAVE_AVCODEC_ACL=true], [true])
  if test x$HAVE_AVCODEC_ACL = xfalse; then
    AC_MSG_WARN([*** libavcodec_acl not found])
  else
    CPPFLAGS="$CPPFLAGS $AVCODEC_ACL_CFLAGS -DAVCODEC"
    LDFLAGS="$LDFLAGS $AVCODEC_ACL_LIBS"
  fi
fi

HAVE_AVFORMAT=false
PKG_CHECK_MODULES(AVFORMAT, libavformat, [HAVE_AVFORMAT=true], [true])
if test x$HAVE_AVFORMAT = xfalse; then
  AC_MSG_WARN([*** libavformat not found])
else
  CPPFLAGS="$CPPFLAGS $AVFORMAT_CFLAGS"
  LDFLAGS="$LDFLAGS $AVFORMAT_LIBS"
fi

if test x$HAVE_AVFORMAT = xfalse; then
  HAVE_AVFORMAT_ACL=false
  PKG_CHECK_MODULES(AVFORMAT_ACL, avformat_acl, [HAVE_AVFORMAT_ACL=true], [true])
  if test x$HAVE_AVFORMAT_ACL = xfalse; then
    AC_MSG_WARN([*** libavformat_acl not found])
  else
    CPPFLAGS="$CPPFLAGS $AVFORMAT_ACL_CFLAGS"
    LDFLAGS="$LDFLAGS $AVFORMAT_ACL_LIBS"
  fi
fi


AC_CHECK_LIB(jack, main, [ac_jack_lib="yes"], [ac_jack_lib="no"])
AC_CHECK_HEADER(jack/jack.h, [ac_jack_h="yes"], [ac_jack_h="no"])
if test "x$ac_jack_lib$ac_jack_h" = "xyesyes"; then
   LDFLAGS="$LDFLAGS -ljack"
else 
   AC_MSG_ERROR([*** You need JACK audio library (libjack-dev) installed.  http://jackit.sf.net/])
fi


dnl FLTK-Check Shamelessly stolen from SpiralSynthModular
AC_PATH_PROG(FLTK_CONFIG, fltk-config, no)
if test "$FLTK_CONFIG" = "no"; then
  AC_PATH_XTRA
  LIBGL=""
  CPPFLAGS="$CPPFLAGS $X_CFLAGS"
  AC_CHECK_HEADERS(GL/gl.h)
  if test $ac_cv_header_GL_gl_h = "yes" ; then
    AC_CHECK_LIB(GL, glBegin, LIBGL="-lGL")
  fi
  AC_CHECK_HEADERS(FL/Fl.h)
  if test $ac_cv_header_FL_Fl_h = "no" ; then
    echo "*** FLTK headers missing."
    exit 1
  fi
  AC_CHECK_LIB(fltk, main)
  FLTK_CXXFLAGS="$X_CFLAGS"
  FLTK_CFLAGS="$FLTK_CXXFLAGS"
  FLTK_LIBS="$X_LIBS -lfltk $LIBGL"
else
  FLTK_CXXFLAGS=`$FLTK_CONFIG $fltk_config_args --cxxflags --use-gl --use-images`
  FLTK_CFLAGS=`$FLTK_CONFIG $fltk_config_args --cxxflags --use-gl --use-images`
  FLTK_LIBS=`$FLTK_CONFIG $fltk_config_args --ldflags --use-gl --use-images`
dnl This caused problems for debian users,
dnl It would be nice to find a more robust way of doing it.
dnl  if test "${FLTK_LIBS/*-rpath*/yes}" != "yes"; then
dnl    echo "*** FLTK needs to be configured with --enable-shared"
dnl   exit 1
dnl  fi
fi
CPPFLAGS="$CPPFLAGS $FLTK_CXXFLAGS"
LDFLAGS="$LDFLAGS $FLTK_LIBS"
AC_SUBST(FLTK_CXXFLAGS)
AC_SUBST(FLTK_CFLAGS)
AC_SUBST(FLTK_LIBS)


AC_CONFIG_FILES([
   Makefile
   ebuilds/Makefile
   doc/Makefile
   doc/images/Makefile
   icons/Makefile
   src/Makefile
   src/portaudio/Makefile
   src/sl/Makefile
   src/tinyxml/Makefile
   src/ProgressDialog/Makefile
   src/LoadSaveManager/Makefile
   src/ErrorDialog/Makefile
   src/timeline/Makefile
])

AC_OUTPUT

