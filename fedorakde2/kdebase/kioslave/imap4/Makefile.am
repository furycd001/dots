INCLUDES= -I$(srcdir)/.. $(SSL_INCLUDES) $(all_includes)

####### Files

kde_module_LTLIBRARIES = kio_imap4.la

kio_imap4_la_SOURCES = 	md5.cc imapcommand.cc imaplist.cc mailaddress.cc \
  mimeheader.cc rfcdecoder.cc imap4.cc imapinfo.cc imapparser.cc mailheader.cc \
  mimehdrline.cc  mimeio.cc
kio_imap4_la_LIBADD  = $(LIB_KIO) $(LIB_KSYCOCA)
kio_imap4_la_LDFLAGS = $(all_libraries) $(KDE_RPATH) -module $(KDE_PLUGIN)

noinst_HEADERS = imap4.h

kdelnk_DATA = imap4.protocol imaps.protocol
kdelnkdir = $(kde_servicesdir)

check_PROGRAMS = linktest
linktest_SOURCES = $(kio_imap4_la_SOURCES) linktest.cpp
linktest.cpp:
	echo "int main() {};" > linktest.cpp
linktest_LDFLAGS  = $(all_libraries) $(KDE_RPATH)
linktest_LDADD = $(kio_imap4_la_LIBADD)

messages:
	$(XGETTEXT) *.cc -o $(podir)/kio_imap4.pot
