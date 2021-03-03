lib_LTLIBRARIES = libkatecore.la kate.la kwrite.la
bin_PROGRAMS = kate kwrite

libkatecore_la_SOURCES = dummy.cpp
libkatecore_la_LIBADD   = ../qt3back/libqregexp3.la \
		../factory/libkatefactory.la ../document/libkatedocument.la ../view/libkateview.la ../app/libkateapp.la \
		../mainwindow/libkatemainwindow.la \
		../fileselector/libkatefileselector.la  \
		../console/libkateconsole.la ../filelist/libkatefilelist.la \
		../pluginmanager/libkatepluginmanager.la ../interfaces/libkateinterfaces.la -lktexteditor -lkspell -lkdeprint
libkatecore_la_LDFLAGS = $(all_libraries) $(KDE_RPATH)  $(KDE_PLUGIN)  -module -avoid-version

kate_la_SOURCES = katemain.cpp
kate_la_LIBADD  = libkatecore.la
kate_la_LDFLAGS = $(all_libraries) $(KDE_RPATH) -module -avoid-version

kwrite_la_SOURCES = kwritemain.cpp
kwrite_la_LIBADD   = libkatecore.la
kwrite_la_LDFLAGS = $(all_libraries) $(KDE_RPATH)  -module -avoid-version

kate_SOURCES = dummy.cpp
kate_LDADD = kate.la
kate_LDFLAGS = $(KDE_RPATH) $(all_libraries)

kwrite_SOURCES = dummy.cpp
kwrite_LDADD = kwrite.la
kwrite_LDFLAGS = $(KDE_RPATH) $(all_libraries)

dummy.cpp:
	echo > dummy.cpp

INCLUDES= $(all_includes)

METASOURCES = AUTO
