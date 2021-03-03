INCLUDES = -I$(srcdir)/../../core -I$(srcdir)/../../share -I$(srcdir)/../../ui $(all_includes)

lib_LTLIBRARIES = libchildpanelextension.la

libchildpanelextension_la_SOURCES = childpanelextension.cpp childpanelextension.skel
libchildpanelextension_la_METASOURCES = AUTO
libchildpanelextension_la_LDFLAGS = $(all_libraries) -version-info 1:0:0 -module 
libchildpanelextension_la_LIBADD = $(LIB_KDEUI) $(LIB_KSYCOCA) ../../kicker.la

noinst_HEADERS = childpanelextension.h

lnkdir = $(kde_datadir)/kicker/extensions
lnk_DATA = childpanelextension.desktop

EXTRA_DIST = $(lnk_DATA)

messages:
	$(XGETTEXT) *.cpp *.h -o $(podir)/childpanelextension.pot
