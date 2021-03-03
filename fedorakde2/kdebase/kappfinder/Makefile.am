SUBDIRS = apps

bin_PROGRAMS = kappfinder
kappfinder_SOURCES = main.cpp scanner.cpp checker.cpp
kappfinder_LDADD = $(LIB_KDEUI)
kappfinder_LDFLAGS = $(all_libraries) $(KDE_RPATH)

noinst_PROGRAMS = kappfinder_install
kappfinder_install_SOURCES = main_install.cpp checker.cpp
kappfinder_install_LDADD = $(LIB_KDEUI)
kappfinder_install_LDFLAGS = $(all_libraries) $(KDE_RPATH)

METASOURCES = AUTO
INCLUDES = $(all_includes)

install-data-local: runkappfinder_$(RUN_KAPPFINDER)

runkappfinder_yes:
	echo "Installing non-KDE applications"
	pwd=`pwd`; \
	(cd $(srcdir) && $$pwd/kappfinder_install "$(DESTDIR)$(kde_appsdir)")

runkappfinder_no:

KDE_ICON = kappfinder

data_DATA = kappfinder.desktop
datadir = $(kde_appsdir)/System

EXTRA_DIST = $(data_DATA)

messages:
	$(XGETTEXT) $(kappfinder_SOURCES) -o $(podir)/kappfinder.pot
