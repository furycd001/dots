INCLUDES = $(all_includes) $(KDE_INCLUDES)/kdevelop -I$(top_srcdir)/kate
METASOURCES = AUTO

kde_module_LTLIBRARIES = libkateeditorpart.la

libkateeditorpart_la_SOURCES = kate_part.cpp kate_factory.cpp document_impl.cpp clipboard_iface_impl.cpp cursor_iface_impl.cpp undo_iface_impl.cpp edit_iface_impl.cpp status_iface_impl.cpp debug_iface_impl.cpp

libkateeditorpart_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries) 
libkateeditorpart_la_LIBADD  = $(LIB_KPARTS) $(LIB_KFILE) -lktexteditor -lkspell -lkeditorinterface $(top_builddir)/kate/main/libkatecore.la

partdesktopdir   = $(kde_servicesdir)
partdesktop_DATA = kate_part.desktop

partrcdir   = $(kde_datadir)/kate_part
partrc_DATA = kate_part.rc
