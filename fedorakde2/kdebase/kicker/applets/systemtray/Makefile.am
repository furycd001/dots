
INCLUDES = $(all_includes)

lib_LTLIBRARIES = libksystemtrayapplet.la

libksystemtrayapplet_la_SOURCES = systemtrayapplet.cpp

libksystemtrayapplet_la_METASOURCES = USE_AUTOMOC
noinst_HEADERS = systemtrayapplet.h

lnkdir = $(kde_datadir)/kicker/applets
lnk_DATA = ksystemtrayapplet.desktop

EXTRA_DIST = $(lnk_DATA)

libksystemtrayapplet_la_LDFLAGS = $(all_libraries) -version-info 1:0:0 -module -no-undefined
libksystemtrayapplet_la_LIBADD = $(LIB_KDEUI) $(LIB_KSYCOCA)


messages:
	$(XGETTEXT)  *.cpp *.h -o $(podir)/ksystemtrayapplet.pot

srcdoc:
	kdoc -a -p -H -d $(HOME)/web/src/ksystemtrayapplet ksystemtrayapplet *.h -lqt -lkdecore -lkdeui -lkfile
