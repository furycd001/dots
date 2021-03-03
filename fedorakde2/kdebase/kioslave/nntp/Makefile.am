INCLUDES= -I$(srcdir)/../.. -I$(srcdir)/.. $(all_includes)

####### Files

kde_module_LTLIBRARIES = kio_nntp.la

kio_nntp_la_SOURCES = nntp.cpp
kio_nntp_la_LIBADD  = $(LIB_KIO)
kio_nntp_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)

METASOURCES = AUTO

noinst_HEADERS = nntp.h

kdelnk_DATA = nntp.protocol
kdelnkdir = $(kde_servicesdir)

messages:
	$(XGETTEXT) *.cpp -o $(podir)/kio_nntp.pot
