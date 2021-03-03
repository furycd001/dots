INCLUDES = $(all_includes)

SUBDIRS = share core ui . proxy taskmanager taskbar applets extensions data

bin_PROGRAMS = kicker
lib_LTLIBRARIES = kicker.la

CLEANFILES = dummy.cpp

kicker_la_LIBADD = share/libkicker_share.la core/libkicker_core.la  ui/libkicker_ui.la $(LIB_KSYCOCA) $(LIB_KFILE)
kicker_la_SOURCES = dummy.cpp
kicker_la_LDFLAGS = $(KDE_PLUGIN) -no-undefined -module

kicker_LDADD = kicker.la
kicker_SOURCES = dummy.cpp
kicker_LDFLAGS = $(all_libraries) $(KDE_RPATH)

autostart_DATA = panel.desktop
autostartdir = $(prefix)/share/autostart

messages:
	$(XGETTEXT) share/*.cpp core/*.cpp ui/*.cpp -o $(podir)/kicker.pot

dummy.cpp:
	echo > dummy.cpp
