#
# Project Sub File
#

include(../../configure.pri)

TEMPLATE = lib

CONFIG += dll

unix {
  inst_library.files += $$LIB_PREFIX/libyafuse2/lib$$MACH/unix/*.so
}

win32 {
  inst_library.files += $$LIB_PREFIX/libyafuse2/lib$$MACH/win/*.dll
}

inst_library.path = $$INSTALL_PREFIX/lib
INSTALLS          += inst_library
