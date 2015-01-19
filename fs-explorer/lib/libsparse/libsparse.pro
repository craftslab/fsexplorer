#
# Sub project file
#

TEMPLATE = lib

CONFIG += staticlib

INCLUDEPATH += $$PWD
INCLUDEPATH += $$PWD/include
INCLUDEPATH += $$PWD/../zlib

HEADERS += $$PWD/*.h
HEADERS += $$PWD/include/sparse/*.h

SOURCES += $$PWD/backed_block.c
SOURCES += $$PWD/output_file.c
SOURCES += $$PWD/sparse.c
SOURCES += $$PWD/sparse_crc32.c
SOURCES += $$PWD/sparse_err.c
SOURCES += $$PWD/sparse_read.c

DEPENDPATH += $$PWD/../zlib

unix {
  LIBS += -L$$PWD/../zlib -lz
}

win32 {
  LIBS += -L$$PWD/../zlib
}

release {
  TARGET = sparse
}
