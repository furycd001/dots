kde_module_LTLIBRARIES        = libkcm_launch.la
INCLUDES                      = $(all_includes) 
libkcm_launch_la_SOURCES      = kcmlaunch.cpp
libkcm_launch_la_LDFLAGS      = $(all_libraries) $(KDE_PLUGIN)
libkcm_launch_la_LIBADD       = $(LIB_KDEUI)
libkcm_launch_la_METASOURCES  = AUTO
noinst_HEADERS                = kcmlaunch.h
data_DATA                     = kcmlaunch.desktop
datadir                       = $(kde_appsdir)/Settings/LookNFeel/
EXTRA_DIST                    = $(data_DATA)

messages: rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/kcmlaunch.pot

