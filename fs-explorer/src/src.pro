CONFIG += qt warn_on release
TEMPLATE = app

RESOURCES = src.qrc

INCLUDEPATH += ../include

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
DESTDIR = ../bin
}
