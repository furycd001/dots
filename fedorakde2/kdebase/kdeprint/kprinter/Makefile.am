INCLUDES = $(all_includes)

bin_PROGRAMS = kprinter

kprinter_SOURCES = main.cpp printwrapper.cpp
kprinter_LDADD = -lkdeprint
kprinter_LDFLAGS = $(all_libraries) $(KDE_RPATH)
kprinter_METASOURCES = AUTO

noinst_HEADERS = printwrapper.h

messages:
	$(XGETTEXT) *.cpp -o $(podir)/kprinter.pot
