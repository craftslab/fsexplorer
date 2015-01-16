#
# Main project file
#

TEMPLATE = subdirs

message(built for $$MACH-bit arch)

CONFIG += warn_on release
CONFIG += ordered

SUBDIRS += lib
SUBDIRS += src
