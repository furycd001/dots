/***************************************************************************
                          kospage.cpp  -  description
                             -------------------
    begin                : Tue May 22 2001
    copyright            : (C) 2001 by Ralf Nolden
    email                : nolden@kde.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qlabel.h>
#include <qradiobutton.h>
#include <qtextview.h>

#include <kconfig.h>
#include <ksimpleconfig.h>
#include <kstddirs.h>
#include <kapp.h>
#include <kglobal.h>
#include <klocale.h>
#include <dcopclient.h>
#include <kipc.h>

#include <kaccel.h>

#include <kdebug.h>

#include "kospage.h"

KOSPage::KOSPage(QWidget *parent, const char *name ) : KOSPageDlg(parent,name) {
	px_osSidebar->setPixmap(locate("data", "kpersonalizer/pics/step2.png"));
	// initialize the textview with the default description - KDE of course
	slotKDEDescription();
	// Set the configfiles
	cglobal = new KConfig("kdeglobals");
	claunch = new KConfig("klaunchrc", false, false);
	cwin = new KConfig("kwinrc");
	cdesktop = new KConfig("kdesktoprc");
	ckcminput = new KConfig("kcminputrc");
	ckcmdisplay = new KConfig("kcmdisplayrc");
	ckonqueror = new KConfig("konquerorrc");
	appkeyfile = "<default>";
	globalkeyfile="<default>";
	getUserDefaults(); // Save the current user defaults
	setDefaults();
}

KOSPage::~KOSPage(){
}


void KOSPage::save(bool currSettings){
    kdDebug() << "KOSPage::save()" << endl;
	// save like we want. Just set the Radiobutton to either how it is set in the dialog (currSettings=true, default)
	// or, if false, take the settings we got in getUserDefaults()
	saveCheckState(currSettings);
	// sync all configs
	cglobal->sync();
	claunch->sync();
	cwin->sync();
	cdesktop->sync();
	ckcmdisplay->sync();
///////////////////////////////////////////
// kcmdisplay changes
	KIPC::sendMessageAll(KIPC::SettingsChanged);
	QApplication::syncX();
	// enable/disable the mac menu, call dcop
	// Tell kdesktop about the new config file
	kapp->dcopClient()->send("kdesktop", "KDesktopIface", "configure()", QByteArray());
///////////////////////////////////////////
/// restart kwin  for window effects
	kapp->dcopClient()->send("kwin*", "", "reconfigure()", "");
}


	/** called by save() -- currSettings= true -> take the radiobutton, otherwise take user values set */
void KOSPage::saveCheckState(bool currSettings){
	if(currSettings){
		// Set the path for the keysscheme resource files
		KGlobal::dirs()->addResourceType("appkeys", KStandardDirs::kde_default("data")+"kcmkeys/standard");
		KGlobal::dirs()->addResourceType("globalkeys", KStandardDirs::kde_default("data")+"kcmkeys/global");
		// write the settings to the configfiles, depending on wich radiobutton is checked
		if(rb_kde->isChecked()){
			writeKDE();
			emit selectedOS("KDE");	// send a signal to be catched by the KStylePage to set the according style by default depending on the OS selection
		}
		else if(rb_unix->isChecked()){
			writeUNIX();
			emit selectedOS("CDE");	// send a signal to be catched by the KStylePage to set the according style by default depending on the OS selection
		}
		else if(rb_windows->isChecked()){
			writeWindows();
			emit selectedOS("win");	// send a signal to be catched by the KStylePage to set the according style by default depending on the OS selection
		}
		else if(rb_mac->isChecked()){
			writeMacOS();
			emit selectedOS("mac");	// send a signal to be catched by the KStylePage to set the according style by default depending on the OS selection
		}
		// write the keyentries
		writeAppKeyEntrys();
		writeGlobalKeyEntrys();
	}
	else {  // User has pressed "cancel & dismiss", so his old settings are written back
		writeUserDefaults();
	}
}


	/** write the settings for KDE-Behavior (called by saveCheckStatus) */
void KOSPage::writeKDE(){
	kdDebug() << "KOSPage::writeKDE()" << endl;

	ckcmdisplay->setGroup("KDE");
	ckcmdisplay->writeEntry("macStyle", false, true, true);

	cglobal->setGroup("KDE");
	cglobal->writeEntry("SingleClick", true, true, true);

	claunch->setGroup("FeedbackStyle");
	claunch->writeEntry("BusyCursor", true);

	cwin->setGroup("Style");
	cwin->writeEntry("PluginLib", "libkwindefault");
	cwin->setGroup("Windows");
	cwin->writeEntry("TitlebarDoubleClickCommand", "Shade");
	cwin->writeEntry("FocusPolicy", "ClickToFocus");
	cwin->writeEntry("AltTabStyle", "KDE");
	cwin->setGroup( "MouseBindings");
	cwin->writeEntry("CommandActiveTitlebar2", "Lower");
	cwin->writeEntry("CommandActiveTitlebar3", "Operations menu");

	cdesktop->setGroup( "Menubar" );
	cdesktop->writeEntry("ShowMenubar", false);
	cdesktop->setGroup( "Mouse Buttons" );
	cdesktop->writeEntry("Middle", "WindowListMenu");
	cdesktop->setGroup( "FMSettings" );
	cdesktop->writeEntry("UnderlineLinks", true);

	ckonqueror->setGroup( "FMSettings" );
	ckonqueror->writeEntry("UnderlineLinks", true);

	ckcminput->setGroup("KDE");
	ckcminput->writeEntry("ChangeCursor", true, true, true );

	appkeyfile=locate("appkeys", "app-kde3.kksrc");
	globalkeyfile=locate("globalkeys", "global-kde3.kksrc");
}


	/** write the settings for fvwm-like-behavior (called by saveCheckStatus) */
