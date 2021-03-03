# $Id: Makefile.am,v 1.18 2001/06/29 06:52:34 haeckel Exp $

INCLUDES= -I$(srcdir)/../.. -I$(srcdir)/.. $(SSL_INCLUDES) $(all_includes)

kde_module_LTLIBRARIES = kio_smtp.la

kio_smtp_la_SOURCES = smtp.cc
kio_smtp_la_LIBADD  = -lkdesasl $(LIB_KIO)
kio_smtp_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)

noinst_HEADERS = smtp.h

kdelnk_DATA = smtp.protocol smtps.protocol
kdelnkdir = $(kde_servicesdir)

messages:
	$(XGETTEXT) *.cpp -o $(podir)/kio_smtp.pot
