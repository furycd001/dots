pic_DATA = lcd.png
picdir = $(kde_datadir)/clockapplet/pics

INCLUDES = $(all_includes)

lib_LTLIBRARIES = libclockapplet.la

libclockapplet_la_SOURCES = clock.cpp conf.ui

METASOURCES = AUTO
noinst_HEADERS = clock.h conf.h

lnkdir = $(kde_datadir)/kicker/applets
lnk_DATA = clockapplet.desktop

EXTRA_DIST = $(lnk_DATA) $(pic_DATA)

libclockapplet_la_LDFLAGS = $(all_libraries) -version-info 1:0:0 -module -no-undefined
libclockapplet_la_LIBADD = $(LIB_KDECORE) $(LIB_KDEUI)

srcdoc:
	kdoc -a -p -H -d $(HOME)/web/src/clockapplet clockapplet *.h -lqt -lkdecore -lkdeui -lkfile

messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -o $(podir)/clockapplet.pot

