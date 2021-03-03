kde_module_LTLIBRARIES = libkcm_arts.la

libkcm_arts_la_SOURCES = arts.cpp artsconfig.ui

libkcm_arts_la_LDFLAGS  = $(all_libraries) -module -avoid-version -no-undefined
libkcm_arts_la_LIBADD = -lkdeui

INCLUDES = -I$(kde_includes)/arts  $(all_includes)

libkcm_arts_la_METASOURCES = AUTO

KDE_ICON = arts

DISTCLEANFILES = $(lib_arts_METASOURCES)

messages: rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/kcmarts.pot

data_DATA = arts.desktop
datadir = $(kde_appsdir)/Settings/Sound

