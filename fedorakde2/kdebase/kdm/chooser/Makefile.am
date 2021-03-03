# $Id: Makefile.am,v 1.8 2001/07/02 10:00:29 ossi Exp $

AM_CXXFLAGS = $(XDM_CFLAGS)
AM_CFLAGS = $(XDM_CFLAGS)

INCLUDES = -I$(srcdir)/../backend $(all_includes)

bin_PROGRAMS = chooser

chooser_SOURCES = \
    chooser.cpp \
    CXdmcp.cpp \
    CXdmcp_c.c \
    DXdmcp.cpp \
    kfdialog.cpp

chooser_LDFLAGS = $(all_libraries) $(KDE_RPATH)
chooser_LDADD  = $(LIB_KDEUI) -lXau $(LIBXDMCP) $(LIB_LIBS) \
    $(LIBSOCKET) $(LIBUCB) $(LIBUTIL)  -lXt

noinst_HEADERS = CXdmcp_c.h CXdmcp.h DXdmcp.h kfdialog.h

METASOURCES = CXdmcp.moc DXdmcp.moc kfdialog.moc

messages:
	$(XGETTEXT) $(chooser_SOURCES) -o $(podir)/kdmchooser.pot

EXTRA_DIST = Chooser.ad
