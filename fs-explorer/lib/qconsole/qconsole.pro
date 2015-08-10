#
# Sub project file
#
TEMPLATE = lib

CONFIG += staticlib
CONFIG += qt

QT += core
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

INCLUDEPATH += $$PWD

HEADERS += $$PWD/*.h
SOURCES += $$PWD/*.cpp

release {
  TARGET = qconsole
}