void KOSPage::writeUNIX(){
	kdDebug() << "KOSPage::writeUNIX()" << endl;

	ckcmdisplay->setGroup("KDE");
	ckcmdisplay->writeEntry("macStyle", false, true, true);

	cglobal->setGroup("KDE");
	cglobal->writeEntry("SingleClick", true, true, true);

	claunch->setGroup("FeedbackStyle");
	claunch->writeEntry("BusyCursor", false);

	cwin->setGroup("Style");
	cwin->writeEntry("PluginLib", "libkwinmwm");
	cwin->setGroup("Windows");
	cwin->writeEntry("TitlebarDoubleClickCommand", "Maximise (vertical only)");
	cwin->writeEntry("FocusPolicy", "FocusStrictlyUnderMouse");
	cwin->writeEntry("AltTabStyle", "CDE");
	cwin->setGroup( "MouseBindings");
	cwin->writeEntry("CommandActiveTitlebar2", "Operations menu");
	cwin->writeEntry("CommandActiveTitlebar3", "Lower");

	cdesktop->setGroup( "Menubar" );
	cdesktop->writeEntry("ShowMenubar", false);
	cdesktop->setGroup( "Mouse Buttons" );
	cdesktop->writeEntry("Middle", "AppMenu");
	cdesktop->setGroup( "FMSettings" );
	cdesktop->writeEntry("UnderlineLinks", false);

	ckonqueror->setGroup( "FMSettings" );
	ckonqueror->writeEntry("UnderlineLinks", false);

	ckcminput->setGroup("KDE");
	ckcminput->writeEntry("ChangeCursor", false, true, true );

	appkeyfile=locate("appkeys", "app-unix3.kksrc");
	globalkeyfile=locate("globalkeys", "global-unix3.kksrc");
}


	/** write the settings for windows-like-behavior (called by saveCheckStatus) */
void KOSPage::writeWindows(){
	kdDebug() << "KOSPage::writeWindows()" << endl;

	ckcmdisplay->setGroup("KDE");
	ckcmdisplay->writeEntry("macStyle", false, true, true);

	cglobal->setGroup("KDE");
	cglobal->writeEntry("SingleClick", false, true, true);

	claunch->setGroup("FeedbackStyle");
	claunch->writeEntry("BusyCursor", true);

	cwin->setGroup("Style");
	cwin->writeEntry("PluginLib", "libkwinredmond");
	cwin->setGroup("Windows");
	cwin->writeEntry("TitlebarDoubleClickCommand", "Maximize");
	cwin->writeEntry("FocusPolicy", "ClickToFocus");
	cwin->writeEntry("AltTabStyle", "KDE");
	cwin->setGroup( "MouseBindings");
	cwin->writeEntry("CommandActiveTitlebar2", "Lower");
	cwin->writeEntry("CommandActiveTitlebar3", "Operations menu");

	cdesktop->setGroup( "Menubar" );
	cdesktop->writeEntry("ShowMenubar", false);
	cdesktop->setGroup( "Mouse Buttons" );
	cdesktop->writeEntry("Middle", "WindowListMenu");
	cdesktop->setGroup( "FMSettings" );
	cdesktop->writeEntry("UnderlineLinks", false);

	ckonqueror->setGroup( "FMSettings" );
	ckonqueror->writeEntry("UnderlineLinks", false);

	ckcminput->setGroup("KDE");
	ckcminput->writeEntry("ChangeCursor", false, true, true );

	appkeyfile=locate("appkeys", "app-win3.kksrc");
  // set the global-keys schemefile depending on whether or not the keyboard has got Metakeys.
	if(KAccel::keyboardHasMetaKey()) { globalkeyfile=locate("globalkeys", "global-win4.kksrc"); }
	else { globalkeyfile=locate("globalkeys", "global-win3.kksrc"); }
}


	/** write the settings for MacOS-like-behavior (called by saveCheckStatus) */
void KOSPage::writeMacOS(){
	kdDebug() << "KOSPage::writeMacOS()" << endl;

	ckcmdisplay->setGroup("KDE");
	ckcmdisplay->writeEntry("macStyle", true, true, true);

	cglobal->setGroup("KDE");
	cglobal->writeEntry("SingleClick", true, true, true);

	claunch->setGroup("FeedbackStyle");
	claunch->writeEntry("BusyCursor", true);

	cwin->setGroup("Style");
	cwin->writeEntry("PluginLib", "libkwinmodernsys");
	cwin->setGroup("Windows");
	cwin->writeEntry("TitlebarDoubleClickCommand", "Shade");
	cwin->writeEntry("FocusPolicy", "ClickToFocus");
	cwin->writeEntry("AltTabStyle", "KDE");
	cwin->setGroup( "MouseBindings");
	cwin->writeEntry("CommandActiveTitlebar2", "Lower");
	cwin->writeEntry("CommandActiveTitlebar3", "Operations menu");

	cdesktop->setGroup( "Menubar" );
	cdesktop->writeEntry("ShowMenubar", true);
	cdesktop->setGroup( "Mouse Buttons" );
	cdesktop->writeEntry("Middle", "WindowListMenu");
	cdesktop->setGroup( "FMSettings" );
	cdesktop->writeEntry("UnderlineLinks", false);

	ckonqueror->setGroup( "FMSettings" );
	ckonqueror->writeEntry("UnderlineLinks", false);

	ckcminput->setGroup("KDE");
	ckcminput->writeEntry("ChangeCursor", true, true, true );

	appkeyfile=locate("appkeys", "app-mac3.kksrc");
	globalkeyfile=locate("globalkeys", "global-mac3.kksrc");
}


	/** write Keyscheme of application shortcuts (called by saveCheckStatus) */
