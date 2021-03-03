## Makefile.am of kdebase/kscreensaver/kxsconfig

INCLUDES = $(all_includes)
kxsconfig_LDFLAGS  = $(KDE_RPATH) $(all_libraries) 
kxsconfig_LDADD    = $(LIB_KDEUI)

kxsrun_LDFLAGS  = $(KDE_RPATH) $(all_libraries) 
kxsrun_LDADD    = $(LIB_KDEUI) $(LIB_KDECORE)

SUBDIRS = config

####### Files

bin_PROGRAMS = kxsconfig kxsrun

kxsconfig_SOURCES = kxsconfig.cpp kxscontrol.cpp kxsitem.cpp
kxsrun_SOURCES = kxsrun.cpp kxsitem.cpp

noinst_HEADERS = kxsconfig.h kxscontrol.h kxsitem.h

METASOURCES = AUTO

####### Build rules

messages:
	grep Label= config/*rc | cut -d':' -f2- | sed -e "s#Label=\(.*\)#i18n(\"\1\");#" > labels.cpp
	$(XGETTEXT) *.cpp *.h -o $(podir)/kxsconfig.pot
	-rm -f labels.cpp

