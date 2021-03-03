INCLUDES = $(all_includes)

lib_LTLIBRARIES = libtaskmanager.la
libtaskmanager_la_SOURCES = taskmanager.cpp
libtaskmanager_la_METASOURCES = taskmanager.moc

libtaskmanager_la_LDFLAGS = $(all_libraries) -version-info 1:0:0 -module -no-undefined
libtaskmanager_la_LIBADD = $(LIB_KDECORE)

messages:
	$(XGETTEXT) *.cpp *.h -o $(podir)/libtaskmanager.pot