void KOSPage::writeAppKeyEntrys(){
	kdDebug() << "KOSPage::writeAppKeyEntrys()" << endl;
	cglobal->setGroup("Keys");
		// get the keyscheme file and go to the key scheme group
	KSimpleConfig* scheme = new KSimpleConfig(appkeyfile, true);
	scheme->setGroup("Standard Key Scheme ");
		// copy the scheme-settings from the schemefile to the global config file
	cglobal->writeEntry("AddBookmark", scheme->readEntry("AddBookmark", "Ctrl+B"), true, true);
	cglobal->writeEntry("Back", scheme->readEntry("Back", "Alt+Left"), true, true);
	cglobal->writeEntry("Close", scheme->readEntry("Close", "Ctrl+W"), true, true);
	cglobal->writeEntry("Copy", scheme->readEntry("Copy", "Ctrl+C"), true, true);
	cglobal->writeEntry("Cut", scheme->readEntry("Cut", "Ctrl+X"), true, true);
	cglobal->writeEntry("DeleteWordBack", scheme->readEntry("DeleteWordBack", "Ctrl+Backspace"), true, true);
	cglobal->writeEntry("DeleteWordForward", scheme->readEntry("DeleteWordForward", "Ctrl+Delete"), true, true);
	cglobal->writeEntry("End", scheme->readEntry("End", "Ctrl+End"), true, true);
	cglobal->writeEntry("Find", scheme->readEntry("Find", "Ctrl+F"), true, true);
	cglobal->writeEntry("FindNext", scheme->readEntry("FindNext", "F3"), true, true);
	cglobal->writeEntry("FindPrev", scheme->readEntry("FindPrev", "Shift+F3"), true, true);
	cglobal->writeEntry("Forward", scheme->readEntry("Forward", "Alt+Right"), true, true);
	cglobal->writeEntry("GotoLine", scheme->readEntry("GotoLine", "Ctrl+G"), true, true);
	cglobal->writeEntry("Help", scheme->readEntry("Help", "F1"), true, true);
	cglobal->writeEntry("Home", scheme->readEntry("Home", "Ctrl+Home"), true, true);
	cglobal->writeEntry("Insert", scheme->readEntry("Insert", "Ctrl+Insert"), true, true);
	cglobal->writeEntry("New", scheme->readEntry("New", "Ctrl+N"), true, true);
	cglobal->writeEntry("Next", scheme->readEntry("Next", "Next"), true, true);
	cglobal->writeEntry("NextCompletion", scheme->readEntry("NextCompletion", "Ctrl+Down"), true, true);
	cglobal->writeEntry("Open", scheme->readEntry("Open", "Ctrl+O"), true, true);
	cglobal->writeEntry("Paste", scheme->readEntry("Paste", "Ctrl+V"), true, true);
	cglobal->writeEntry("PopupMenuContext", scheme->readEntry("PopupMenuContext", "Menu"), true, true);
	cglobal->writeEntry("PrevCompletion", scheme->readEntry("PrevCompletion", "Ctrl+Up"), true, true);
	cglobal->writeEntry("Print", scheme->readEntry("Print", "Ctrl+P"), true, true);
	cglobal->writeEntry("Prior", scheme->readEntry("Prior", "Prior"), true, true);
	cglobal->writeEntry("Quit", scheme->readEntry("Quit", "Ctrl+Q"), true, true);
	cglobal->writeEntry("Redo", scheme->readEntry("Redo", "Shift+Ctrl+Z"), true, true);
	cglobal->writeEntry("Reload", scheme->readEntry("Reload", "F5"), true, true);
	cglobal->writeEntry("Replace", scheme->readEntry("Replace", "Ctrl+R"), true, true);
	cglobal->writeEntry("RotateDown", scheme->readEntry("RotateDown", "Down"), true, true);
	cglobal->writeEntry("RotateUp", scheme->readEntry("RotateUp", "Up"), true, true);
	cglobal->writeEntry("Save", scheme->readEntry("Save", "Ctrl+S"), true, true);
	cglobal->writeEntry("SelectAll", scheme->readEntry("SelectAll", "Ctrl+A"), true, true);
	cglobal->writeEntry("ShowMenubar", scheme->readEntry("ShowMenubar", "Ctrl+M"), true, true);
	cglobal->writeEntry("TextCompletion", scheme->readEntry("TextCompletion", "Ctrl+E"), true, true);
	cglobal->writeEntry("Undo", scheme->readEntry("Undo", "Ctrl+Z"), true, true);
	cglobal->writeEntry("Up", scheme->readEntry("Up", "Alt+Up"), true, true);
	cglobal->writeEntry("WhatThis", scheme->readEntry("WhatThis", "Shift+F1"), true, true);
	cglobal->writeEntry("ZoomIn", scheme->readEntry("ZoomIn", "Ctrl+Plus"), true, true);
	cglobal->writeEntry("ZoomOut", scheme->readEntry("ZoomOut", "Ctrl+Minus"), true, true);
	delete scheme;
};


	/** write keyscheme of global shortcuts (called by saveCheckStatus) */
