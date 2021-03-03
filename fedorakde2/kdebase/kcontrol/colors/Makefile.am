# $Id: Makefile.am,v 1.2 2000/12/09 11:07:23 faure Exp $

INCLUDES = $(all_includes)

kde_module_LTLIBRARIES = libkcm_colors.la

libkcm_colors_la_SOURCES = colorscm.cpp widgetcanvas.cpp
libkcm_colors_la_LDFLAGS = -module -avoid-version $(all_libraries) -no-undefined
libkcm_colors_la_LIBADD = $(LIB_KDEUI)
libkcm_colors_la_METASOURCES = colorscm.moc widgetcanvas.moc

noinst_HEADERS = colorscm.h widgetcanvas.h

messages:
	$(XGETTEXT) -kni18n $(libkcm_colors_la_SOURCES) -o $(podir)/kcmcolors.pot

lnf_DATA = colors.desktop
lnfdir = $(kde_appsdir)/Settings/LookNFeel

EXTRA_DIST = $(lnf_DATA)
