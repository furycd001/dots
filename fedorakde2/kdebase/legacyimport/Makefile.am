INCLUDES = $(all_includes)

bin_PROGRAMS = klegacyimport
lib_LTLIBRARIES = klegacyimport.la

klegacyimport_la_SOURCES = legacyimport.cpp
klegacyimport_la_LIBADD = $(LIB_KDEUI) $(LIB_KFILE)
klegacyimport_la_LDFLAGS = $(all_libraries) -module -avoid-version

klegacyimport_SOURCES = dummy.cpp
klegacyimport_LDADD   = klegacyimport.la
klegacyimport_LDFLAGS = $(all_libraries) $(KDE_RPATH)

noinst_HEADERS = legacyimport.h

METASOURCES = AUTO
apps_DATA = klegacyimport.desktop
appsdir = $(kde_appsdir)/System

EXTRA_DIST = $(apps_DATA)

messages:
	$(XGETTEXT) *.cpp -o $(podir)/klegacyimport.pot
