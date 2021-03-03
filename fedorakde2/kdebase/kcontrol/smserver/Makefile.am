kde_module_LTLIBRARIES = libkcm_smserver.la

libkcm_smserver_la_SOURCES = kcmsmserver.cpp smserverconfigdlg.ui smserverconfigimpl.cpp
libkcm_smserver_la_LDFLAGS  = $(all_libraries) -module -avoid-version -no-undefined
libkcm_smserver_la_LIBADD = -lkdeui
INCLUDES= $(all_includes) 

libkcm_smserver_la_METASOURCES = AUTO

noinst_HEADERS = kcmsmserver.h smserverconfigimpl.h

KDE_ICON = go

messages: rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/kcmsmserver.pot

data_DATA = kcmsmserver.desktop
datadir = $(kde_appsdir)/Settings/System/

EXTRA_DIST = $(data_DATA)
