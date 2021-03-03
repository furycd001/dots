INCLUDES = $(all_includes)

template_DATA = template.css
templatedir = $(kde_datadir)/kcmcss

kde_module_LTLIBRARIES = libkcm_css.la
libkcm_css_la_SOURCES = template.cpp cssconfig.ui kcmcss.cpp preview.ui
libkcm_css_la_LDFLAGS  = $(all_libraries) -module -avoid-version -no-undefined
libkcm_css_la_LIBADD = -lkdeui -lkfile

libkcm_css_la_METASOURCES = AUTO
 
messages: rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/kcmcss.pot
 
data_DATA = kcmcss.desktop
datadir = $(kde_appsdir)/Settings/WebBrowsing
