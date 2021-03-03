INCLUDES = $(all_includes)

lib_LTLIBRARIES = libdockbarextension.la

libdockbarextension_la_SOURCES = dockcontainer.cpp dockbarextension.cpp
libdockbarextension_la_METASOURCES = AUTO
libdockbarextension_la_LDFLAGS = $(all_libraries) -version-info 1:0:0 -module -no-undefined
libdockbarextension_la_LIBADD = $(LIB_KDEUI)

noinst_HEADERS = dockcontainer.h dockbarextension.h

lnkdir = $(kde_datadir)/kicker/extensions
lnk_DATA = dockbarextension.desktop

EXTRA_DIST = $(lnk_DATA)

messages:
	$(XGETTEXT) *.cpp *.h -o $(podir)/dockbarextension.pot
