INCLUDES = -I$(srcdir)/../taskmanager $(all_includes)

lib_LTLIBRARIES = libtaskbar.la
libtaskbar_la_SOURCES = lmbmenu.cpp rmbmenu.cpp taskcontainer.cpp taskbar.cpp fittslawscrollview.cpp scrollbutton.cpp windowlistbutton.cpp 
libtaskbar_la_METASOURCES = AUTO

libtaskbar_la_LDFLAGS = $(all_libraries) -version-info 1:0:0 -module -no-undefined
libtaskbar_la_LIBADD = $(LIB_KDEUI) ../taskmanager/libtaskmanager.la

messages:
	$(XGETTEXT) *.cpp *.h -o $(podir)/libtaskbar.pot
