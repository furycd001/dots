#SUBDIRS = Applications Games Graphics Internet Multimedia Utilities \
#	System Settings ScreenSavers Control

#data_DATA = .directory
#datadir = $(kde_appsdir)

#EXTRA_DIST = $(data_DATA)


install-data-local:
	$(mkinstalldirs) $(kde_appsdir)
	$(INSTALL_DATA) $(srcdir)/.directory $(kde_appsdir)/.directory

	$(mkinstalldirs) $(kde_appsdir)/Applications
	$(INSTALL_DATA) $(srcdir)/applications.desktop $(kde_appsdir)/Applications/.directory
	$(mkinstalldirs) $(kde_appsdir)/Development
	$(INSTALL_DATA) $(srcdir)/development.desktop $(kde_appsdir)/Development/.directory

	$(mkinstalldirs) $(kde_appsdir)/Games
	$(INSTALL_DATA) $(srcdir)/games.desktop $(kde_appsdir)/Games/.directory
	$(mkinstalldirs) $(kde_appsdir)/Games/Arcade
	$(INSTALL_DATA) $(srcdir)/games-arcade.desktop $(kde_appsdir)/Games/Arcade/.directory
	$(mkinstalldirs) $(kde_appsdir)/Games/Board
	$(INSTALL_DATA) $(srcdir)/games-board.desktop $(kde_appsdir)/Games/Board/.directory
	$(mkinstalldirs) $(kde_appsdir)/Games/Card
	$(INSTALL_DATA) $(srcdir)/games-card.desktop $(kde_appsdir)/Games/Card/.directory
	$(mkinstalldirs) $(kde_appsdir)/Games/TacticStrategy/
	$(INSTALL_DATA) $(srcdir)/games-tacticstrategy.desktop $(kde_appsdir)/Games/TacticStrategy/.directory

	$(mkinstalldirs) $(kde_appsdir)/Graphics
	$(INSTALL_DATA) $(srcdir)/graphics.desktop $(kde_appsdir)/Graphics/.directory
	$(mkinstalldirs) $(kde_appsdir)/Multimedia
	$(INSTALL_DATA) $(srcdir)/multimedia.desktop $(kde_appsdir)/Multimedia/.directory

	$(mkinstalldirs) $(kde_appsdir)/System
	$(INSTALL_DATA) $(srcdir)/system.desktop $(kde_appsdir)/System/.directory
	$(mkinstalldirs) $(kde_appsdir)/System/ScreenSavers
	$(INSTALL_DATA) $(srcdir)/system-screensavers.desktop $(kde_appsdir)/System/ScreenSavers/.directory
	$(mkinstalldirs) $(kde_appsdir)/System/Terminal
	$(INSTALL_DATA) $(srcdir)/system-terminal.desktop $(kde_appsdir)/System/Terminal/.directory

	$(mkinstalldirs) $(kde_appsdir)/Internet
	$(INSTALL_DATA) $(srcdir)/internet.desktop $(kde_appsdir)/Internet/.directory
	$(mkinstalldirs) $(kde_appsdir)/Internet/Terminal
	$(INSTALL_DATA) $(srcdir)/internet-terminal.desktop $(kde_appsdir)/Internet/Terminal/.directory

	$(mkinstalldirs) $(kde_appsdir)/Utilities
	$(INSTALL_DATA) $(srcdir)/utilities.desktop $(kde_appsdir)/Utilities/.directory
	$(mkinstalldirs) $(kde_appsdir)/Office
	$(INSTALL_DATA) $(srcdir)/office.desktop $(kde_appsdir)/Office/.directory
	$(mkinstalldirs) $(kde_appsdir)/Editors
	$(INSTALL_DATA) $(srcdir)/editors.desktop $(kde_appsdir)/Editors/.directory
	$(mkinstalldirs) $(kde_appsdir)/Toys
	$(INSTALL_DATA) $(srcdir)/toys.desktop $(kde_appsdir)/Toys/.directory

	$(mkinstalldirs) $(kde_appsdir)/Settings
	$(INSTALL_DATA) $(srcdir)/settings.desktop $(kde_appsdir)/Settings/.directory
	$(mkinstalldirs) $(kde_appsdir)/Settings/LookNFeel
	$(INSTALL_DATA) $(srcdir)/settings-looknfeel.desktop $(kde_appsdir)/Settings/LookNFeel/.directory
	$(mkinstalldirs) $(kde_appsdir)/Settings/Information
	$(INSTALL_DATA) $(srcdir)/settings-information.desktop $(kde_appsdir)/Settings/Information/.directory
	$(mkinstalldirs) $(kde_appsdir)/Settings/Sound
	$(INSTALL_DATA) $(srcdir)/settings-sound.desktop $(kde_appsdir)/Settings/Sound/.directory
	$(mkinstalldirs) $(kde_appsdir)/Settings/Peripherals
	$(INSTALL_DATA) $(srcdir)/settings-peripherals.desktop $(kde_appsdir)/Settings/Peripherals/.directory
	$(mkinstalldirs) $(kde_appsdir)/Settings/FileBrowsing
	$(INSTALL_DATA) $(srcdir)/settings-filebrowsing.desktop $(kde_appsdir)/Settings/FileBrowsing/.directory
	$(mkinstalldirs) $(kde_appsdir)/Settings/WebBrowsing
	$(INSTALL_DATA) $(srcdir)/settings-webbrowsing.desktop $(kde_appsdir)/Settings/WebBrowsing/.directory
	$(mkinstalldirs) $(kde_appsdir)/Settings/PowerControl
	$(INSTALL_DATA) $(srcdir)/settings-power.desktop $(kde_appsdir)/Settings/PowerControl/.directory
	$(mkinstalldirs) $(kde_appsdir)/Settings/Personalization
	$(INSTALL_DATA) $(srcdir)/settings-person.desktop $(kde_appsdir)/Settings/Personalization/.directory
	$(mkinstalldirs) $(kde_appsdir)/Settings/Help
	$(INSTALL_DATA) $(srcdir)/settings-help.desktop $(kde_appsdir)/Settings/Help/.directory
	$(mkinstalldirs) $(kde_appsdir)/Settings/Network
	$(INSTALL_DATA) $(srcdir)/settings-network.desktop $(kde_appsdir)/Settings/Network/.directory
	$(mkinstalldirs) $(kde_appsdir)/Settings/System
	$(INSTALL_DATA) $(srcdir)/settings-system.desktop $(kde_appsdir)/Settings/System/.directory
	$(mkinstalldirs) $(kde_appsdir)/Settings/Databases
	$(INSTALL_DATA) $(srcdir)/settings-databases.desktop $(kde_appsdir)/Settings/Databases/.directory
	$(mkinstalldirs) $(kde_appsdir)/WordProcessing
	$(INSTALL_DATA) $(srcdir)/wordprocessing.desktop $(kde_appsdir)/WordProcessing/.directory

