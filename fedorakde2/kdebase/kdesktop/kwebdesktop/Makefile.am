AM_CPPFLAGS = -DQT_NO_COMPAT -DQT_NO_ASCII_CAST

INCLUDES= $(all_includes)
LDADD = $(LIB_KHTML)

bin_PROGRAMS = 	kwebdesktop

METASOURCES = AUTO

kwebdesktop_SOURCES = kwebdesktop.cpp 
kwebdesktop_LDFLAGS = $(all_libraries) $(KDE_RPATH)

data_DATA = kwebdesktop.desktop
datadir = $(kde_datadir)/kdesktop/programs

