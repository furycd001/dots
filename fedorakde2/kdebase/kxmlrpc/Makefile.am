# Makefile for kxmlrpcd.
# Written by Kurt Granroth using tons of KDE Makefiles as examples

INCLUDES         = $(all_includes)

bin_PROGRAMS     = kxmlrpcd
kxmlrpcd_LDADD   = kxmlrpcd.la
kxmlrpcd_LDFLAGS = $(KDE_RPATH)
kxmlrpcd_SOURCES = dummy.cpp

lib_LTLIBRARIES        = kxmlrpcd.la libkcm_xmlrpcd.la
kxmlrpcd_la_LIBADD  = $(LIB_KSYCOCA)
kxmlrpcd_la_LDFLAGS = $(all_libraries) -module -avoid-version 
kxmlrpcd_la_SOURCES = kxmlrpcserver.cpp kxmlrpcparser.cpp \
                         kxmlrpcdaemon.cpp kxmlrpcutil.cpp
kxmlrpcd_la_METASOURCES = AUTO

libkcm_xmlrpcd_la_LIBADD = $(LIB_KDECORE)
libkcm_xmlrpcd_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN)
libkcm_xmlrpcd_la_SOURCES = kxmlrpcinit.cpp

service_DATA = kxmlrpcd.desktop
servicedir = $(kde_servicesdir)

noinst_HEADERS   = kxmlrpcdaemon.h kxmlrpcparser.h kxmlrpcserver.h kxmlrpcutil.h

messages: rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/kxmlrpcd.pot

dummy.cpp:
	echo > dummy.cpp
