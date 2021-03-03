kde_module_LTLIBRARIES = libkdeprint_part.la

libkdeprint_part_la_SOURCES = printpartfactory.cpp printpart.cpp
libkdeprint_part_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined
libkdeprint_part_la_LIBADD = -lkdeprint_management $(LIB_KPARTS)

noinst_HEADERS = printpartfactory.h printpart.h

METASOURCES = AUTO
INCLUDES = $(all_includes)

# service
myservice_DATA = kdeprint_part.desktop
myservicedir = $(kde_servicesdir)

# UI file
myrcfile_DATA = kdeprint_part.rc
myrcfiledir = $(kde_datadir)/kdeprint_part
