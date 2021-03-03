SUBDIRS = pics

INCLUDES = -I$(srcdir)/../backend $(all_includes)

bin_PROGRAMS = kdm_config kdm_greet 

kdm_config_SOURCES = kdm_config.c

kdm_greet_SOURCES = \
    kdm_greet.c \
    kdmconfig.cpp \
    kdmshutdown.cpp \
    kfdialog.cpp \
    kdmclock.cpp \
    kgreeter.cpp \
    liloinfo.cpp
kdm_greet_LDFLAGS = $(all_libraries) $(KDE_RPATH)
kdm_greet_LDADD = $(LIB_QT) $(LIB_KDECORE) $(LIBXINERAMA) $(LIB_KDEUI)

METASOURCES = AUTO

noinst_PROGRAMS = genkdmconf
genkdmconf_SOURCES = genkdmconf.c
genkdmconf_LDFLAGS = $(all_libraries)
genkdmconf_LDADD = $(LIB_X11)

install-data-local:
	@if test ! -d $(DESTDIR)$(kde_confdir)/kdm; then \
	    $(mkinstalldirs) $(DESTDIR)$(kde_confdir)/kdm; \
	    ./genkdmconf --in $(DESTDIR)$(kde_confdir)/kdm; \
	fi

messages:
	$(XGETTEXT) $(kdm_greet_SOURCES) -o $(podir)/kdmgreet.pot

noinst_HEADERS = kdm_greet.h kdm_config.h kdmshutdown.h kdmclock.h kgreeter.h liloinfo.h kfdialog.h kdmconfig.h

