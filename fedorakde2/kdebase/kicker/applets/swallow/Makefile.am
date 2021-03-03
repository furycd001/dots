INCLUDES = $(all_includes)

lib_LTLIBRARIES = libswallowapplet.la

libswallowapplet_la_LDFLAGS	= $(all_libraries) -version-info 1:0:0 -module -no-undefined
libswallowapplet_la_LIBADD = $(LIB_KDEUI)

libswallowapplet_la_SOURCES = swallow.cpp

noinst_HEADERS = swallow.h

libswallowapplet_la_METASOURCES = AUTO

EXTRA_DIST = 

applnk_DATA = swallowapplet.desktop
applnkdir = $(kde_datadir)/kicker/applets

messages:
#	$(XGETTEXT) $(libswallowapplet_la_SOURCES) -o $(podir)/swallowapplet.pot
