SUBDIRS= . sounds

kde_module_LTLIBRARIES = libkcm_knotify.la

libkcm_knotify_la_SOURCES = main.cpp knotify.cpp events.cpp

libkcm_knotify_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined
libkcm_knotify_la_LIBADD = $(LIB_KFILE)
INCLUDES= $(all_includes)
 
libkcm_knotify_la_METASOURCES = AUTO

noinst_HEADERS = knotify.h events.h

messages:
	$(XGETTEXT) $(libkcm_knotify_la_SOURCES) -o $(podir)/kcmnotify.pot

data_DATA = kcmnotify.desktop
datadir = $(kde_appsdir)/Settings/Sound

pics_DATA = play.png
picsdir = $(kde_datadir)/kcontrol/pics

# the following should be gone at release time.
install-data-local:
	rm -f $(kde_appsdir)/Settings/LookNFeel/knotify.desktop
	rm -f $(kde_appsdir)/Settings/LookNFeel/kcmnotify.desktop

