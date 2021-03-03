pic_DATA = naughty-happy.png naughty-sad.png
picdir = $(kde_datadir)/naughtyapplet/pics

INCLUDES = $(all_includes)

lib_LTLIBRARIES = libnaughtyapplet.la

libnaughtyapplet_la_SOURCES = \
  NaughtyProcessMonitor.cpp \
  NaughtyConfigDialog.cpp \
  NaughtyApplet.cpp

METASOURCES = AUTO

noinst_HEADERS = \
  NaughtyProcessMonitor.h \
  NaughtyConfigDialog.h \
  NaughtyApplet.h

lnkdir = $(kde_datadir)/kicker/applets
lnk_DATA = naughtyapplet.desktop

EXTRA_DIST = $(lnk_DATA)

libnaughtyapplet_la_LDFLAGS = $(all_libraries) -version-info 1:0:0 -module -no-undefined
libnaughtyapplet_la_LIBADD = $(LIB_KDEUI)

messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -o $(podir)/naughtyapplet.pot

