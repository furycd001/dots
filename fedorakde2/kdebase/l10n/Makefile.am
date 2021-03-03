SUBDIRS=C

regionsdir = $(kde_locale)/l10n
regions_DATA = africa.desktop asia.desktop caribbean.desktop easteurope.desktop northamerica.desktop southamerica.desktop westeurope.desktop centraleurope.desktop northeurope.desktop southeurope.desktop

EXTRA_DIST = README

install-data-local:
	@list=`(cd $(srcdir) && ls -1d * | grep -v CVS)` ;\
	for i in $$list; do if test -d $(srcdir)/$$i; then \
	  echo $(mkinstalldirs) $(DESTDIR)$(kde_locale)/l10n/$$i ;\
	  $(mkinstalldirs) $(DESTDIR)$(kde_locale)/l10n/$$i; \
	  echo $(INSTALL_DATA) $(srcdir)/$$i/entry.desktop $(DESTDIR)$(kde_locale)/l10n/$$i ;\
	  $(INSTALL_DATA) $(srcdir)/$$i/entry.desktop $(DESTDIR)$(kde_locale)/l10n/$$i ;\
	  echo $(INSTALL_DATA) $(srcdir)/$$i/flag.png $(DESTDIR)$(kde_locale)/l10n/$$i ;\
	  $(INSTALL_DATA) $(srcdir)/$$i/flag.png $(DESTDIR)$(kde_locale)/l10n/$$i ;\
	fi; done
