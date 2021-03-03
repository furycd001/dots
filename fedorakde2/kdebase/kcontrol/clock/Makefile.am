kde_module_LTLIBRARIES = libkcm_clock.la

libkcm_clock_la_SOURCES = dtime.cpp tzone.cpp main.cpp 

libkcm_clock_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined
libkcm_clock_la_LIBADD = -lkdeui

INCLUDES= $(all_includes)
 
libkcm_clock_la_METASOURCES = AUTO

messages:
	$(XGETTEXT) $(libkcm_clock_la_SOURCES) -o $(podir)/kcmkclock.pot

data_DATA = clock.desktop
datadir = $(kde_appsdir)/Settings/System
