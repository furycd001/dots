SUBDIRS= pics

kde_module_LTLIBRARIES = libkcm_input.la

libkcm_input_la_SOURCES = main.cpp mouse.cpp kmousedlg.ui

libkcm_input_la_LDFLAGS  = $(all_libraries) -module -avoid-version -no-undefined
libkcm_input_la_LIBADD = @XTESTLIB@ $(LIB_KIO) $(LIB_KDEUI)

INCLUDES= $(all_includes)

libkcm_input_la_METASOURCES = AUTO

messages: rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/kcminput.pot

data_DATA = mouse.desktop
datadir = $(kde_appsdir)/Settings/Peripherals

fontdir = $(kde_datadir)/kcminput
font_DATA = cursor_large.pcf.gz
EXTRA_DIST = $(font_DATA)

install-data-hook:
	$(mkinstalldirs) $(DESTDIR)$(prefix)/share/fonts/override
	-mkfontdir $(DESTDIR)$(prefix)/share/fonts/override
