kde_module_LTLIBRARIES = libkcm_midi.la

libkcm_midi_la_SOURCES = midi.cpp 

libkcm_midi_la_LDFLAGS  = $(all_libraries) -module -avoid-version -no-undefined
libkcm_midi_la_LIBADD = -lkdeui -lkfile -lkmid 
INCLUDES= $(all_includes) 

libkcm_midi_la_METASOURCES = AUTO

noinst_HEADERS = midi.h

KDE_ICON = midi

messages:
	$(XGETTEXT) $(libkcm_midi_la_SOURCES) -o $(podir)/kcmmidi.pot

data_DATA = midi.desktop
datadir = $(kde_appsdir)/Settings/Sound
