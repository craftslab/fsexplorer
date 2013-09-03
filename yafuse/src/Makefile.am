MAINTAINERCLEANFILES = $(srcdir)/Makefile.in

bin_PROGRAMS = yafuse

yafuse_LDADD = $(top_builddir)/libfs/libfs.la \
               $(top_builddir)/libext4/libext4.la \
               $(top_builddir)/libfat/libfat.la

if REALLYSTATIC
yafuse_LDFLAGS = $(AM_LDFLAGS) -all-static
endif

yafuse_CFLAGS = $(AM_CFLAGS) \
                -I$(top_srcdir)/include/base \
                -I$(top_srcdir)/include/libfs \
                -I$(top_srcdir)/include/libext4 \
                -I$(top_srcdir)/include/libfat \
                -I$(top_srcdir)/src

yafuse_SOURCES  = main.c \
                  subsystem.c

if RUN_LDCONFIG
install-exec-hook:
	$(LDCONFIG)
endif
