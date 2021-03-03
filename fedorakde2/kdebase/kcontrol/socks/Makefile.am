INCLUDES = $(all_includes)
kde_module_LTLIBRARIES = libkcm_socks.la

libkcm_socks_la_SOURCES = socks.cpp socksbase.ui 

libkcm_socks_la_LDFLAGS  = $(all_libraries) -module -avoid-version -no-undefined
libkcm_socks_la_LIBADD = -lkdeui -lkfile

libkcm_socks_la_METASOURCES = AUTO

noinst_HEADERS = socks.h

KDE_ICON = socks

messages: rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/kcmsocks.pot

data_DATA = socks.desktop
datadir = $(kde_appsdir)/Settings/Network
