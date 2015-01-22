#
# Sub project file
#

#
# Customized configuration
#
MACH = 64
INSTALL_PREFIX = $$PWD/../../install

TEMPLATE = lib

CONFIG += dll

unix {
  inst_library.files += $$PWD/lib$$MACH/unix/*.so
}

win32 {
  inst_library.files += $$PWD/lib$$MACH/win/*.dll
}

inst_library.path = $$INSTALL_PREFIX/lib
INSTALLS += inst_library
