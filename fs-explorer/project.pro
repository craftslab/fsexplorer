#
# Project Main File
#

include(configure.pri)

TEMPLATE = subdirs

message(built for $$MACH-bit arch)

CONFIG += warn_on release
CONFIG += ordered

SUBDIRS += lib
SUBDIRS += src
