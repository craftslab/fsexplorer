CONFIG += qt warn_on release
TEMPLATE = app

QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

RESOURCES += src.qrc
RC_FILE += src.rc

#FORMS += statswindow.ui

HEADERS += fsengine.h
HEADERS += fstreeitem.h
HEADERS += fstreemodel.h
HEADERS += statswindow.h
HEADERS += consolethread.h
HEADERS += consolewindow.h
HEADERS += mainwindow.h

SOURCES += fsengine.cpp
SOURCES += fstreeitem.cpp
SOURCES += fstreemodel.cpp
SOURCES += statswindow.cpp
SOURCES += consolethread.cpp
SOURCES += consolewindow.cpp
SOURCES += mainwindow.cpp
SOURCES += main.cpp

INCLUDEPATH += ../include
#unix:LIBS += "-L../install/lib/ -lext4 -lfat"
#win32:LIBS += "-L../install/lib/"

MACH = 64
message(building $$MACH bit)

contains(MACH, 32) {
  QMAKE_CFLAGS += -m32
  QMAKE_CXXFLAGS += -m32
  QMAKE_LFLAGS += -m32
}

contains(MACH, 64) {
  QMAKE_CFLAGS += -m64
  QMAKE_CXXFLAGS += -m64
  QMAKE_LFLAGS += -m64
}

unix {
  inst.files += ../lib$$MACH/unix/libfs.so
  inst.files += ../lib$$MACH/unix/libext4.so
  inst.files += ../lib$$MACH/unix/libfat.so
  inst.files += ../lib$$MACH/unix/libQt5Core.so.5.2.0
  inst.files += ../lib$$MACH/unix/libQt5Gui.so.5.2.0
}

win32 {
  inst.files += ../lib$$MACH/win/libfs.dll
  inst.files += ../lib$$MACH/win/libext4.dll
  inst.files += ../lib$$MACH/win/libfat.dll
  inst.files += ../lib$$MACH/win/Qt5Core.dll
  inst.files += ../lib$$MACH/win/Qt5Gui.dll
}

inst.path = ../install/bin
INSTALLS += inst

release {
  TARGET = fs-explorer
  DESTDIR = ../install/bin
}
