kde_module_LTLIBRARIES = libkcm_audiocd.la

libkcm_audiocd_la_SOURCES = audiocdconfig.ui kcmaudiocd.cpp

libkcm_audiocd_la_LDFLAGS  = $(all_libraries) -module -avoid-version -no-undefined
libkcm_audiocd_la_LIBADD = -lkdeui

INCLUDES = $(all_includes)

libkcm_audiocd_la_METASOURCES = AUTO

KDE_ICON = cdrom_unmount

messages: rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/kcmaudiocd.pot

DISTCLEANFILES = $(lib_audiocd_METASOURCES)

data_DATA = audiocd.desktop
datadir = $(kde_appsdir)/Settings/Sound

audiocdconfig.cpp: audiocdconfig.h