void KOSPage::writeGlobalKeyEntrys(){
	kdDebug() << "KOSPage::writeGlobalKeyEntrys()" << endl;
	cglobal->setGroup("Global Keys");
		// get the keyscheme file and go to the key scheme group
	KSimpleConfig* scheme = new KSimpleConfig(globalkeyfile, true);
	scheme->setGroup("Global Key Scheme ");
		// copy the scheme-settings from the schemefile to the global config file
	cglobal->writeEntry("Execute command", scheme->readEntry("Execute command"), true, true);
	cglobal->writeEntry("Kill Window", scheme->readEntry("Kill Window"), true, true);
	cglobal->writeEntry("Lock screen", scheme->readEntry("Lock screen"), true, true);
	cglobal->writeEntry("Logout", scheme->readEntry("Logout"), true, true);
	cglobal->writeEntry("Logout without Confirmation", scheme->readEntry("Logout without Confirmation"), true, true);
	cglobal->writeEntry("Mouse emulation", scheme->readEntry("Mouse emulation"), true, true);
	cglobal->writeEntry("Next keyboard layout", scheme->readEntry("Next keyboard layout"), true, true);
	cglobal->writeEntry("Pop-up window operations menu", scheme->readEntry("Pop-up window operations menu"), true, true);
	cglobal->writeEntry("Popup Launch Menu", scheme->readEntry("Popup Launch Menu"), true, true);
	cglobal->writeEntry("Show taskmanager", scheme->readEntry("Show taskmanager"), true, true);
	cglobal->writeEntry("Show window list", scheme->readEntry("Show window list"), true, true);
	cglobal->writeEntry("Switch desktop down", scheme->readEntry("Switch desktop down"), true, true);
	cglobal->writeEntry("Switch desktop left", scheme->readEntry("Switch desktop left"), true, true);
	cglobal->writeEntry("Switch desktop next", scheme->readEntry("Switch desktop next"), true, true);
	cglobal->writeEntry("Switch desktop previous", scheme->readEntry("Switch desktop previous"), true, true);
	cglobal->writeEntry("Switch desktop right", scheme->readEntry("Switch desktop right"), true, true);
	cglobal->writeEntry("Switch desktop up", scheme->readEntry("Switch desktop up"), true, true);
	cglobal->writeEntry("Switch to desktop 1", scheme->readEntry("Switch to desktop 1"), true, true);
	cglobal->writeEntry("Switch to desktop 10", scheme->readEntry("Switch to desktop 10"), true, true);
	cglobal->writeEntry("Switch to desktop 11", scheme->readEntry("Switch to desktop 11"), true, true);
	cglobal->writeEntry("Switch to desktop 12", scheme->readEntry("Switch to desktop 12"), true, true);
	cglobal->writeEntry("Switch to desktop 13", scheme->readEntry("Switch to desktop 13"), true, true);
	cglobal->writeEntry("Switch to desktop 14", scheme->readEntry("Switch to desktop 14"), true, true);
	cglobal->writeEntry("Switch to desktop 15", scheme->readEntry("Switch to desktop 15"), true, true);
	cglobal->writeEntry("Switch to desktop 16", scheme->readEntry("Switch to desktop 16"), true, true);
	cglobal->writeEntry("Switch to desktop 2", scheme->readEntry("Switch to desktop 2"), true, true);
	cglobal->writeEntry("Switch to desktop 3", scheme->readEntry("Switch to desktop 3"), true, true);
	cglobal->writeEntry("Switch to desktop 4", scheme->readEntry("Switch to desktop 4"), true, true);
	cglobal->writeEntry("Switch to desktop 5", scheme->readEntry("Switch to desktop 5"), true, true);
	cglobal->writeEntry("Switch to desktop 6", scheme->readEntry("Switch to desktop 6"), true, true);
	cglobal->writeEntry("Switch to desktop 7", scheme->readEntry("Switch to desktop 7"), true, true);
	cglobal->writeEntry("Switch to desktop 8", scheme->readEntry("Switch to desktop 8"), true, true);
	cglobal->writeEntry("Switch to desktop 9", scheme->readEntry("Switch to desktop 9"), true, true);
	cglobal->writeEntry("Toggle raise and lower", scheme->readEntry("Toggle raise and lower"), true, true);
	cglobal->writeEntry("Walk back through desktop list", scheme->readEntry("Walk back through desktop list"), true, true);
	cglobal->writeEntry("Walk back through desktops", scheme->readEntry("Walk back through desktops"), true, true);
	cglobal->writeEntry("Walk back through windows", scheme->readEntry("Walk back through windows"), true, true);
	cglobal->writeEntry("Walk through desktop list", scheme->readEntry("Walk through desktop list"), true, true);
	cglobal->writeEntry("Walk through desktops", scheme->readEntry("Walk through desktops"), true, true);
	cglobal->writeEntry("Walk through windows", scheme->readEntry("Walk through windows"), true, true);
	cglobal->writeEntry("Window close", scheme->readEntry("Window close"), true, true);
	cglobal->writeEntry("Window close all", scheme->readEntry("Window close all"), true, true);
	cglobal->writeEntry("Window iconify", scheme->readEntry("Window iconify"), true, true);
	cglobal->writeEntry("Window iconify all", scheme->readEntry("Window iconify all"), true, true);
	cglobal->writeEntry("Window lower", scheme->readEntry("Window lower"), true, true);
	cglobal->writeEntry("Window maximize", scheme->readEntry("Window maximize"), true, true);
	cglobal->writeEntry("Window maximize horizontal", scheme->readEntry("Window maximize horizontal"), true, true);
	cglobal->writeEntry("Window maximize vertical", scheme->readEntry("Window maximize vertical"), true, true);
	cglobal->writeEntry("Window move", scheme->readEntry("Window move"), true, true);
	cglobal->writeEntry("Window raise", scheme->readEntry("Window raise"), true, true);
	cglobal->writeEntry("Window resize", scheme->readEntry("Window resize"), true, true);
	cglobal->writeEntry("Window shade", scheme->readEntry("Window shade"), true, true);
	cglobal->writeEntry("Window to Desktop 1", scheme->readEntry("Window to Desktop 1"), true, true);
	cglobal->writeEntry("Window to Desktop 10", scheme->readEntry("Window to Desktop 10"), true, true);
	cglobal->writeEntry("Window to Desktop 11", scheme->readEntry("Window to Desktop 11"), true, true);
	cglobal->writeEntry("Window to Desktop 12", scheme->readEntry("Window to Desktop 12"), true, true);
	cglobal->writeEntry("Window to Desktop 13", scheme->readEntry("Window to Desktop 13"), true, true);
	cglobal->writeEntry("Window to Desktop 14", scheme->readEntry("Window to Desktop 14"), true, true);
	cglobal->writeEntry("Window to Desktop 15", scheme->readEntry("Window to Desktop 15"), true, true);
	cglobal->writeEntry("Window to Desktop 16", scheme->readEntry("Window to Desktop 16"), true, true);
	cglobal->writeEntry("Window to Desktop 2", scheme->readEntry("Window to Desktop 2"), true, true);
	cglobal->writeEntry("Window to Desktop 3", scheme->readEntry("Window to Desktop 3"), true, true);
	cglobal->writeEntry("Window to Desktop 4", scheme->readEntry("Window to Desktop 4"), true, true);
	cglobal->writeEntry("Window to Desktop 5", scheme->readEntry("Window to Desktop 5"), true, true);
	cglobal->writeEntry("Window to Desktop 6", scheme->readEntry("Window to Desktop 6"), true, true);
	cglobal->writeEntry("Window to Desktop 7", scheme->readEntry("Window to Desktop 7"), true, true);
	cglobal->writeEntry("Window to Desktop 8", scheme->readEntry("Window to Desktop 8"), true, true);
	cglobal->writeEntry("Window to Desktop 9", scheme->readEntry("Window to Desktop 9"), true, true);
	cglobal->writeEntry("Window to next desktop", scheme->readEntry("Window to next desktop"), true, true);
	cglobal->writeEntry("Window to previous desktop", scheme->readEntry("Window to previous desktop"), true, true);
	cglobal->writeEntry("repeat-last-klipper-action", scheme->readEntry("repeat-last-klipper-action"), true, true);
	cglobal->writeEntry("show-klipper-popupmenu", scheme->readEntry("show-klipper-popupmenu"), true, true);
	cglobal->writeEntry("toggle-clipboard-actions", scheme->readEntry("toggle-clipboard-actions"), true, true);
	delete scheme;
}


void KOSPage::slotKDEDescription(){
	kdDebug() << "slotKDEDescription()" << endl;
	textview_ospage->setText("");
	textview_ospage->setText(i18n(
	"<b>Window decorations:</b> <i>KDE default</i><br>"
	"<b>Window activation:</b> <i>Focus on click</i><br>"
	"<b>Titlebar doubleclick:</b> <i>Shade window</i><br>"
	"<b>Mouse selection:</b> <i>Single click</i><br>"
	"<b>Application startup notification:</b> <i>busy cursor</i><br>"
	"<b>Keyboard scheme:</b> <i>KDE default</i><br>"
	));
}


void KOSPage::slotUnixDescription(){
	kdDebug() << "slotUnixDescription()" << endl;
	textview_ospage->setText("" );
	textview_ospage->setText(i18n(
	"<b>Window decorations:</b> <i>MWM</i><br>"
	"<b>Window activation:</b> <i>Focus follows mouse</i><br>"
	"<b>Titlebar doubleclick:</b> <i>Send window to back</i><br>"
	"<b>Mouse selection:</b> <i>Single click</i><br>"
	"<b>Application startup notification:</b> <i>none</i><br>"
	"<b>Keyboard scheme:</b> <i>UNIX</i><br>"
	));
}


