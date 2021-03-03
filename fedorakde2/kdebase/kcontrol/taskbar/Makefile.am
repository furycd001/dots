kde_module_LTLIBRARIES = libkcm_taskbar.la

libkcm_taskbar_la_SOURCES = kcmtaskbarui.ui kcmtaskbar.cpp
libkcm_taskbar_la_LDFLAGS  = $(all_libraries) -module -avoid-version -no-undefined
libkcm_taskbar_la_LIBADD = -lkdeui
INCLUDES= $(all_includes) 

libkcm_taskbar_la_METASOURCES = AUTO

noinst_HEADERS = kcmtaskbar.h

KDE_ICON = go

messages: rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/kcmtaskbar.pot

data_DATA = kcmtaskbar.desktop
datadir = $(kde_appsdir)/Settings/LookNFeel/

EXTRA_DIST = $(data_DATA)
