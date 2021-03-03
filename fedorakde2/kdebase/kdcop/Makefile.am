#
#                           KDCOP Makefile.am
#
#  Copyright 2000 Matthias Kalle Dalheimer, kalle@dalheimer.de
#
#

INCLUDES = $(all_includes)

bin_PROGRAMS = kdcop

kdcop_SOURCES = kdcop.cpp kdcopwindow.cpp
kdcop_LDFLAGS = $(all_libraries) $(KDE_RPATH) $(LIB_KDEUI) $(LIB_KDECORE) -lDCOP $(LIB_QT)

noinst_HEADERS = kdcopwindow.h
METASOURCES =	AUTO

rcdir = $(kde_datadir)/kdcop
rc_DATA = kdcopui.rc

messages: rc.cpp
	$(XGETTEXT) rc.cpp *.cpp -o $(podir)/kdcop.pot

