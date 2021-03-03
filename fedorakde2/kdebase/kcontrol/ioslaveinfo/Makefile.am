kde_module_LTLIBRARIES = libkcm_ioslaveinfo.la

libkcm_ioslaveinfo_la_SOURCES = kcmioslaveinfo.cpp

libkcm_ioslaveinfo_la_LDFLAGS  = $(all_libraries) -module -avoid-version -no-undefined
libkcm_ioslaveinfo_la_LIBADD = -lkdeui -lkfile

INCLUDES= $(all_includes)

SUBDIRS = 

libkcm_ioslaveinfo_la_METASOURCES = AUTO

messages:
	$(XGETTEXT) $(libkcm_ioslaveinfo_la_SOURCES) -o $(podir)/kcmioslaveinfo.pot

data_DATA = ioslaveinfo.desktop
datadir = $(kde_appsdir)/Settings/Network

install-data-local: uninstall.desktop
	$(mkinstalldirs) $(kde_appsdir)/Settings/Network
	$(INSTALL_DATA) $(srcdir)/uninstall.desktop $(kde_appsdir)/Settings/Information/ioslaveinfo.desktop

