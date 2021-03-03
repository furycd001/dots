INCLUDES = $(all_includes)

bin_PROGRAMS = kjobviewer
lib_LTLIBRARIES = kjobviewer.la

kjobviewer_SOURCES = dummy.cpp
kjobviewer_LDADD = kjobviewer.la
kjobviewer_LDFLAGS = $(KDE_RPATH) $(all_libraries)

kjobviewer_la_SOURCES = kjobviewer.cpp main.cpp
kjobviewer_la_METASOURCES = AUTO
kjobviewer_la_LIBADD = -lkdeprint_management $(LIB_KDEUI)
kjobviewer_la_LDFLAGS = -module -avoid-version $(all_libraries) $(KDE_RPATH)

noinst_HEADERS = kjobviewer.h

KDE_ICON = kjobviewer

rc_DATA = kjobviewerui.rc
rcdir = $(kde_datadir)/kjobviewer

desktop_DATA = kjobviewer.desktop
desktopdir = $(kde_appsdir)/Utilities

dummy.cpp:
	echo > dummy.cpp

messages: rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/kjobviewer.pot
