# $Id: Makefile.am,v 1.89 2001/05/23 00:09:36 waba Exp $
# Makefile.am for kscreensave.
#

AM_CPPFLAGS = -UQT_NO_ASCII_CAST

INCLUDES = $(all_includes)
LDFLAGS = $(all_libraries) $(KDE_RPATH)
LDADD = $(LIB_KDEUI) -lm

bin_PROGRAMS = krandom.kss

SUBDIRS = kdesavers xsavers kxsconfig kpartsaver

krandom_kss_SOURCES = random.cpp

messages:
	$(EXTRACTRC) */*.ui > rc.cpp
	$(XGETTEXT) rc.cpp random.cpp */*.cpp */*.h -o $(podir)/klock.pot

########## Meta objects ##########

data_DATA = 
datadir = $(kde_datadir)/kscreensaver

desktop_DATA = KRandom.desktop
desktopdir = $(kde_appsdir)/System/ScreenSavers

EXTRA_DIST = $(desktop_DATA)