void KOSPage::slotWindowsDescription(){
	kdDebug() << "slotWindowsDescription()" << endl;
	textview_ospage->setText("");
	textview_ospage->setText(i18n(
	"<b>Window decorations:</b> <i>Redmond</i><br>"
	"<b>Window activation:</b> <i>Focus on click</i><br>"
	"<b>Titlebar doubleclick:</b> <i>Maximize window</i><br>"
	"<b>Mouse selection:</b> <i>Double click</i><br>"
	"<b>Application startup notification:</b> <i>busy cursor</i><br>"
	"<b>Keyboard scheme:</b> <i>Windows</i><br>"
	));
}


void KOSPage::slotMacDescription(){
	kdDebug() << "slotMacDescription()" << endl;
	textview_ospage->setText("");
	textview_ospage->setText(i18n(
	"<b>Window decorations:</b> <i>ModSystem</i><br>"
	"<b>Window activation:</b> <i>Focus on click</i><br>"
	"<b>Titlebar doubleclick:</b> <i>Shade window</i><br>"
	"<b>Mouse selection:</b> <i>Single click</i><br>"
	"<b>Application startup notification:</b> <i>none</i><br>"
	"<b>Keyboard scheme:</b> <i>Mac</i><br>"
	));
}


	/** retrieves the user's local values. In case he doesn't have these set, use the default values of KDE */
void KOSPage::getUserDefaults(){
	ckcmdisplay->setGroup("KDE");
	b_MacMenuBar = ckcmdisplay->readBoolEntry("macStyle", false);

	cglobal->setGroup("KDE");
	b_SingleClick = cglobal->readBoolEntry("SingleClick", true);

	claunch->setGroup("FeedbackStyle");
	b_BusyCursor = claunch->readBoolEntry("BusyCursor", true);

	cwin->setGroup("Style");
	s_PluginLib = cwin->readEntry("PluginLib");
	cwin->setGroup("Windows");
	s_TitlebarDCC = cwin->readEntry("TitlebarDoubleClickCommand", "Shade");
	s_FocusPolicy = cwin->readEntry("FocusPolicy", "ClickToFocus");
	s_AltTabStyle = cwin->readEntry("AltTabStyle", "KDE");
	cwin->setGroup( "MouseBindings");
	s_TitlebarMMB = cwin->readEntry("CommandActiveTitlebar2", "Lower");
	s_TitlebarRMB = cwin->readEntry("CommandActiveTitlebar3", "Operations menu");

	cdesktop->setGroup( "Menubar" );
	b_ShowMenuBar = cdesktop->readBoolEntry("ShowMenubar", false);
	cdesktop->setGroup( "Mouse Buttons" );
	s_MMB = cdesktop->readEntry("Middle", "WindowListMenu");
	cdesktop->setGroup( "FMSettings" );
	b_DesktopUnderline = cdesktop->readBoolEntry("UnderlineLinks", true);

	ckonqueror->setGroup( "FMSettings" );
	b_KonqUnderline = ckonqueror->readBoolEntry("UnderlineLinks", true);

	ckcminput->setGroup("KDE");
	b_ChangeCursor = ckcminput->readBoolEntry("ChangeCursor", true);

	getUserAppKeys();
	getUserGlobalKeys();
}


	/** writes the user-defaults back */
void KOSPage::writeUserDefaults(){
	kdDebug() << "KOSPage::writeUserDefaults()" << endl;

	ckcmdisplay->setGroup("KDE");
	ckcmdisplay->writeEntry("macStyle", b_MacMenuBar, true, true);

	cglobal->setGroup("KDE");
	cglobal->writeEntry("SingleClick", b_SingleClick, true, true);

	claunch->setGroup("FeedbackStyle");
	claunch->writeEntry("BusyCursor", b_BusyCursor);

	cwin->setGroup("Style");
	cwin->writeEntry("PluginLib", s_PluginLib);
	cwin->setGroup("Windows");
	cwin->writeEntry("TitlebarDoubleClickCommand", s_TitlebarDCC);
	cwin->writeEntry("FocusPolicy", s_FocusPolicy);
	cwin->writeEntry("AltTabStyle", s_AltTabStyle);
	cwin->setGroup( "MouseBindings");
	cwin->writeEntry("CommandActiveTitlebar2", s_TitlebarMMB);
	cwin->writeEntry("CommandActiveTitlebar3", s_TitlebarRMB);

	cdesktop->setGroup( "Menubar" );
	cdesktop->writeEntry("ShowMenubar", b_ShowMenuBar);
	cdesktop->setGroup( "Mouse Buttons" );
	cdesktop->writeEntry("Middle", s_MMB);
	cdesktop->setGroup( "FMSettings" );
	cdesktop->writeEntry("UnderlineLinks", b_DesktopUnderline);

	ckonqueror->setGroup( "FMSettings" );
	ckonqueror->writeEntry("UnderlineLinks", b_KonqUnderline);

	ckcminput->setGroup("KDE");
	ckcminput->writeEntry("ChangeCursor", b_ChangeCursor, true, true );

	writeUserAppKeys();
	writeUserGlobalKeys();
}


	/** called by getUserDefaults() */
