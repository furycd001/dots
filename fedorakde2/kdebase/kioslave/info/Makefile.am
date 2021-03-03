## Makefile.am of kdebase/kioslave/info

INCLUDES = $(all_includes)

METASOURCES = AUTO

####### Files

kde_module_LTLIBRARIES = kio_info.la

kio_info_la_SOURCES = info.cc
kio_info_la_LIBADD = $(LIB_KIO)
kio_info_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
noinst_HEADERS = info.h

kdelnk_DATA = info.protocol
kdelnkdir = $(kde_servicesdir)

data_DATA = kde-info2html kde-info2html.conf
datadir = $(kde_datadir)/kio_info
