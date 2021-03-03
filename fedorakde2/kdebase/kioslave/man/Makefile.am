## Makefile.am of kdebase/kioslave/man

INCLUDES= $(all_includes)
LDFLAGS = $(all_libraries) $(KDE_RPATH)

EXTRA_PROGRAMS = kio_man_test man2html_simple

####### just for testing (j.habenicht@europemail.com, 15.02.2001)

kio_man_test_SOURCES = kio_man_test.cpp
kio_man_test_LDFLAGS = $(all_libraries) $(KDE_RPATH)
kio_man_test_LDADD = man2html.o kio_man.o $(LIB_KFILE) $(LIB_KDEUI) $(LIB_KDECORE) $(LIB_QT)

####### Files

kde_module_LTLIBRARIES = kio_man.la

kio_man_la_SOURCES = man2html.cpp kio_man.cpp
kio_man_la_LIBADD = -lksycoca
kio_man_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
noinst_HEADERS = kio_man.h

kdelnk_DATA = man.protocol
kdelnkdir = $(kde_servicesdir)

METASOURCES = AUTO

messages:
	$(XGETTEXT) -C *.cpp *.h -o $(podir)/kio_man.pot

man2html_simple_SOURCES = man2html_simple.cpp
man2html_simple_LDADD = $(LIB_QT)
man2html_simple_LDFLAGS = $(all_libraries) $(KDE_RPATH)

man2html: man2html.cpp
	$(MAKE) DEFS="-DSIMPLE_MAN2HTML $(DEFS)" man2html_simple
	-rm -f man2html
	$(LN_S) man2html_simple man2html

man2html_simple.cpp: 
	$(LN_S) $(srcdir)/man2html.cpp man2html_simple.cpp
