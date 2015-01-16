#
# Sub project file
#

TEMPLATE = lib

CONFIG += staticlib

INCLUDEPATH += $$PWD
INCLUDEPATH += $$PWD/include

HEADERS += $$PWD/*.h
HEADERS += $$PWD/include/sparse/*.h

SOURCES += $$PWD/*.c

release {
  TARGET = sparse
}
