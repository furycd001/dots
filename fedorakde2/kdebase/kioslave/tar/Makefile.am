## $Id: Makefile.am,v 1.12 2001/03/30 22:53:36 faure Exp $
## Makefile.am of kdebase/kioslave/tar

INCLUDES = $(all_includes)
LDFLAGS = $(all_libraries) $(KDE_RPATH)
METASOURCES = AUTO

kde_module_LTLIBRARIES = kio_tar.la

kio_tar_la_SOURCES = tar.cc
kio_tar_la_LIBADD = $(LIB_KSYCOCA)
kio_tar_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)

check_PROGRAMS = ktartest

ktartest_SOURCES = ktartest.cpp
ktartest_LDADD = $(LIB_KSYCOCA)

noinst_HEADERS = tar.h

kdelnk_DATA = tar.protocol
kdelnkdir = $(kde_servicesdir)

