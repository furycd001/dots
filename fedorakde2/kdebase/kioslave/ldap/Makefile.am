## Makefile.am of kdebase/kioslave/ldap

INCLUDES=	$(all_includes) $(LDAP_INCS)
LDFLAGS =       $(all_libraries) $(KDE_RPATH) $(LDAP_RPATH)
LDADD	=	$(LIB_KIO) $(LDAP_LIBS)

####### Files

kde_module_LTLIBRARIES = kio_ldap.la

kio_ldap_la_SOURCES = kio_ldap.cpp kldap.cpp kldapurl.cpp
kio_ldap_la_LIBADD = $(LIB_KIO) $(LDAP_LIBS) $(KERBEROS_LIBS)
kio_ldap_la_LDFLAGS = $(all_libraries) $(KDE_RPATH) $(LDAP_RPATH) $(KRB_RPATH) -module $(KDE_PLUGIN)

noinst_HEADERS = kio_ldap.h kldap.h kldapurl.h

kdelnk_DATA = ldap.protocol
kdelnkdir = $(kde_servicesdir)

messages:
	$(XGETTEXT) *.cpp -o $(podir)/kio_ldap.pot
