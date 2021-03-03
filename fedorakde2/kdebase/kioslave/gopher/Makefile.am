# $Id: Makefile.am,v 1.8 2001/05/21 13:24:42 coolo Exp $
# Makefile for kdebase/kioslave/gopher

INCLUDES= -I$(srcdir)/../.. -I$(srcdir)/.. $(all_includes)

####### Files

kde_module_LTLIBRARIES = kio_gopher.la

kio_gopher_la_SOURCES = gopher.cc
kio_gopher_la_LIBADD  = $(LIB_KIO)
kio_gopher_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)

noinst_HEADERS = gopher.h

kdelnk_DATA = gopher.protocol
kdelnkdir = $(kde_servicesdir)

messages:
	$(XGETTEXT) *.cc -o $(podir)/kio_gopher.pot
