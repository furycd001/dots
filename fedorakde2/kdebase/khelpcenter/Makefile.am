SUBDIRS = plugins pics 

INCLUDES = $(all_includes)
METASOURCES = AUTO

# khelpcenter: binary for convenience
bin_PROGRAMS = 	khelpcenter

khelpcenter_LDFLAGS = $(all_libraries) $(KDE_RPATH)
khelpcenter_LDADD   = $(LIB_KHTML) # htmlsearch/libhtmlsearch.la 
khelpcenter_SOURCES = main.cpp khc_main.cc $(libkhelpcenterpart_la_SOURCES)

khelpcenter_la_LDFLAGS = $(all_libraries) $(KDE_RPATH) -module $(KDE_PLUGIN) -no-undefined
khelpcenter_la_LIBADD   = $(LIB_KHTML) # htmlsearch/libhtmlsearch.la 
khelpcenter_la_SOURCES = khc_main.cc $(libkhelpcenterpart_la_SOURCES)

kde_module_LTLIBRARIES = libkhelpcenterpart.la khelpcenter.la
libkhelpcenterpart_la_SOURCES = khc_factory.cc khc_navigator.cc \
	khc_navigatoritem.cc khc_view.cc
libkhelpcenterpart_la_LIBADD = $(LIB_KHTML) $(LIB_KPARTS)
libkhelpcenterpart_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN)

service_DATA = khelpcenter.desktop
servicedir = $(kde_servicesdir)

data_DATA = Help.desktop
datadir = $(kde_appsdir)

rc_DATA = khelpcenterui.rc intro.html.in glossary.html.in
rcdir = $(kde_datadir)/khelpcenter

messages: rc.cpp
	$(XGETTEXT) *.cc *.cpp -o $(podir)/khelpcenter.pot

