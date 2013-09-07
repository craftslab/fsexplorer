CONFIG += qt warn_on release
TEMPLATE = app

RESOURCES = src.qrc

#
# Import header and lib of fs
#
INCLUDEPATH += ../install/include
unix:LIBS += "-L../install/lib/unix -lext4 -lfat -lfs -lio"
win32:LIBS += "-L../install/lib/win32"

HEADERS += platform.h
HEADERS += explorer.h
HEADERS += mainwindow.h

unix:SOURCES += platform_unix.cpp
win32:SOURCES += platform_win32.cpp
SOURCES += explorer.cpp
SOURCES += mainwindow.cpp
SOURCES += main.cpp

release {
TARGET = fs-explorer
DESTDIR = ../install/bin
}
