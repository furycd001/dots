INCLUDES= $(all_includes)

SUBDIRS = 

bin_PROGRAMS = kpager

kpager_SOURCES = desktop.cpp kpager.cpp config.cpp windowdrag.cpp main.cpp  

kpager_METASOURCES = AUTO
kpager_LDFLAGS = $(all_libraries) $(KDE_RPATH)

kpager_LDADD = $(LIB_KDEUI)

KDE_ICON = kpager

appsdir = $(kde_appsdir)/Utilities
apps_DATA = kpager.desktop

messages:
	$(XGETTEXT) *.cpp *.h -o $(podir)/kpager.pot
