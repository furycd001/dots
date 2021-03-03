
SUBDIRS = plugins

INCLUDES = -I$(srcdir)/plugins/ikws $(all_includes)
LDFLAGS =  $(all_libraries)

METASOURCES = AUTO

kde_module_LTLIBRARIES = libkcm_kurifilt.la

libkcm_kurifilt_la_SOURCES = filteropts.cpp main.cpp

libkcm_kurifilt_la_LDFLAGS  = $(all_libraries) -module -avoid-version -no-undefined
libkcm_kurifilt_la_LIBADD = $(LIB_KSYCOCA)
# Don't link this lib to the plugins. They are dlopened and linking to them
# results in crashes when closing kcmshell.
# plugins/ikws/libkuriikwsfilter.la plugins/shorturi/libkshorturifilter.la

messages:
	$(XGETTEXT) $(libkcm_kurifilt_la_SOURCES) plugins/ikws/*.cpp plugins/shorturi/*.cpp -o $(podir)/kcmkurifilt.pot

data_DATA = ebrowsing.desktop
datadir = $(kde_appsdir)/Settings/WebBrowsing
