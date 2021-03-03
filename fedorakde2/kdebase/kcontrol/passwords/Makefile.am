## Makefile.am for kcmpasswords

INCLUDES= $(all_includes)

kde_module_LTLIBRARIES = libkcm_passwords.la
libkcm_passwords_la_SOURCES = passwords.cpp
libkcm_passwords_la_LDFLAGS = -module -avoid-version $(all_libraries) -no-undefined
libkcm_passwords_la_LIBADD = $(LIB_KDEUI) -lkdesu

METASOURCES = AUTO
noinst_HEADERS = passwords.h

messages:
	$(XGETTEXT) $(libkcm_passwords_la_SOURCES) -o $(podir)/passwords.pot

data_DATA = passwords.desktop
datadir = $(kde_appsdir)/Settings/Personalization
