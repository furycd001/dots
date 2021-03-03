## Makefile.am of kdebase/kdesktop

INCLUDES = -I$(top_srcdir)/kcontrol/background -I$(top_srcdir)/libkonq $(all_includes)
kdesktop_la_LDFLAGS  = $(all_libraries) -lkdesu -module -avoid-version
kdesktop_la_LIBADD   = $(top_builddir)/libkonq/libkonq.la $(top_builddir)/kcontrol/background/libbgnd.la 
kdesktop_LDFLAGS  = $(KDE_RPATH) $(all_libraries)
kdesktop_LDADD    = kdesktop.la

SUBDIRS = . pics patterns programs init kwebdesktop

####### Files

bin_PROGRAMS = kdesktop kdmdesktop
bin_SCRIPTS = kdeeject
lib_LTLIBRARIES = kdesktop.la

kdesktop_la_SOURCES = krootwm.cc xautolock.cc kdiconview.cc desktop.cc main.cc \
	lockeng.cc lockdlg.cc KDesktopIface.skel \
	bgmanager.cc init.cc KScreensaverIface.skel \
	minicli.cpp KBackgroundIface.skel pixmapserver.cc kcustommenu.cc \
	startupid.cpp

kdesktop_SOURCES = dummy.cc

include_HEADERS = KDesktopIface.h KScreensaverIface.h KBackgroundIface.h

noinst_HEADERS = desktop.h bgmanager.h krootwm.h \
	xautolock.h lockeng.h lockdlg.h init.h minicli.h \
	pixmapserver.h startupid.h

kdmdesktop_SOURCES = kdmdesktop.cpp
kdmdesktop_LDFLAGS = $(KDE_RPATH) $(all_libraries) 
kdmdesktop_LDADD   = $(top_builddir)/kcontrol/background/libbgnd.la

METASOURCES = AUTO

####### Build rules

messages:
	$(XGETTEXT) *.cc *.cpp *.h -o $(podir)/kdesktop.pot

dummy.cc:
	echo > dummy.cc

menu_DATA = kdesktop_custom_menu1 kdesktop_custom_menu2
menudir = $(kde_confdir)

autostart_DATA = kdesktop.desktop
autostartdir = $(prefix)/share/autostart

PAM = @KSCREENSAVER_PAM_SERVICE@

install-data-local:
	@test -n "$(DESTDIR)" || test -z "$(PAM)" || $(top_srcdir)/mkpamserv $(PAM)
