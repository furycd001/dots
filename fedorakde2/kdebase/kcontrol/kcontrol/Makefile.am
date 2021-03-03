#
#  Copyright (c) 1999 Matthias Hoelzer-Kluepfel <hoelzer@kde.org>
#  Copyright (c) 2000 Matthias Elter <elter@kde.org>
# 
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
# 
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
# 
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
# 

INCLUDES= $(all_includes)
METASOURCES = AUTO

SUBDIRS = . pics

CLEANFILES = dummy.cpp

bin_PROGRAMS = kcontrol kcmshell kcminit
kde_module_LTLIBRARIES = kcontrol.la kcmshell.la kcminit.la

kcontrol_la_SOURCES = main.cpp toplevel.cpp indexwidget.cpp searchwidget.cpp \
	helpwidget.cpp moduleinfo.cpp utils.cpp modules.cpp proxywidget.cpp \
	modloader.cpp dockcontainer.cpp aboutwidget.cpp global.cpp \
	quickhelp.cpp moduletreeview.cpp moduleiconview.cpp \
	kecdialog.cpp kcrootonly.cpp 

kcontrol_la_LIBADD  = $(LIB_KDEUI) $(LIB_KSYCOCA)
kcontrol_la_LDFLAGS = $(all_libraries) -module -avoid-version

kcmshell_la_SOURCES = kcmshell.cpp moduleinfo.cpp modloader.cpp kecdialog.cpp \
			utils.cpp proxywidget.cpp kcdialog.cpp global.cpp \
			kcdialog.skel
kcmshell_la_LIBADD = $(LIB_KDEUI) $(LIB_KSYCOCA)
kcmshell_la_LDFLAGS = $(all_libraries) -module -avoid-version


kcminit_la_SOURCES = kcminit.cpp global.cpp utils.cpp
kcminit_la_LIBADD   = $(LIB_KDEUI) $(LIB_KSYCOCA)
kcminit_la_LDFLAGS = $(all_libraries) -module -avoid-version

kcontrol_SOURCES = dummy.cpp
kcontrol_LDADD = kcontrol.la
kcontrol_LDFLAGS = $(all_libraries) $(KDE_RPATH) -export-dynamic

kcmshell_SOURCES = dummy.cpp
kcmshell_LDADD = kcmshell.la
kcmshell_LDFLAGS = $(all_libraries) $(KDE_RPATH) -export-dynamic

kcminit_SOURCES = dummy.cpp
kcminit_LDADD = kcminit.la
kcminit_LDFLAGS = $(all_libraries) $(KDE_RPATH) -export-dynamic

rcdir = $(kde_datadir)/kcontrol
rc_DATA = kcontrolui.rc

data_DATA = KControl.desktop
datadir = $(kde_appsdir)

KDE_ICON=AUTO

EXTRA_DIST = $(data_DATA)

messages: rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/kcontrol.pot

dummy.cpp:
	echo > dummy.cpp