void KOSPage::getUserAppKeys(){
	kdDebug() << "KOSPage::getUserAppKeys()" << endl;
	cglobal->setGroup("Keys");
	s_UserAppKeys[0] = cglobal->readEntry("AddBookmark");
	s_UserAppKeys[1] = cglobal->readEntry("Back");
	s_UserAppKeys[2] = cglobal->readEntry("Close");
	s_UserAppKeys[3] = cglobal->readEntry("Copy");
	s_UserAppKeys[4] = cglobal->readEntry("Cut");
	s_UserAppKeys[5] = cglobal->readEntry("DeleteWordBack");
	s_UserAppKeys[6] = cglobal->readEntry("DeleteWordForward");
	s_UserAppKeys[7] = cglobal->readEntry("End");
	s_UserAppKeys[8] = cglobal->readEntry("Find");
	s_UserAppKeys[9] = cglobal->readEntry("FindNext");
	s_UserAppKeys[10] = cglobal->readEntry("FindPrev");
	s_UserAppKeys[11] = cglobal->readEntry("Forward");
	s_UserAppKeys[12] = cglobal->readEntry("GotoLine");
	s_UserAppKeys[13] = cglobal->readEntry("Help");
	s_UserAppKeys[14] = cglobal->readEntry("Home");
	s_UserAppKeys[15] = cglobal->readEntry("Insert");
	s_UserAppKeys[16] = cglobal->readEntry("New");
	s_UserAppKeys[17] = cglobal->readEntry("Next");
	s_UserAppKeys[18] = cglobal->readEntry("NextCompletion");
	s_UserAppKeys[19] = cglobal->readEntry("Open");
	s_UserAppKeys[20] = cglobal->readEntry("Paste");
	s_UserAppKeys[21] = cglobal->readEntry("PopupMenuContext");
	s_UserAppKeys[22] = cglobal->readEntry("PrevCompletion");
	s_UserAppKeys[23] = cglobal->readEntry("Print");
	s_UserAppKeys[24] = cglobal->readEntry("Prior");
	s_UserAppKeys[25] = cglobal->readEntry("Quit");
	s_UserAppKeys[26] = cglobal->readEntry("Redo");
	s_UserAppKeys[27] = cglobal->readEntry("Reload");
	s_UserAppKeys[28] = cglobal->readEntry("Replace");
	s_UserAppKeys[29] = cglobal->readEntry("RotateDown");
	s_UserAppKeys[30] = cglobal->readEntry("RotateUp");
	s_UserAppKeys[31] = cglobal->readEntry("Save");
	s_UserAppKeys[32] = cglobal->readEntry("SelectAll");
	s_UserAppKeys[33] = cglobal->readEntry("ShowMenubar");
	s_UserAppKeys[34] = cglobal->readEntry("TextCompletion");
	s_UserAppKeys[35] = cglobal->readEntry("Undo");
	s_UserAppKeys[36] = cglobal->readEntry("Up");
	s_UserAppKeys[37] = cglobal->readEntry("WhatThis");
	s_UserAppKeys[38] = cglobal->readEntry("ZoomIn");
	s_UserAppKeys[39] = cglobal->readEntry("ZoomOut");
}


	/** called by getUserDefaults() */
void KOSPage::getUserGlobalKeys(){
	kdDebug() << "KOSPage::getUserGlobalKeys()" << endl;
	cglobal->setGroup("Global Keys");
	s_UserGlobalKeys[0] = cglobal->readEntry("Execute command");
	s_UserGlobalKeys[1] = cglobal->readEntry("Kill Window");
	s_UserGlobalKeys[2] = cglobal->readEntry("Lock screen");
	s_UserGlobalKeys[3] = cglobal->readEntry("Logout");
	s_UserGlobalKeys[4] = cglobal->readEntry("Logout without Confirmation");
	s_UserGlobalKeys[5] = cglobal->readEntry("Mouse emulation");
	s_UserGlobalKeys[6] = cglobal->readEntry("Next keyboard layout");
	s_UserGlobalKeys[7] = cglobal->readEntry("Pop-up window operations menu");
	s_UserGlobalKeys[8] = cglobal->readEntry("Popup Launch Menu");
	s_UserGlobalKeys[9] = cglobal->readEntry("Show taskmanager");
	s_UserGlobalKeys[10] = cglobal->readEntry("Show window list");
	s_UserGlobalKeys[11] = cglobal->readEntry("Switch desktop down");
	s_UserGlobalKeys[12] = cglobal->readEntry("Switch desktop left");
	s_UserGlobalKeys[13] = cglobal->readEntry("Switch desktop next");
	s_UserGlobalKeys[14] = cglobal->readEntry("Switch desktop previous");
	s_UserGlobalKeys[15] = cglobal->readEntry("Switch desktop right");
	s_UserGlobalKeys[16] = cglobal->readEntry("Switch desktop up");
	s_UserGlobalKeys[17] = cglobal->readEntry("Switch to desktop 1");
	s_UserGlobalKeys[18] = cglobal->readEntry("Switch to desktop 10");
	s_UserGlobalKeys[19] = cglobal->readEntry("Switch to desktop 11");
	s_UserGlobalKeys[20] = cglobal->readEntry("Switch to desktop 12");
	s_UserGlobalKeys[21] = cglobal->readEntry("Switch to desktop 13");
	s_UserGlobalKeys[22] = cglobal->readEntry("Switch to desktop 14");
	s_UserGlobalKeys[23] = cglobal->readEntry("Switch to desktop 15");
	s_UserGlobalKeys[24] = cglobal->readEntry("Switch to desktop 16");
	s_UserGlobalKeys[25] = cglobal->readEntry("Switch to desktop 2");
	s_UserGlobalKeys[26] = cglobal->readEntry("Switch to desktop 3");
	s_UserGlobalKeys[27] = cglobal->readEntry("Switch to desktop 4");
	s_UserGlobalKeys[28] = cglobal->readEntry("Switch to desktop 5");
	s_UserGlobalKeys[29] = cglobal->readEntry("Switch to desktop 6");
	s_UserGlobalKeys[30] = cglobal->readEntry("Switch to desktop 7");
	s_UserGlobalKeys[31] = cglobal->readEntry("Switch to desktop 8");
	s_UserGlobalKeys[32] = cglobal->readEntry("Switch to desktop 9");
	s_UserGlobalKeys[33] = cglobal->readEntry("Toggle raise and lower");
	s_UserGlobalKeys[34] = cglobal->readEntry("Walk back through desktop list");
	s_UserGlobalKeys[35] = cglobal->readEntry("Walk back through desktops");
	s_UserGlobalKeys[36] = cglobal->readEntry("Walk back through windows");
	s_UserGlobalKeys[37] = cglobal->readEntry("Walk through desktop list");
	s_UserGlobalKeys[38] = cglobal->readEntry("Walk through desktops");
	s_UserGlobalKeys[39] = cglobal->readEntry("Walk through windows");
	s_UserGlobalKeys[40] = cglobal->readEntry("Window close");
	s_UserGlobalKeys[41] = cglobal->readEntry("Window close all");
	s_UserGlobalKeys[42] = cglobal->readEntry("Window iconify");
	s_UserGlobalKeys[43] = cglobal->readEntry("Window iconify all");
	s_UserGlobalKeys[44] = cglobal->readEntry("Window lower");
	s_UserGlobalKeys[45] = cglobal->readEntry("Window maximize");
	s_UserGlobalKeys[46] = cglobal->readEntry("Window maximize horizontal");
	s_UserGlobalKeys[47] = cglobal->readEntry("Window maximize vertical");
	s_UserGlobalKeys[48] = cglobal->readEntry("Window move");
	s_UserGlobalKeys[49] = cglobal->readEntry("Window raise");
	s_UserGlobalKeys[50] = cglobal->readEntry("Window resize");
	s_UserGlobalKeys[51] = cglobal->readEntry("Window shade");
	s_UserGlobalKeys[52] = cglobal->readEntry("Window to Desktop 1");
	s_UserGlobalKeys[53] = cglobal->readEntry("Window to Desktop 10");
	s_UserGlobalKeys[54] = cglobal->readEntry("Window to Desktop 11");
	s_UserGlobalKeys[55] = cglobal->readEntry("Window to Desktop 12");
	s_UserGlobalKeys[56] = cglobal->readEntry("Window to Desktop 13");
	s_UserGlobalKeys[57] = cglobal->readEntry("Window to Desktop 14");
	s_UserGlobalKeys[58] = cglobal->readEntry("Window to Desktop 15");
	s_UserGlobalKeys[59] = cglobal->readEntry("Window to Desktop 16");
	s_UserGlobalKeys[60] = cglobal->readEntry("Window to Desktop 2");
	s_UserGlobalKeys[61] = cglobal->readEntry("Window to Desktop 3");
	s_UserGlobalKeys[62] = cglobal->readEntry("Window to Desktop 4");
	s_UserGlobalKeys[63] = cglobal->readEntry("Window to Desktop 5");
	s_UserGlobalKeys[64] = cglobal->readEntry("Window to Desktop 6");
	s_UserGlobalKeys[65] = cglobal->readEntry("Window to Desktop 7");
	s_UserGlobalKeys[66] = cglobal->readEntry("Window to Desktop 8");
	s_UserGlobalKeys[67] = cglobal->readEntry("Window to Desktop 9");
	s_UserGlobalKeys[68] = cglobal->readEntry("Window to next desktop");
	s_UserGlobalKeys[69] = cglobal->readEntry("Window to previous desktop");
	s_UserGlobalKeys[70] = cglobal->readEntry("repeat-last-klipper-action");
	s_UserGlobalKeys[71] = cglobal->readEntry("show-klipper-popupmenu");
	s_UserGlobalKeys[72] = cglobal->readEntry("toggle-clipboard-actions");
}


	/** called by writeUserDefaults() */
