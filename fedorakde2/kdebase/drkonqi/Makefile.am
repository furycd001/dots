# $Id: Makefile.am,v 1.13 2001/03/23 00:42:57 waba Exp $

AM_CPPFLAGS = -DQT_NO_CAST_ASCII -DQT_NO_COMPAT

SUBDIRS = presets pics debuggers

INCLUDES = $(all_includes)

EXTRA_DIST = LICENSE

bin_PROGRAMS = drkonqi

# Libraries:
LDFLAGS = $(all_libraries) $(KDE_RPATH)
LDADD  = $(LIB_KDEUI) $(LIB_KFILE)

# toplevel.cpp must be last in row due to X-headers being included. (--enable-final)
# Did I mention already that X header files really suck?
drkonqi_SOURCES = main.cpp debugger.cpp krashconf.cpp \
	drbugreport.cpp backtrace.cpp toplevel.cpp

drkonqi_METASOURCES = AUTO

check_PROGRAMS = crashtest
crashtest_SOURCES = crashtest.cpp

messages:
	$(XGETTEXT) $(drkonqi_SOURCES) -o $(podir)/drkonqi.pot
