/***************************************************************************
                          kstylepage.h  -  description
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

#ifndef KSTYLEPAGE_H
#define KSTYLEPAGE_H

#include <qcolor.h>
#include "kstylepagedlg.h"

/**Abstract class for the style page
  *@author Ralf Nolden
  */
class QListViewItem;

class KStylePage : public KStylePageDlg  {
   Q_OBJECT
public: 
	KStylePage(QWidget *parent=0, const char *name=0);
	~KStylePage();
	void save(bool curSettings=true);
  /** No descriptions */
  void saveColors( bool curSettings=true);
  /** No descriptions */
  void saveTheme(bool curSettings=true);
  /** resets to KDE style as default */
  void setDefaults();
  /** No descriptions */
public slots: // Public slots
  /** to be connected to the OS page. Catches
either KDE, CDE, win or mac and pre-sets the style.
 */
  void presetStyle(const QString& style);

  private:
    QString currTheme;
  struct colorSet{
    QString colorFile, bgMode;
    int contrast;
    QColor usrCol1, usrCol2;
    QColor foreground;
    QColor background;
    QColor windowForeground;
    QColor windowBackground;
    QColor selectForeground;
    QColor selectBackground;
    QColor buttonForeground;
    QColor buttonBackground;
    QColor linkColor;
    QColor visitedLinkColor;
    QColor activeForeground;
    QColor inactiveForeground;
    QColor activeBackground;
    QColor inactiveBackground;
    QColor activeBlend;
    QColor inactiveBlend;
    QColor activeTitleBtnBg;
    QColor inactiveTitleBtnBg;
  } usrColors, themeColors;
    // first, the KDE 2 default color values
    QColor kde2Blue;
    QColor widget;
    QColor button;
    QColor link;
    QColor visitedLink;
    void getColors(colorSet *set, bool colorfile );

    QListViewItem * kde;
    QListViewItem * cde;
    QListViewItem * win;
    QListViewItem * mac;

};

#endif