void KOSPage::writeUserAppKeys(){
	kdDebug() << "KOSPage::writeAppKeyEntrys()" << endl;
	cglobal->setGroup("Keys");
	cglobal->writeEntry("AddBookmark", s_UserAppKeys[0], true, true);
	cglobal->writeEntry("Back", s_UserAppKeys[1], true, true);
	cglobal->writeEntry("Close", s_UserAppKeys[2], true, true);
	cglobal->writeEntry("Copy", s_UserAppKeys[3], true, true);
	cglobal->writeEntry("Cut", s_UserAppKeys[4], true, true);
	cglobal->writeEntry("DeleteWordBack", s_UserAppKeys[5], true, true);
	cglobal->writeEntry("DeleteWordForward", s_UserAppKeys[6], true, true);
	cglobal->writeEntry("End", s_UserAppKeys[7], true, true);
	cglobal->writeEntry("Find", s_UserAppKeys[8], true, true);
	cglobal->writeEntry("FindNext", s_UserAppKeys[9], true, true);
	cglobal->writeEntry("FindPrev", s_UserAppKeys[10], true, true);
	cglobal->writeEntry("Forward", s_UserAppKeys[11], true, true);
	cglobal->writeEntry("GotoLine", s_UserAppKeys[12], true, true);
	cglobal->writeEntry("Help", s_UserAppKeys[13], true, true);
	cglobal->writeEntry("Home", s_UserAppKeys[14], true, true);
	cglobal->writeEntry("Insert", s_UserAppKeys[15], true, true);
	cglobal->writeEntry("New", s_UserAppKeys[16], true, true);
	cglobal->writeEntry("Next", s_UserAppKeys[17], true, true);
	cglobal->writeEntry("NextCompletion", s_UserAppKeys[18], true, true);
	cglobal->writeEntry("Open", s_UserAppKeys[19], true, true);
	cglobal->writeEntry("Paste", s_UserAppKeys[20], true, true);
	cglobal->writeEntry("PopupMenuContext", s_UserAppKeys[21], true, true);
	cglobal->writeEntry("PrevCompletion", s_UserAppKeys[22], true, true);
	cglobal->writeEntry("Print", s_UserAppKeys[23], true, true);
	cglobal->writeEntry("Prior", s_UserAppKeys[24], true, true);
	cglobal->writeEntry("Quit", s_UserAppKeys[25], true, true);
	cglobal->writeEntry("Redo", s_UserAppKeys[26], true, true);
	cglobal->writeEntry("Reload", s_UserAppKeys[27], true, true);
	cglobal->writeEntry("Replace", s_UserAppKeys[28], true, true);
	cglobal->writeEntry("RotateDown", s_UserAppKeys[29], true, true);
	cglobal->writeEntry("RotateUp", s_UserAppKeys[30], true, true);
	cglobal->writeEntry("Save", s_UserAppKeys[31], true, true);
	cglobal->writeEntry("SelectAll", s_UserAppKeys[32], true, true);
	cglobal->writeEntry("ShowMenubar", s_UserAppKeys[33], true, true);
	cglobal->writeEntry("TextCompletion", s_UserAppKeys[34], true, true);
	cglobal->writeEntry("Undo", s_UserAppKeys[35], true, true);
	cglobal->writeEntry("Up", s_UserAppKeys[36], true, true);
	cglobal->writeEntry("WhatThis", s_UserAppKeys[37], true, true);
	cglobal->writeEntry("ZoomIn", s_UserAppKeys[38], true, true);
	cglobal->writeEntry("ZoomOut", s_UserAppKeys[39], true, true);
}


	/** called by writeUserDefaults */
