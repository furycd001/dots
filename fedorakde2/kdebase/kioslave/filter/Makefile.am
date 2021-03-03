KDE_CPPFLAGS = -DQT_NO_CAST_ASCII

INCLUDES = $(all_includes)

kde_module_LTLIBRARIES = kio_filter.la

kio_filter_la_SOURCES = filter.cc
kio_filter_la_LIBADD = $(LIB_KSYCOCA)
kio_filter_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
noinst_HEADERS = filter.h

METASOURCES = AUTO

protocoldir = $(kde_servicesdir)
protocol_DATA = gzip.protocol $(BZIP2FILES)

