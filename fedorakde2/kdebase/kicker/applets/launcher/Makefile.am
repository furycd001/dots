
INCLUDES = -I$(top_srcdir)/kicker/share -I$(top_srcdir)/kicker/ui $(all_includes)

lib_LTLIBRARIES = libquicklauncher.la

libquicklauncher_la_SOURCES = quicklauncher.cpp quickbutton.cpp quickaddappsmenu.cpp

METASOURCES = AUTO
noinst_HEADERS = quicklauncher.h quickbutton.h quickaddappsmenu.h

lnkdir = $(kde_datadir)/kicker/applets
lnk_DATA = quicklauncher.desktop

EXTRA_DIST = $(lnk_DATA)

libquicklauncher_la_LDFLAGS = $(all_libraries) -version-info 1:0:0 -module 
libquicklauncher_la_LIBADD = ../../kicker.la $(LIB_KSYCOCA) $(LIB_KDEUI) 

messages:
	$(XGETTEXT) *.cpp *.h -o $(podir)/quicklauncher.pot

srcdoc:
	kdoc -a -p -H -d $(HOME)/web/src/quicklauncher quicklauncher *.h -lqt -lkdecore -lkdeui
