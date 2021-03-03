INCLUDES = $(all_includes)

kde_module_LTLIBRARIES = libkcm_printmgr.la

libkcm_printmgr_la_SOURCES = kcmprintmgr.cpp
libkcm_printmgr_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined
libkcm_printmgr_la_LIBADD = -lkdeprint_management

noinst_HEADERS = kcmprintmgr.h

KDE_ICON = printmgr

data_DATA = printmgr.desktop
datadir = $(kde_appsdir)/Settings/System
