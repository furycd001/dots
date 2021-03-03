# $Id: Makefile.am,v 1.5 2001/07/12 11:56:22 lukas Exp $

INCLUDES = $(all_includes)

kde_module_LTLIBRARIES = libkwinmodernsys_config.la

libkwinmodernsys_config_la_SOURCES = config.cpp
libkwinmodernsys_config_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN)
libkwinmodernsys_config_la_LIBADD = $(LIB_KDEUI)

METASOURCES = AUTO
noinst_HEADERS = config.h

lnkdir = $(kde_datadir)/kwin/

messages:
	$(XGETTEXT) *.cpp -o $(podir)/libkwinmodernsys_config.pot
