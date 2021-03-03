INCLUDES= $(all_includes)
METASOURCES = AUTO

bin_PROGRAMS = kxkb

kxkb_SOURCES = dummy.cpp
kxkb_LDFLAGS = $(all_libraries) $(KDE_RPATH)
kxkb_LDADD = kxkb.la

lib_LTLIBRARIES = libkcm_keyboard.la kxkb.la
 
libkcm_keyboard_la_SOURCES = rules.cpp kcmlayout.cpp pixmap.cpp kcmmisc.cpp
 
libkcm_keyboard_la_LDFLAGS  = $(all_libraries) -module -avoid-version -no-undefined
libkcm_keyboard_la_LIBADD = @XTESTLIB@ $(LIB_KFILE)

kxkb_la_SOURCES = rules.cpp kxkb.cpp extension.cpp pixmap.cpp
kxkb_la_LDFLAGS = $(all_libraries) -module -avoid-version
kxkb_la_LIBADD = -lX11 -lXext $(LIB_KDEUI)
 
data_DATA = keyboard.desktop
datadir = $(kde_appsdir)/Settings/Peripherals

install-data-local: uninstall.desktop
	$(mkinstalldirs) $(kde_appsdir)/Settings/Personalization
	$(INSTALL_DATA) $(srcdir)/uninstall.desktop $(kde_appsdir)/Settings/Personalization/kcmlayout.desktop

servicesdir = $(kde_servicesdir)
services_DATA = kxkb.desktop
 
messages: 
	$(XGETTEXT) $(kxkb_la_SOURCES) kxkbbindings.cpp -o $(podir)/kxkb.pot
	$(XGETTEXT) $(libkcm_keyboard_la_SOURCES) kxkbbindings.cpp -o $(podir)/kcmlayout.pot 

dummy.cpp:
	echo > dummy.cpp