void KOSPage::writeUserGlobalKeys(){
	kdDebug() << "KOSPage::writeUserGlobalKeys()" << endl;
	cglobal->setGroup("Global Keys");
	cglobal->writeEntry("Execute command", s_UserGlobalKeys[0], true, true);
	cglobal->writeEntry("Kill Window", s_UserGlobalKeys[1], true, true);
	cglobal->writeEntry("Lock screen", s_UserGlobalKeys[2], true, true);
	cglobal->writeEntry("Logout", s_UserGlobalKeys[3], true, true);
	cglobal->writeEntry("Logout without Confirmation", s_UserGlobalKeys[4], true, true);
	cglobal->writeEntry("Mouse emulation", s_UserGlobalKeys[5], true, true);
	cglobal->writeEntry("Next keyboard layout", s_UserGlobalKeys[6], true, true);
	cglobal->writeEntry("Pop-up window operations menu", s_UserGlobalKeys[7], true, true);
	cglobal->writeEntry("Popup Launch Menu", s_UserGlobalKeys[8], true, true);
	cglobal->writeEntry("Show taskmanager", s_UserGlobalKeys[9], true, true);
	cglobal->writeEntry("Show window list", s_UserGlobalKeys[10], true, true);
	cglobal->writeEntry("Switch desktop down", s_UserGlobalKeys[11], true, true);
	cglobal->writeEntry("Switch desktop left", s_UserGlobalKeys[12], true, true);
	cglobal->writeEntry("Switch desktop next", s_UserGlobalKeys[13], true, true);
	cglobal->writeEntry("Switch desktop previous", s_UserGlobalKeys[14], true, true);
	cglobal->writeEntry("Switch desktop right", s_UserGlobalKeys[15], true, true);
	cglobal->writeEntry("Switch desktop up", s_UserGlobalKeys[16], true, true);
	cglobal->writeEntry("Switch to desktop 1", s_UserGlobalKeys[17], true, true);
	cglobal->writeEntry("Switch to desktop 10", s_UserGlobalKeys[18], true, true);
	cglobal->writeEntry("Switch to desktop 11", s_UserGlobalKeys[19], true, true);
	cglobal->writeEntry("Switch to desktop 12", s_UserGlobalKeys[20], true, true);
	cglobal->writeEntry("Switch to desktop 13", s_UserGlobalKeys[21], true, true);
	cglobal->writeEntry("Switch to desktop 14", s_UserGlobalKeys[22], true, true);
	cglobal->writeEntry("Switch to desktop 15", s_UserGlobalKeys[23], true, true);
	cglobal->writeEntry("Switch to desktop 16", s_UserGlobalKeys[24], true, true);
	cglobal->writeEntry("Switch to desktop 2", s_UserGlobalKeys[25], true, true);
	cglobal->writeEntry("Switch to desktop 3", s_UserGlobalKeys[26], true, true);
	cglobal->writeEntry("Switch to desktop 4", s_UserGlobalKeys[27], true, true);
	cglobal->writeEntry("Switch to desktop 5", s_UserGlobalKeys[28], true, true);
	cglobal->writeEntry("Switch to desktop 6", s_UserGlobalKeys[29], true, true);
	cglobal->writeEntry("Switch to desktop 7", s_UserGlobalKeys[30], true, true);
	cglobal->writeEntry("Switch to desktop 8", s_UserGlobalKeys[31], true, true);
	cglobal->writeEntry("Switch to desktop 9", s_UserGlobalKeys[32], true, true);
	cglobal->writeEntry("Toggle raise and lower", s_UserGlobalKeys[33], true, true);
	cglobal->writeEntry("Walk back through desktop list", s_UserGlobalKeys[34], true, true);
	cglobal->writeEntry("Walk back through desktops", s_UserGlobalKeys[35], true, true);
	cglobal->writeEntry("Walk back through windows", s_UserGlobalKeys[36], true, true);
	cglobal->writeEntry("Walk through desktop list", s_UserGlobalKeys[37], true, true);
	cglobal->writeEntry("Walk through desktops", s_UserGlobalKeys[38], true, true);
	cglobal->writeEntry("Walk through windows", s_UserGlobalKeys[39], true, true);
	cglobal->writeEntry("Window close", s_UserGlobalKeys[40], true, true);
	cglobal->writeEntry("Window close all", s_UserGlobalKeys[41], true, true);
	cglobal->writeEntry("Window iconify", s_UserGlobalKeys[42], true, true);
	cglobal->writeEntry("Window iconify all", s_UserGlobalKeys[43], true, true);
	cglobal->writeEntry("Window lower", s_UserGlobalKeys[44], true, true);
	cglobal->writeEntry("Window maximize", s_UserGlobalKeys[45], true, true);
	cglobal->writeEntry("Window maximize horizontal", s_UserGlobalKeys[46], true, true);
	cglobal->writeEntry("Window maximize vertical", s_UserGlobalKeys[47], true, true);
	cglobal->writeEntry("Window move", s_UserGlobalKeys[48], true, true);
	cglobal->writeEntry("Window raise", s_UserGlobalKeys[49], true, true);
	cglobal->writeEntry("Window resize", s_UserGlobalKeys[50], true, true);
	cglobal->writeEntry("Window shade", s_UserGlobalKeys[51], true, true);
	cglobal->writeEntry("Window to Desktop 1", s_UserGlobalKeys[52], true, true);
	cglobal->writeEntry("Window to Desktop 10", s_UserGlobalKeys[53], true, true);
	cglobal->writeEntry("Window to Desktop 11", s_UserGlobalKeys[54], true, true);
	cglobal->writeEntry("Window to Desktop 12", s_UserGlobalKeys[55], true, true);
	cglobal->writeEntry("Window to Desktop 13", s_UserGlobalKeys[56], true, true);
	cglobal->writeEntry("Window to Desktop 14", s_UserGlobalKeys[57], true, true);
	cglobal->writeEntry("Window to Desktop 15", s_UserGlobalKeys[58], true, true);
	cglobal->writeEntry("Window to Desktop 16", s_UserGlobalKeys[59], true, true);
	cglobal->writeEntry("Window to Desktop 2", s_UserGlobalKeys[60], true, true);
	cglobal->writeEntry("Window to Desktop 3", s_UserGlobalKeys[61], true, true);
	cglobal->writeEntry("Window to Desktop 4", s_UserGlobalKeys[62], true, true);
	cglobal->writeEntry("Window to Desktop 5", s_UserGlobalKeys[63], true, true);
	cglobal->writeEntry("Window to Desktop 6", s_UserGlobalKeys[64], true, true);
	cglobal->writeEntry("Window to Desktop 7", s_UserGlobalKeys[65], true, true);
	cglobal->writeEntry("Window to Desktop 8", s_UserGlobalKeys[66], true, true);
	cglobal->writeEntry("Window to Desktop 9", s_UserGlobalKeys[67], true, true);
	cglobal->writeEntry("Window to next desktop", s_UserGlobalKeys[68], true, true);
	cglobal->writeEntry("Window to previous desktop", s_UserGlobalKeys[69], true, true);
	cglobal->writeEntry("repeat-last-klipper-action", s_UserGlobalKeys[70], true, true);
	cglobal->writeEntry("show-klipper-popupmenu", s_UserGlobalKeys[71], true, true);
	cglobal->writeEntry("toggle-clipboard-actions", s_UserGlobalKeys[72], true, true);
}


	/** resets the radio button selected to kde */
void KOSPage::setDefaults(){
    rb_kde->setChecked(true);
}

#include "kospage.moc"
