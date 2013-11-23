
TARGET   = waterspout
TEMPLATE = app
CONFIG  -= qt

ROOTDIR = $$PWD/..
INCLUDEDIR = $$ROOTDIR/include
SRCDIR = $$ROOTDIR/src
TESTDIR = $$ROOTDIR/tests
DESTDIR = $$ROOTDIR/bin
OBJECTS_DIR = $$DESTDIR/temp

HEADERS += \
  $$INCLUDEDIR/waterspout.h \
  $$SRCDIR/math_avx.h \
  $$SRCDIR/math_fpu.h \
  $$SRCDIR/math_mmx.h \
  $$SRCDIR/math_neon.h \
  $$SRCDIR/math_sse.h \
  $$SRCDIR/math_sse2.h \
  $$SRCDIR/math_sse3.h \
  $$SRCDIR/math_sse41.h \
  $$SRCDIR/math_sse42.h \
  $$SRCDIR/math_ssse3.h \
  $$TESTDIR/unittest.h \
  $$TESTDIR/common.h

SOURCES += \
  $$SRCDIR/waterspout.cpp \
  $$TESTDIR/main.cpp

OTHER_FILES += \
  $$PWD/premake4.lua \
  $$ROOTDIR/README.md \
  $$ROOTDIR/AUTHORS \
  $$ROOTDIR/LICENSE

DEFINES += \
  __MMX__=1 \
  __SSE__=1 \
  __SSE2__=1 \
  __SSE3__=1 \
  __SSSE3__=1 \
  __SSE4_1__=1 \
  __SSE4_2__=1 \
  __AVX__=1

unix{
  DEFINES += \
    LINUX=1

  INCLUDEPATH += \
    $$INCLUDEDIR \
      /usr/include
}
