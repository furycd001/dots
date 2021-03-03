INCLUDES= -I$(srcdir)/../.. -I$(srcdir)/.. $(SSL_INCLUDES) $(all_includes)

####### Files

kde_module_LTLIBRARIES = kio_pop3.la

kio_pop3_la_SOURCES = md5.cc pop3.cc
kio_pop3_la_LIBADD  = $(LIB_KIO)
kio_pop3_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)

noinst_HEADERS = pop3.h

kdelnk_DATA = pop3.protocol pop3s.protocol
kdelnkdir = $(kde_servicesdir)

messages:
	$(XGETTEXT) *.cc -o $(podir)/kio_pop3.pot
