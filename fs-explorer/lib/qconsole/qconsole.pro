#
# Project Sub File
#

include(../../configure.pri)

TEMPLATE = lib

CONFIG += staticlib
CONFIG += qt

QT += core
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

INCLUDEPATH += $$LIB_PREFIX/qconsole

HEADERS += $$LIB_PREFIX/qconsole/*.h
SOURCES += $$LIB_PREFIX/qconsole/*.cpp

release {
  TARGET = qconsole
}
