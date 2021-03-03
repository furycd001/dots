INCLUDES = -I$(srcdir)/../share $(all_includes)

CLEANFILES = dummy.cpp

bin_PROGRAMS = appletproxy extensionproxy
lib_LTLIBRARIES = appletproxy.la extensionproxy.la

appletproxy_la_LIBADD = $(LIB_KDEUI) ../share/libkicker_share.la
appletproxy_la_SOURCES = appletproxy.cpp
appletproxy_la_METASOURCES = appletproxy.moc
appletproxy_la_LDFLAGS = $(all_libraries) $(KDE_RPATH) -module -avoid-version

appletproxy_LDADD = appletproxy.la
appletproxy_SOURCES = dummy.cpp
appletproxy_LDFLAGS = $(all_libraries) $(KDE_RPATH)

extensionproxy_la_LIBADD = $(LIB_KDEUI) ../share/libkicker_share.la
extensionproxy_la_SOURCES = extensionproxy.cpp
extensionproxy_la_METASOURCES = extensionproxy.moc
extensionproxy_la_LDFLAGS = $(all_libraries) $(KDE_RPATH) -module -avoid-version

extensionproxy_LDADD = extensionproxy.la
extensionproxy_SOURCES = dummy.cpp
extensionproxy_LDFLAGS = $(all_libraries) $(KDE_RPATH)

noinst_HEADERS = appletproxy.h extensionproxy.h

messages:
	$(XGETTEXT) appletproxy.cpp -o $(podir)/appletproxy.pot
	$(XGETTEXT) extensionproxy.cpp -o $(podir)/extensionproxy.pot

dummy.cpp:
	echo > dummy.cpp

