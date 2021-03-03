INCLUDES = -I$(srcdir)/../../taskmanager $(all_includes)

lib_LTLIBRARIES = libkasbarextension.la

libkasbarextension_la_SOURCES = kasbar.cpp kasitem.cpp kasbarextension.cpp \
				kastaskitem.cpp	kastasker.cpp kaspopup.cpp \
				kastaskpopup.cpp kasstartupitem.cpp \
				kasaboutdlg.cpp kasprefsdlg.cpp \
				kasgroupitem.cpp kasgrouppopup.cpp

libkasbarextension_la_METASOURCES = AUTO
libkasbarextension_la_LDFLAGS = $(all_libraries) -version-info 1:0:0 -module -no-undefined
libkasbarextension_la_LIBADD =  $(LIB_KDEUI) ../../taskmanager/libtaskmanager.la

noinst_HEADERS = kasbar.h kasitem.h kasbarextension.h \
		kastaskitem.h kastasker.h kaspopup.h kasstartupitem.h \
		kasaboutdlg.h kasprefsdlg.h version.h \
		kasgroupitem.h kasgrouppopup.h

lnkdir = $(kde_datadir)/kicker/extensions
lnk_DATA = kasbarextension.desktop

EXTRA_DIST = $(lnk_DATA)

messages:
	$(XGETTEXT) *.cpp *.h -o $(podir)/kasbarextension.pot

srcdoc:
	$(kde_bindir)/kdoc -a -p -d classdocs -n 'Kasbar2' *.h -lqt -lkdecore -lkdeui
