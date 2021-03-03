# backend needs to be conditional; chooser will go away.
SUBDIRS = backend kfrontend @kdmchooserdir@

EXTRA_DIST = misc

PAM = @KDM_PAM_SERVICE@

install-data-local:
	@test -n "$(DESTDIR)" || test -z "$(PAM)" || $(top_srcdir)/mkpamserv $(PAM)
