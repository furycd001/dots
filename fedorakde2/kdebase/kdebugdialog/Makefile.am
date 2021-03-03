## $Id: Makefile.am,v 1.5 2000/04/08 12:46:38 faure Exp $
## Makefile.am of kdebugdialog

INCLUDES=	$(all_includes)
LDFLAGS =       $(all_libraries) $(KDE_RPATH)
LDADD	=	$(LIB_KDEUI)

####### Files

bin_PROGRAMS = kdebugdialog

kdebugdialog_SOURCES = main.cpp kabstractdebugdialog.cpp kdebugdialog.cpp klistdebugdialog.cpp
kdebugdialog_METASOURCES = AUTO

messages:
	$(XGETTEXT) *.cpp -o $(podir)/kdebugdialog.pot	

