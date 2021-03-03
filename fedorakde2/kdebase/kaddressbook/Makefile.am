KDE_ICON = kaddressbook

appsdir = $(kde_appsdir)/Utilities

INCLUDES= $(all_includes)

SUBDIRS = pics

SRCDOC_DEST=$(kde_htmldir)/en/kaddressbook

bin_PROGRAMS = kaddressbook

kaddressbook_SOURCES = kaddressbook.cpp kaddressbookiface.skel \
  undo.cpp attributes.cpp selectfields.cpp viewoptions.cpp namevaluewidget.cpp \
  contactentrylist.cpp browserentryeditor.cpp entryeditorwidget.cpp \
  datepickerdialog.cpp kaddressbookview.cpp main.cpp contactentry.cpp \
  undocmds.cpp contactlistview.cpp kimportdialog.cpp

kaddressbook_METASOURCES = AUTO

kaddressbook_LDFLAGS = $(all_libraries) $(KDE_RPATH)

kaddressbook_LDADD = -lkab $(LIB_KFILE)

noinst_HEADERS = kaddressbook.h undo.h attributes.h selectfields.h \
  viewoptions.h namevaluewidget.h contactentrylist.h browserentryeditor.h \
  entryeditorwidget.h datepickerdialog.h kaddressbookview.h undocmds.h \
  contactlistview.h kimportdialog.h

rcdir = $(kde_datadir)/kaddressbook
rc_DATA = kaddressbookui.rc

messages: rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/kaddressbook.pot

srcdoc:
	$(mkinstalldirs) $(SRCDOC_DEST)
	kdoc -H -d $(SRCDOC_DEST) kaddressbook $(noinst_HEADERS) -lqt

apps_DATA = kaddressbook.desktop

