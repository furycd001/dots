INCLUDES = -I$(srcdir)/../../taskbar -I$(srcdir)/../../taskmanager $(all_includes)

lib_LTLIBRARIES = libtaskbarapplet.la

libtaskbarapplet_la_SOURCES = taskbarapplet.cpp

libtaskbarapplet_la_METASOURCES = USE_AUTOMOC
libtaskbarapplet_la_LDFLAGS = $(all_libraries) -version-info 1:0:0 -module -no-undefined
libtaskbarapplet_la_LIBADD = $(LIB_KDEUI)  ../../taskbar/libtaskbar.la

noinst_HEADERS = taskbarapplet.h

lnkdir = $(kde_datadir)/kicker/applets
lnk_DATA = ktaskbarapplet.desktop

EXTRA_DIST = $(lnk_DATA)

messages:
	$(XGETTEXT) *.cpp *.h -o $(podir)/ktaskbarapplet.pot

srcdoc:
	kdoc -a -p -H -d $(HOME)/web/src/ktaskbarapplet ktaskbarapplet *.h -lqt -lkdecore -lkdeui -lkfile
