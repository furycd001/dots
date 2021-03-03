INCLUDES = -I$(top_builddir)/lib $(all_includes)

METASOURCES = AUTO


kde_module_LTLIBRARIES = libkcm_kdb.la

#libkcm_kdb_la_SOURCES = kcmkdb.cpp connectionlist.cpp connectionconfig.cpp pluginconfig.cpp
libkcm_kdb_la_SOURCES = kcmkdb.cpp connectionconfig.cpp pluginconfig.cpp

libkcm_kdb_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN)

libkcm_kdb_la_LIBADD = $(LIB_KDEUI) $(LIB_KDB) -lkdbcore -lkdbui


kcmdata_DATA = kcmkdb.desktop

kcmdatadir = $(kde_appsdir)/Settings/Databases

messages:
	$(XGETTEXT) *.cpp -o $(podir)/kcmkdb.pot

