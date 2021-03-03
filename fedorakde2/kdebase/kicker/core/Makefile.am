INCLUDES = -I$(srcdir)/../share -I$(srcdir)/../ui -I$(top_srcdir)/libkonq $(all_includes)

noinst_LTLIBRARIES = libkicker_core.la

libkicker_core_la_SOURCES = panelbutton.cpp main.cpp global.cpp panel.cpp panel.skel panner.cpp userrectsel.cpp \
	containerarea.cpp panelbuttonbase.cpp applethandle.cpp container_base.cpp container_panel.cpp \
	container_button.cpp container_applet.cpp containerareabox.cpp \
	container_extension.cpp extensionmanager.cpp

libkicker_core_la_LDFLAGS = $(all_libraries)
libkicker_core_la_LIBADD = $(top_builddir)/libkonq/libkonq.la $(LIB_KDEUI)
libkicker_core_la_METASOURCES = AUTO

data_DATA = default-apps
datadir = $(kde_datadir)/kicker
