/***************************************************************************
                          kospage.h  -  description
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

#ifndef KOSPAGE_H
#define KOSPAGE_H


#include"kospagedlg.h"

/**Abstract class for the second page.  Uses save() to change the according settings and applies them.
  *@author Ralf Nolden
  */

class KOSPage : public KOSPageDlg  {
	Q_OBJECT
public:
	KOSPage(QWidget *parent=0, const char *name=0);
	~KOSPage();
	void save(bool currSettings=true);
	void saveCheckState(bool currSettings);
	void writeKDE();
	void writeUNIX();
	void writeWindows();
	void writeMacOS();
	void writeAppKeyEntrys();
	void writeGlobalKeyEntrys();
	void writeUserDefaults();
	/** retrieve the user's local values */
	void getUserDefaults();
	void slotMacDescription();
	void slotWindowsDescription();
	void slotUnixDescription();
	void slotKDEDescription();
	/** resets the radio button selected to kde */
	void setDefaults();
	void getUserGlobalKeys();
	void getUserAppKeys();
	void writeUserGlobalKeys();
	void writeUserAppKeys();
signals: // Signals
	/** emits either of: KDE, CDE, win or mac in save() depending
	on the selection made by the user. */
	void selectedOS(const QString&);
private:
	QString appkeyfile;
	QString globalkeyfile;
	KConfig* cglobal;
	KConfig* claunch;
	KConfig* cwin;
	KConfig* cdesktop;
	KConfig* ckcminput;
	KConfig* ckcmdisplay;
	KConfig* ckonqueror;
	// DEFAULT VALUES SET BY USER
	bool b_MacMenuBar, b_SingleClick, b_BusyCursor, b_ShowMenuBar,
		 b_DesktopUnderline, b_KonqUnderline, b_ChangeCursor;
	QString	s_PluginLib, s_TitlebarDCC, s_FocusPolicy, s_AltTabStyle, s_MMB,
			s_TitlebarMMB, s_TitlebarRMB, s_UserAppKeys[40], s_UserGlobalKeys[73];
	// DEFAULT VALLUES SET BY USER (END)
};
#endif
