kde_module_LTLIBRARIES = libkcm_bell.la

libkcm_bell_la_SOURCES = bell.cpp

libkcm_bell_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined
libkcm_bell_la_LIBADD = -lkdeui

INCLUDES= $(all_includes)

libkcm_bell_la_METASOURCES = bell.moc

KDE_ICON = bell

messages:
	$(XGETTEXT) $(libkcm_bell_la_SOURCES) -o $(podir)/kcmbell.pot

data_DATA = bell.desktop
datadir = $(kde_appsdir)/Settings/Sound
