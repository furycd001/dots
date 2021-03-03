# $Id: Makefile.am,v 1.9 2001/06/18 05:20:43 waba Exp $

SUBDIRS = . pics

INCLUDES = $(all_includes)

kde_module_LTLIBRARIES = libkcm_background.la

libkcm_background_la_SOURCES = backgnd.cpp bgdialogs.cpp 
libkcm_background_la_LDFLAGS = -module -avoid-version $(all_libraries) -no-undefined
libkcm_background_la_LIBADD = $(LIB_KFILE) libbgnd.la

METASOURCES = AUTO

noinst_HEADERS = backgnd.h bgdialogs.h bgsettings.h bgrender.h 

noinst_LTLIBRARIES = libbgnd.la
libbgnd_la_SOURCES = bgsettings.cc bgrender.cc
libbgnd_la_LIBADD = $(LIB_KSYCOCA)
libbgnd_la_LDFLAGS = -no-undefined $(all_libraries)

messages:
	$(XGETTEXT) -kni18n *.cc *.cpp -o $(podir)/kcmbackground.pot

desktop_DATA =  background.desktop
desktopdir = $(kde_appsdir)/Settings/LookNFeel

EXTRA_DIST = $(desktop_DATA)
