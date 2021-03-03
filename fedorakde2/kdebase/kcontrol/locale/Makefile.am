AM_CPPFLAGS = -DQT_NO_CAST_ASCII -DQT_NO_COMPAT
INCLUDES = $(all_includes)

SUBDIRS = default pics

kde_module_LTLIBRARIES = libkcm_locale.la
libkcm_locale_la_SOURCES = klocaleadv.cpp main.cpp toplevel.cpp locale.cpp klanguagebutton.cpp klocalesample.cpp localenum.cpp localemon.cpp localetime.cpp 

METASOURCES = AUTO
noinst_HEADERS = klocaleadv.h toplevel.h locale.h klanguagebutton.h klocalesample.h localenum.h localemon.h localetime.h menuitem.h

libkcm_locale_la_LDFLAGS = -module -avoid-version $(all_libraries) -no-undefined
libkcm_locale_la_LIBADD = $(LIB_KDEUI)
 
messages:
	$(XGETTEXT) $(libkcm_locale_la_SOURCES) -o $(podir)/kcmlocale.pot

data_DATA = language.desktop
datadir = $(kde_appsdir)/Settings/Personalization

EXTRA_DIST = $(data_DATA)
