INCLUDES = -I$(srcdir)/../../taskmanager -I$(srcdir)/../../taskbar $(all_includes)

lib_LTLIBRARIES = libtaskbarextension.la

libtaskbarextension_la_SOURCES = taskbarextension.cpp
libtaskbarextension_la_METASOURCES = AUTO
libtaskbarextension_la_LDFLAGS = $(all_libraries) -version-info 1:0:0 -module -no-undefined
libtaskbarextension_la_LIBADD = $(LIB_KDEUI) ../../taskmanager/libtaskmanager.la ../../taskbar/libtaskbar.la

noinst_HEADERS = taskbarextension.h

lnkdir = $(kde_datadir)/kicker/extensions
lnk_DATA = taskbarextension.desktop

EXTRA_DIST = $(lnk_DATA)

messages:
	$(XGETTEXT) *.cpp *.h -o $(podir)/taskbarextension.pot
