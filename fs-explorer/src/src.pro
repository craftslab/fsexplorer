CONFIG += qt warn_on release
TEMPLATE = app

QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

RESOURCES += src.qrc
RC_FILE += src.rc

FORMS += aboutdialog.ui

HEADERS += fsengine.h
HEADERS += fstreeitem.h
HEADERS += fstreemodel.h
HEADERS += fslistitem.h
HEADERS += fslistmodel.h
HEADERS += searchengine.h
HEADERS += searchwindow.h
HEADERS += consoleengine.h
HEADERS += consolewindow.h
HEADERS += statswindow.h
HEADERS += aboutdialog.h
HEADERS += mainwindow.h

SOURCES += fsengine.cpp
SOURCES += fstreeitem.cpp
SOURCES += fstreemodel.cpp
SOURCES += fslistitem.cpp
SOURCES += fslistmodel.cpp
SOURCES += searchengine.cpp
SOURCES += searchwindow.cpp
SOURCES += consoleengine.cpp
SOURCES += consolewindow.cpp
SOURCES += statswindow.cpp
SOURCES += aboutdialog.cpp
SOURCES += mainwindow.cpp
SOURCES += main.cpp

INCLUDEPATH += ../include
#unix:LIBS += "-L../release/lib/ -lyafuse2"
#win32:LIBS += "-L../release/lib/"

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
  inst_library.files += ../lib$$MACH/unix/libyafuse2.so
#  inst_library.files += ../lib$$MACH/unix/libQt5Core.so.5.2.0
#  inst_library.files += ../lib$$MACH/unix/libQt5Gui.so.5.2.0
#  inst_library.files += ../lib$$MACH/unix/libQt5Widgets.so.5.2.0
#  inst_library.files += ../lib$$MACH/unix/libicudata.so.51.1
#  inst_library.files += ../lib$$MACH/unix/libicui18n.so.51.1
#  inst_library.files += ../lib$$MACH/unix/libicuuc.so.51.1
  inst_ico.files +=
}

win32 {
  inst_library.files += ../lib$$MACH/win/libyafuse2.dll
#  inst_library.files += ../lib$$MACH/win/Qt5Core.dll
#  inst_library.files += ../lib$$MACH/win/Qt5Gui.dll
#  inst_library.files += ../lib$$MACH/win/Qt5Widgets.dll
#  inst_library.files += ../lib$$MACH/win/icudt51.dll
#  inst_library.files += ../lib$$MACH/win/icuin51.dll
#  inst_library.files += ../lib$$MACH/win/icuuc51.dll
#  inst_library.files += ../lib$$MACH/win/libGLESv2.dll
  inst_ico.files += ../src/images/icon.ico
}

inst_library.path = ../release/lib
INSTALLS += inst_library

inst_releasenote.files = ../ReleaseNote.txt
inst_releasenote.path = ../release
INSTALLS += inst_releasenote

inst_ico.path = ../release
INSTALLS += inst_ico

release {
  TARGET = fs-explorer
  DESTDIR = ../release/bin
}
