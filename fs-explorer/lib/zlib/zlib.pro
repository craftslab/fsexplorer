#
# Sub project file
#

TEMPLATE = lib

CONFIG += staticlib

INCLUDEPATH += $$PWD/src

HEADERS += $$PWD/src/*.h

SOURCES += $$PWD/src/adler32.c
SOURCES += $$PWD/src/compress.c
SOURCES += $$PWD/src/crc32.c
SOURCES += $$PWD/src/deflate.c
SOURCES += $$PWD/src/gzclose.c
SOURCES += $$PWD/src/gzlib.c
SOURCES += $$PWD/src/gzread.c
SOURCES += $$PWD/src/gzwrite.c
SOURCES += $$PWD/src/infback.c
SOURCES += $$PWD/src/inflate.c
SOURCES += $$PWD/src/inftrees.c
SOURCES += $$PWD/src/inffast.c
SOURCES += $$PWD/src/trees.c
SOURCES += $$PWD/src/uncompr.c
SOURCES += $$PWD/src/zutil.c

release {
  TARGET = z
}
