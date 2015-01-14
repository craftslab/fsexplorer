#
# Main project file
#

TEMPLATE = subdirs

message(built for $$MACH-bit arch)

CONFIG += warn_on release
CONFIG += ordered

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

SUBDIRS += lib
SUBDIRS += src
