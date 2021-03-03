# $Id: Makefile.am,v 1.4 2001/05/26 17:11:58 andrea Exp $

kde_module_LTLIBRARIES = libkcm_konsole.la

libkcm_konsole_la_SOURCES =  schemaeditor.cpp kcmkonsole.cpp \
	schemadialog.ui kcmkonsoledialog.ui

libkcm_konsole_la_LDFLAGS  = $(all_libraries) -module -avoid-version -no-undefined
libkcm_konsole_la_LIBADD = -lkdeui -lkfile 
INCLUDES= $(all_includes) 

libkcm_konsole_la_METASOURCES = AUTO

noinst_HEADERS = kcmkonsole.h schemaeditor.h schemadialog.h kcmkonsoledialog.h

KDE_ICON = konsole

data_DATA = kcmkonsole.desktop
datadir = $(kde_appsdir)/Settings/System

messages: rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/kcmkonsole.pot
