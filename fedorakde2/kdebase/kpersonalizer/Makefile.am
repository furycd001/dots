bin_PROGRAMS = kpersonalizer
kpersonalizer_SOURCES = klocaleadv.cpp klanguagebutton.cpp krefinepage.cpp \
                                          kstylepage.cpp keyecandypage.cpp kospage.cpp kcountrypage.cpp kpersonalizer.cpp \
                                          main.cpp \
                                          kcountrypagedlg.ui kospagedlg.ui keyecandypagedlg.ui kstylepagedlg.ui krefinepagedlg.ui

kpersonalizer_LDADD   = $(LIB_KFILE) 

EXTRA_DIST = main.cpp kpersonalizer.cpp kpersonalizer.h kpersonalizer.desktop kcountrypage.cpp kcountrypage.h kospage.cpp kospage.h keyecandypage.cpp keyecandypage.h kstylepage.cpp kstylepage.h krefinepage.cpp krefinepage.h klanguagebutton.cpp klanguagebutton.h klocaleadv.cpp klocaleadv.h hi16-app-kpersonalizer.png hi32-app-kpersonalizer.png README kcountrypagedlg.ui kospage.ui keyecandypagedlg.ui kstylepagedlg.ui krefinepagedlg.ui

desktop_DATA = kpersonalizer.desktop
desktopdir = $(kde_appsdir)/System

autostart_DATA = kpersonalizer.desktop
autostartdir = $(prefix)/share/autostart

####### kdevelop will overwrite this part!!! (end)############
SUBDIRS		= pics

# this 10 paths are KDE specific. Use them:
# kde_htmldir       Where your docs should go to. (contains lang subdirs)
# kde_appsdir       Where your application file (.kdelnk) should go to. 
# kde_icondir       Where your icon should go to.
# kde_minidir       Where your mini icon should go to.
# kde_datadir       Where you install application data. (Use a subdir)
# kde_locale        Where translation files should go to.(contains lang subdirs)
# kde_cgidir        Where cgi-bin executables should go to.
# kde_confdir       Where config files should go to.
# kde_mimedir       Where mimetypes should go to.
# kde_toolbardir    Where general toolbar icons should go to.
# kde_wallpaperdir  Where general wallpapers should go to.

# set the include path for X, qt and KDE
INCLUDES= $(all_includes)

METASOURCES = AUTO

KDE_ICON= AUTO
# the library search path. 
kpersonalizer_LDFLAGS = $(all_libraries) $(KDE_RPATH)

# Uncomment the following two lines if you add a ui.rc file for your application to make use of
# KDE´s XML GUI builing
#rcdir = $(kde_datadir)/kpersonalizer
#rc_DATA = kpersonalizerui.rc

messages: rc.cpp
	LIST=`find . -name \*.h -o -name \*.hh -o -name \*.H -o -name \*.hxx -o -name \*.hpp -o -name \*.cpp -o -name \*.cc -o -name \*.cxx -o -name \*.ecpp -o -name \*.C`; \
	if test -n "$$LIST"; then \
	 $(XGETTEXT) $$LIST -o $(podir)/kpersonalizer.pot; \
	fi

