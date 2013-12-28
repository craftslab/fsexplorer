CONFIG += qt warn_on release
TEMPLATE = app

RESOURCES += src.qrc
RC_FILE += src.rc

#FORMS += statsframe.ui

HEADERS += fsengine.h
HEADERS += fstreeitem.h
HEADERS += fstreemodel.h
HEADERS += mainwindow.h

SOURCES += fsengine.cpp
SOURCES += fstreeitem.cpp
SOURCES += fstreemodel.cpp
SOURCES += mainwindow.cpp
SOURCES += main.cpp

INCLUDEPATH += ../include
#unix:LIBS += "-L../install/lib/ -lext4 -lfat"
#win32:LIBS += "-L../install/lib/"

unix{
  inst.files += ../lib/unix/libfs.so
  inst.files += ../lib/unix/libext4.so
  inst.files += ../lib/unix/libfat.so
  inst.files += ../lib/unix/libQtCore.so.4
  inst.files += ../lib/unix/libQtGui.so.4
}
win32{
  inst.files += ../lib/win32/libfs.dll
  inst.files += ../lib/win32/libext4.dll
  inst.files += ../lib/win32/libfat.dll
  inst.files += ../lib/win32/QtCore4.dll
  inst.files += ../lib/win32/QtGui4.dll
}
inst.path = ../install/bin
INSTALLS += inst

release {
TARGET = fs-explorer
DESTDIR = ../install/bin
}
