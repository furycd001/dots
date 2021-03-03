INCLUDES = $(all_includes)

SUBDIRS = pics

bin_PROGRAMS = ktip
ktip_SOURCES = ktipwindow.cpp
ktip_LDADD = $(LIB_KDECORE) $(LIB_KDEUI) $(all_libraries) $(KDE_RPATH)

METASOURCES = AUTO
KDE_ICON = AUTO

desktop_DATA = ktip.desktop
desktopdir = $(kde_appsdir)/Toys

autostart_DATA = ktip.desktop
autostartdir = $(prefix)/share/autostart

messages: 
	perl ./preparetips > tips.cc
	$(XGETTEXT) *.cpp *.h tips.cc -o $(podir)/ktip.pot
	rm -f tips.cc

data_DATA = tips
datadir=$(kde_datadir)/kdewizard