uninstall-local:
	-rm -f $(kde_appsdir)/.directory

	-rm -f $(kde_appsdir)/Applicatications/.directory

	-rm -f $(kde_appsdir)/Games/.directory
	-rm -f $(kde_appsdir)/Games/Arcade/.directory
	-rm -f $(kde_appsdir)/Games/Board/.directory
	-rm -f $(kde_appsdir)/Games/Card/.directory
	-rm -f $(kde_appsdir)/Games/TacticStrategy/.directory

	-rm -f $(kde_appsdir)/Graphics/.directory
	-rm -f $(kde_appsdir)/Multimedia/.directory
	-rm -f $(kde_appsdir)/WordProcessing/.directory

	-rm -f $(kde_appsdir)/System/.directory
	-rm -f $(kde_appsdir)/System/ScreenSavers/.directory
	-rm -f $(kde_appsdir)/System/Terminal/.directory

	-rm -f $(kde_appsdir)/Internet/.directory
	-rm -f $(kde_appsdir)/Internet/Terminal/.directory

	-rm -f $(kde_appsdir)/Utilities/.directory
	-rm -f $(kde_appsdir)/Office/.directory

	-rm -f $(kde_appsdir)/Settings/.directory
	-rm -f $(kde_appsdir)/Settings/LookNFeel/.directory
	-rm -f $(kde_appsdir)/Settings/Information/.directory
	-rm -f $(kde_appsdir)/Settings/Sound/.directory
	-rm -f $(kde_appsdir)/Settings/Peripherals/.directory
	-rm -f $(kde_appsdir)/Settings/Help/.directory
	-rm -f $(kde_appsdir)/Settings/FileBrowsing/.directory
	-rm -f $(kde_appsdir)/Settings/Network/.directory
	-rm -f $(kde_appsdir)/Settings/WebBrowsing/.directory
	-rm -f $(kde_appsdir)/Settings/PowerControl/.directory
	-rm -f $(kde_appsdir)/Settings/Personalization/.directory
	-rm -f $(kde_appsdir)/Settings/Databases/.directory
