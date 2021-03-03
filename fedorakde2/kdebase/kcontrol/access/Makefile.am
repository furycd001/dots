
INCLUDES= $(all_includes)
METASOURCES = AUTO

bin_PROGRAMS = kaccess

kaccess_SOURCES = dummy.cpp
kaccess_LDFLAGS = $(all_libraries) $(KDE_RPATH)
kaccess_LDADD = kaccess.la

kde_module_LTLIBRARIES = libkcm_access.la kaccess.la

kaccess_la_SOURCES = kaccess.cpp main.cpp
kaccess_la_LDFLAGS = $(all_libraries) -module -avoid-version
kaccess_la_LIBADD = $(LIB_KDEUI)

libkcm_access_la_SOURCES = kcmaccess.cpp 
libkcm_access_la_LDFLAGS  = $(all_libraries) -module -avoid-version -no-undefined
libkcm_access_la_LIBADD = $(LIB_KFILE)

noinst_HEADERS = kcmaccess.h

data_DATA = kcmaccess.desktop
datadir = $(kde_appsdir)/Settings/Personalization

services_DATA = kaccess.desktop
servicesdir = $(kde_servicesdir)

#KDE_ICON = sample

messages:
	$(XGETTEXT) $(kaccess_la_SOURCES) -o $(podir)/kaccess.pot
	$(XGETTEXT) $(libkcm_access_la_SOURCES) -o $(podir)/kcmaccess.pot

dummy.cpp:
	echo > dummy.cpp
