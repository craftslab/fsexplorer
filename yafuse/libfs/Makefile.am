MAINTAINERCLEANFILES = $(srcdir)/Makefile.in

if INSTALL_LIBRARY
lib_LTLIBRARIES    = libfs.la
pkgconfig_DATA     = libfs.pc
else
noinst_LTLIBRARIES = libfs.la
endif

libfs_la_CFLAGS  = $(AM_CFLAGS) \
                    -I$(top_srcdir)/include/base \
                    -I$(top_srcdir)/include/libio

libfs_la_LDFLAGS = -version-info \
                    $(LIBFAT_VERSION) \
                    -no-undefined

libfs_la_SOURCES = fs.c

if WINDOWS
libfs_la_SOURCES +=
else
libfs_la_SOURCES +=
endif
