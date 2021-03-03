INCLUDES = -I$(srcdir)/../core -I$(srcdir)/../share -I$(top_srcdir)/libkonq $(all_includes)

noinst_LTLIBRARIES = libkicker_ui.la

#libkicker_ui_la_SOURCES = addbutton_mnu.cpp addapplet_mnu.cpp appletop_mnu.cpp buttonop_mnu.cpp base_mnu.cpp 
libkicker_ui_la_SOURCES = addbutton_mnu.cpp addapplet_mnu.cpp appletop_mnu.cpp base_mnu.cpp \
			browser_mnu.cpp client_mnu.cpp dirdrop_mnu.cpp exe_dlg.cpp k_mnu.cpp \
			panelop_mnu.cpp quickbrowser_mnu.cpp recent_mnu.cpp service_mnu.cpp \
			addextension_mnu.cpp extensionop_mnu.cpp konsole_mnu.cpp \
			addcontainer_mnu.cpp recentapps.cpp browser_dlg.cpp

libkicker_ui_la_LIBADD = $(top_builddir)/libkonq/libkonq.la

libkicker_ui_la_METASOURCES = AUTO

#noinst_HEADERS = addbutton_mnu.h addapplet_mnu.h appletop_mnu.h buttonop_mnu.h base_mnu.h 
noinst_HEADERS = addbutton_mnu.h addapplet_mnu.h appletop_mnu.h base_mnu.h \
		browser_mnu.h client_mnu.h dirdrop_mnu.h exe_dlg.h k_mnu.h \
		panelop_mnu.h quickbrowser_mnu.h recent_mnu.h service_mnu.h \
		addextension_mnu.h extensionop_mnu.h konsole_mnu.h \
		addcontainer_mnu.h recentapps.h browser_dlg.h
