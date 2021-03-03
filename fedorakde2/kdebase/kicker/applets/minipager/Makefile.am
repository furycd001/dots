SUBDIRS = 

INCLUDES = $(all_includes)

lib_LTLIBRARIES = libkminipagerapplet.la

libkminipagerapplet_la_SOURCES = pagerapplet.cpp

libkminipagerapplet_la_METASOURCES = USE_AUTOMOC
noinst_HEADERS = pagerapplet.h

lnkdir = $(kde_datadir)/kicker/applets
lnk_DATA = kminipagerapplet.desktop

EXTRA_DIST = $(lnk_DATA)

libkminipagerapplet_la_LDFLAGS = $(all_libraries) -version-info 1:0:0 -module -no-undefined
libkminipagerapplet_la_LIBADD = $(LIB_KDEUI)


messages:
	$(XGETTEXT) *.cpp *.h -o $(podir)/kminipagerapplet.pot

srcdoc:
	kdoc -a -p -H -d $(HOME)/web/src/kminipagerapplet kminipagerapplet *.h -lqt -lkdecore -lkdeui -lkfile
