INCLUDES = $(all_includes) 

lib_LTLIBRARIES = libkrunapplet.la

libkrunapplet_la_SOURCES = runapplet.cpp

METASOURCES = runapplet.moc
noinst_HEADERS = runapplet.h

lnkdir = $(kde_datadir)/kicker/applets
lnk_DATA = krunapplet.desktop

EXTRA_DIST = $(lnk_DATA)

libkrunapplet_la_LDFLAGS = $(all_libraries) -version-info 1:0:0 -module -no-undefined
libkrunapplet_la_LIBADD = $(LIB_KSYCOCA) $(LIB_KDEUI)

messages:
	$(XGETTEXT) *.cpp *.h -o $(podir)/krunapplet.pot
