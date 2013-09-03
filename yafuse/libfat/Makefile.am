MAINTAINERCLEANFILES = $(srcdir)/Makefile.in

if INSTALL_LIBRARY
lib_LTLIBRARIES    = libfat.la
pkgconfig_DATA     = libfat.pc
else
noinst_LTLIBRARIES = libfat.la
endif

libfat_la_LIBADD = $(top_builddir)/libio/libio.la

libfat_la_CFLAGS  = $(AM_CFLAGS) \
                    -I$(top_srcdir)/include/base \
                    -I$(top_srcdir)/include/libio \
                    -I$(top_srcdir)/include/libfat

libfat_la_LDFLAGS = -version-info \
                    $(LIBFAT_VERSION) \
                    -no-undefined

libfat_la_SOURCES = super.c \
                    dir.c \
                    file.c \
                    stat.c \
                    fs_fat.c

if WINDOWS
libfat_la_SOURCES +=
else
libfat_la_SOURCES +=
endif
