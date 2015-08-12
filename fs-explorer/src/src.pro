#
# Project Sub File
#

include(../configure.pri)

TEMPLATE = app

CONFIG += qt

QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

RESOURCES += $$SRC_PREFIX/src.qrc
RC_FILE   += $$SRC_PREFIX/src.rc
FORMS     += $$SRC_PREFIX/aboutdialog.ui

INCLUDEPATH += $$LIB_PREFIX/qconsole
INCLUDEPATH += $$LIB_PREFIX/libyafuse2/include

HEADERS += $$SRC_PREFIX/*.h
SOURCES += $$SRC_PREFIX/*.cpp

DEPENDPATH += $$LIB_PREFIX/qconsole

unix {
  LIBS += -L$$LIB_PREFIX/qconsole -lqconsole
}

win32 {
  LIBS += -L$$LIB_PREFIX/qconsole/release -lqconsole
}

unix {
  inst_ico.files +=
}

win32 {
  inst_ico.files += $$SRC_PREFIX/images/icon.ico
}

inst_releasenote.files = $$PREFIX/ReleaseNote.txt
inst_releasenote.path  = $$INSTALL_PREFIX
INSTALLS               += inst_releasenote

inst_ico.path = $$INSTALL_PREFIX
INSTALLS      += inst_ico

release {
  TARGET = fs-explorer
  DESTDIR = $$INSTALL_PREFIX/bin
}
