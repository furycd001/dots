kde_module_LTLIBRARIES = libkcm_spellchecking.la

libkcm_spellchecking_la_SOURCES = spellchecking.cpp

libkcm_spellchecking_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined
libkcm_spellchecking_la_LIBADD = -lkdeui -lkspell

INCLUDES= $(all_includes)

libkcm_spellchecking_la_METASOURCES = spellchecking.moc

KDE_ICON = spellchecking

messages:
	$(XGETTEXT) $(libkcm_spellchecking_la_SOURCES) -o $(podir)/kcmspellchecking.pot

data_DATA = spellchecking.desktop
datadir = $(kde_appsdir)/Settings/Personalization
