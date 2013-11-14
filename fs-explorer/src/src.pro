CONFIG += qt warn_on release
TEMPLATE = app

RESOURCES += src.qrc
RC_FILE += src.rc

#
# Import header and lib of fs
#
INCLUDEPATH += ../include
unix:LIBS += "-L../install/lib/ -lext4"
win32:LIBS += "-L../install/lib/"

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
