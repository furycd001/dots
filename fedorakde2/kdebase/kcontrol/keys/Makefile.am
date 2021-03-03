kde_module_LTLIBRARIES = libkcm_keys.la

libkcm_keys_la_SOURCES =  keyconfig.cpp savescm.cpp main.cpp
libkcm_keys_la_LIBADD   = -lkdeui
libkcm_keys_la_LDFLAGS  = $(all_libraries) -module -avoid-version -no-undefined

INCLUDES= $(all_includes)

libkcm_keys_la_METASOURCES = AUTO

messages:
	customkeys=`grep "^.include .\.\." keyconfig.cpp | sed -e "s#.*\"\(.*\)\"#\1#"` ;\
	$(XGETTEXT) $(libkcm_keys_la_SOURCES) $$customkeys -o $(podir)/kcmkeys.pot    

data_DATA = keys.desktop
datadir = $(kde_appsdir)/Settings/LookNFeel

app_scheme_DATA = app-kde3.kksrc app-kde4.kksrc app-mac3.kksrc app-unix3.kksrc app-win3.kksrc
app_schemedir = $(kde_datadir)/kcmkeys/standard

global_scheme_DATA = global-kde3.kksrc global-kde4.kksrc global-mac3.kksrc global-unix3.kksrc global-win3.kksrc global-win4.kksrc
global_schemedir = $(kde_datadir)/kcmkeys/global
