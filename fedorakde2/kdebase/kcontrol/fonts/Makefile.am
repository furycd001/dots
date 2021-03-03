# $Id: Makefile.am,v 1.3 2000/12/09 11:07:31 faure Exp $

INCLUDES = $(all_includes)

kde_module_LTLIBRARIES = libkcm_fonts.la

libkcm_fonts_la_SOURCES = fonts.cpp
libkcm_fonts_la_LDFLAGS = -module -avoid-version $(all_libraries) -no-undefined
libkcm_fonts_la_LIBADD = $(LIB_KDEUI) 
libkcm_fonts_la_METASOURCES = fonts.moc

noinst_HEADERS = fonts.h

messages:
	$(XGETTEXT) -kni18n $(libkcm_fonts_la_SOURCES) -o $(podir)/kcmfonts.pot

lnf_DATA = fonts.desktop
lnfdir = $(kde_appsdir)/Settings/LookNFeel

EXTRA_DIST = $(lnf_DATA)
