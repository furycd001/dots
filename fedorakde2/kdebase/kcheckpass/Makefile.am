## Makefile.am for kcheckpass
## written by Christian Esken
##

INCLUDES= $(all_includes)
LDADD	= $(all_libraries) -lkdefakes $(PASSWDLIBS) $(LIBSOCKET)

# for shadow passwords override the value
INSTALL_PROGRAM = $(INSTALL) $(SETUIDFLAGS)

bin_PROGRAMS = kcheckpass

kcheckpass_SOURCES = kcheckpass.c \
	checkpass_etcpasswd.c checkpass_pam.c checkpass_shadow.c \
	checkpass_osfc2passwd.c checkpass_aix.c

noinst_HEADERS = kcheckpass.h

EXTRA_DIST = README

PAM = @KCHECKPASS_PAM_SERVICE@

install-data-local:
	@test -n "$(DESTDIR)" || test -z "$(PAM)" || $(top_srcdir)/mkpamserv $(PAM)
