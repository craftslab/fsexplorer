#
# Sub project file
#

#
# Customized configuration
#
INSTALL_PREFIX = $$PWD/../install

TEMPLATE = app

CONFIG += qt

QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

RESOURCES += src.qrc
RC_FILE += src.rc

FORMS += aboutdialog.ui

INCLUDEPATH += $$PWD/../lib/qconsole
INCLUDEPATH += $$PWD/../lib/libyafuse2/include
INCLUDEPATH += $$PWD/../lib/zlib
INCLUDEPATH += $$PWD/../lib/libsparse
INCLUDEPATH += $$PWD/../lib/libsparse/include

HEADERS += $$PWD/*.h
SOURCES += $$PWD/*.cpp

DEPENDPATH += $$PWD/../lib/qconsole
DEPENDPATH += $$PWD/../lib/zlib
DEPENDPATH += $$PWD/../lib/libsparse

unix {
  LIBS += -L$$PWD/../lib/qconsole -lqconsole
  LIBS += -L$$PWD/../lib/libsparse -lsparse
  LIBS += -L$$PWD/../lib/zlib -lz
}

win32 {
  LIBS += -L$$PWD/../lib/qconsole
  LIBS += -L$$PWD/../lib/libsparse
  LIBS += -L$$PWD/../lib/zlib
}

unix {
  inst_ico.files +=
}

win32 {
  inst_ico.files += $$PWD/images/icon.ico
}

inst_releasenote.files = $$PWD/../ReleaseNote.txt
inst_releasenote.path = $$INSTALL_PREFIX
INSTALLS += inst_releasenote

inst_ico.path = $$INSTALL_PREFIX
INSTALLS += inst_ico

release {
  TARGET = fs-explorer
  DESTDIR = $$INSTALL_PREFIX/bin
}
