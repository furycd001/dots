/***************************************************************************
                          kstylepage.cpp  -  description
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
#include <stdlib.h>

#include <qlabel.h>
#include <qlistview.h>
#include <qcolor.h>

#include <kconfig.h>
#include <kstddirs.h>
#include <kapp.h>
#include <kglobal.h>
#include <kdebug.h>
#include <klocale.h>
#include <klistview.h>
#include <kipc.h>
#include <kthemebase.h>
#include <ksimpleconfig.h>
#include <dcopclient.h>

#include "kstylepage.h"

KStylePage::KStylePage(QWidget *parent, const char *name ) : KStylePageDlg(parent,name) {
   px_themesSidebar->setPixmap(locate("data", "kpersonalizer/pics/step4.png"));

  klv_styles->addColumn(i18n("Theme"));
  klv_styles->addColumn( i18n( "Description" ) );
  klv_styles->setAllColumnsShowFocus(true);

  kde = new QListViewItem( klv_styles);
  kde->setText( 0, i18n( "KDE" ) );
  kde->setText( 1, i18n( "KDE default style" ) );

  cde = new QListViewItem( klv_styles);
  cde->setText( 0, i18n( "Sunshine" ) );
  cde->setText( 1, i18n( "A very common desktop" ) );

  win = new QListViewItem( klv_styles );
  win->setText( 0, i18n( "Redmond" ) );
  win->setText( 1, i18n( "A style from the northwest of the USA" ) );

  mac = new QListViewItem( klv_styles );
  mac->setText( 0, i18n( "Platinum" ) );
  mac->setText( 1, i18n( "The platinum style" ) );


//  evex = new QListViewItem( lv_styles, item );
//  evex->setText( 0, i18n( "Eve X" ) );
//  evex->setText( 1, i18n( "some people like apples" ) );

//  item = new QListViewItem( lv_styles, item );
//  item->setText( 0, i18n( "Technical" ) );
//  item->setText( 1, i18n( "technical look" ) );
    // first, the KDE 2 default color values
    if (QPixmap::defaultDepth() > 8)
      kde2Blue.setRgb(10, 95, 137);
    else
      kde2Blue.setRgb(0, 0, 192);

    widget.setRgb(220, 220, 220);

    if (QPixmap::defaultDepth() > 8)
      button.setRgb(228, 228, 228);
    else
      button.setRgb(220, 220, 220);

    link.setRgb(0, 0, 192);
    visitedLink.setRgb(128, 0,128);

    // Theme file settings
    KGlobal::config()->setGroup("KDE");
    KGlobal::dirs()->addResourceType("themes", KStandardDirs::kde_default("data") + "kstyle2/themes");
    QString themefile;
    if(QColor::numBitPlanes() > 8)
        themefile=locate("themes","highcolor.themerc");
    else
        themefile=locate("themes","default.themerc");
    currTheme=KGlobal::config()->readEntry("currentTheme", themefile);

    getColors(&usrColors, false);  // first get the user´s current colors with reading them from kdeglobals
}
KStylePage::~KStylePage(){
}
/** No descriptions */
void KStylePage::save(bool curSettings){
    kdDebug() << "KStylePage::save()" << endl;
    // First, the theme, then the colors as themes overwrite color settings
  saveTheme(curSettings);
  saveColors(curSettings);

}
/** No descriptions */
void KStylePage::saveTheme(bool curSettings){

  QString themefile;

  if(curSettings){
    KGlobal::dirs()->addResourceType("themes", KStandardDirs::kde_default("data") + "kstyle2/themes");

    // set the style
    if(kde->isSelected()){
        // Use the highcolor theme if the display supports it
	if(QColor::numBitPlanes() > 8)
            themefile=locate("themes","highcolor.themerc");
	else
            themefile=locate("themes","default.themerc");
    }
    else if(cde->isSelected()){
        themefile=locate("themes","qtmotif.themerc");
    }
    else if(win->isSelected()){
        themefile=locate("themes","qtwindows.themerc");
    }
    else if(mac->isSelected()){
        themefile=locate("themes","qtplatinum.themerc");
    }
  }
  else{
    themefile=currTheme; //reset theme to the user's theme
  }
  KThemeBase::applyConfigFile(themefile);
  KIPC::sendMessageAll(KIPC::StyleChanged);
}
/** No descriptions */
void KStylePage::saveColors(bool curSettings){
  struct colorSet* toSave;

  QString colorfile;
  if(curSettings){
    getColors(&themeColors, true);
    toSave=&themeColors;  // set the color struct to save as the theme colors
  }
  else{
     toSave=&usrColors;
  }

    // the GLOBAL config entries must be taken from the kcsrc file and written to it. Use the default values
    // equals that the file is <default> which is no file. TODO: use the default values in that case (kde selected)
    KConfig *config = KGlobal::config();
    config->setGroup( "General" );
    config->writeEntry("foreground", toSave->foreground, true, true);
    config->writeEntry("background", toSave->background, true, true);
    config->writeEntry("windowForeground", toSave->windowForeground, true, true);
    config->writeEntry("windowBackground", toSave->windowBackground, true, true);
    config->writeEntry("selectForeground",  toSave->selectForeground, true, true);
    config->writeEntry("selectBackground", toSave->selectBackground, true, true);
    config->writeEntry("buttonForeground", toSave->buttonForeground, true, true);
    config->writeEntry("buttonBackground", toSave->buttonBackground, true, true);
    config->writeEntry("linkColor", toSave->linkColor, true, true);
    config->writeEntry("visitedLinkColor", toSave->visitedLinkColor, true, true);
    // set to the WM group, *only* the KGlobal one, a kcsrc file only has the group "Color Scheme"  hmpf...
    config->setGroup( "WM" );

    config->writeEntry("activeForeground", toSave->activeForeground, true, true);
    config->writeEntry("inactiveForeground", toSave->inactiveForeground, true, true);
    config->writeEntry("activeBackground", toSave->activeBackground, true, true);
    config->writeEntry("inactiveBackground", toSave->inactiveBackground, true, true);
    config->writeEntry("activeBlend", toSave->activeBlend, true, true);
    config->writeEntry("inactiveBlend", toSave->inactiveBlend, true, true);
    config->writeEntry("activeTitleBtnBg", toSave->activeTitleBtnBg, true, true);
    config->writeEntry("inactiveTitleBtnBg", toSave->inactiveTitleBtnBg, true, true);

    ////////////////////////////////////////////////////
     // KDE-1.x support
    KSimpleConfig *kconfig =
    new KSimpleConfig( QCString(::getenv("HOME")) + "/.kderc" );
    kconfig->setGroup( "General" );
    kconfig->writeEntry("background", toSave->background );
    kconfig->writeEntry("selectBackground", toSave->selectBackground );
    kconfig->writeEntry("foreground", toSave->foreground );
    kconfig->writeEntry("windowForeground", toSave->windowForeground );
    kconfig->writeEntry("windowBackground", toSave->windowBackground );
    kconfig->writeEntry("selectForeground", toSave->selectForeground );
    kconfig->sync();
    delete kconfig;
    /////////////////////////////////////////////
    config->setGroup("KDE");
    // write the color scheme filename and the contrast, default 7, otherwise from file
    config->writeEntry("colorScheme", toSave->colorFile,true, true);
    config->writeEntry("contrast", toSave->contrast, true, true);
    config->sync();

    QApplication::setOverrideCursor( waitCursor );
    QStringList args;
    args.append("style");
    kapp->kdeinitExecWait("kcminit2", args);
    QApplication::restoreOverrideCursor();
    QApplication::flushX();
    // color palette changes
    KIPC::sendMessageAll(KIPC::PaletteChanged);

  // background color changes
  KConfig kdesktop("kdesktoprc");
  kdesktop.setGroup("Desktop0"); // we only need to set one desktop

  kdesktop.writeEntry("BackgroundMode", toSave->bgMode);
  kdesktop.writeEntry("Color1", toSave->usrCol1);
  kdesktop.writeEntry("Color2", toSave->usrCol2);
  kdesktop.sync();
  kapp->dcopClient()->send("kdesktop", "KBackgroundIface", "configure()", "");
}

/** to be connected to the OS page. Catches
either KDE, CDE, win or mac and pre-sets the style.
 */
void KStylePage::presetStyle(const QString& style){
    kdDebug() << "KStylePage::presetStyle(): "<< style << endl;

    if(style=="KDE")
      klv_styles->setSelected(kde,true);
    else if(style=="CDE")
      klv_styles->setSelected(cde,true);
    else if(style=="win")
      klv_styles->setSelected(win,true);
    else if(style=="mac")
      klv_styles->setSelected(mac,true);
}
/** resets to KDE style as default */
void KStylePage::setDefaults(){
//      kde->setSelected(true);
}
/** No descriptions */
void KStylePage::getColors(colorSet *set, bool colorfile ){
    KConfig* config;
    // get the color scheme file and go to the color scheme group
    if(colorfile){
       KGlobal::dirs()->addResourceType("colors", KStandardDirs::kde_default("data")+"kdisplay2/color-schemes");
       // set the style
       if(kde->isSelected()){
           set->bgMode="VerticalGradient";
           set->usrCol1.setNamedColor ("#1E72A0");
           set->usrCol2.setNamedColor ("#C0C0C0");
           set->colorFile="<default>";
            kdDebug() << "KStylePage::getColors(): "<< set->colorFile << endl;
       }
       else if(cde->isSelected()){
           set->bgMode="Flat";
           set->usrCol1.setNamedColor("#718BA5");
           set->usrCol2.setNamedColor ("#C0C0C0");
           set->colorFile=locate("colors","SolarisCDE.kcsrc");
            kdDebug() << "KStylePage::getColors(): "<< set->colorFile << endl;
       }
       else if(win->isSelected()){
           set->bgMode="Flat";
           set->usrCol1.setNamedColor("#008183");
           set->usrCol2.setNamedColor ("#C0C0C0");
           set->colorFile=locate("colors","Windows2000.kcsrc");
            kdDebug() << "KStylePage::getColors(): "<< set->colorFile << endl;
       }
       else if(mac->isSelected()){
           set->bgMode="VerticalGradient";
           set->usrCol1.setNamedColor("#2A569D");
           set->usrCol2.setNamedColor("#6C8BB9");
           set->colorFile=locate("colors","EveX.kcsrc");
            kdDebug() << "KStylePage::getColors(): "<< set->colorFile << endl;
         }    // now comes the fun part, oh boy...
      set->contrast=7;
      config = new KSimpleConfig(set->colorFile, true);
      config->setGroup("Color Scheme");
    }
    else{  //
      KConfig kdesktop("kdesktoprc");
      kdesktop.setGroup("Desktop0");
      set->bgMode=kdesktop.readEntry("BackgroundMode", "VerticalGradient");
      QColor tmp1("#1E72A0");
      QColor tmp2("#C0C0C0");

      set->usrCol1=kdesktop.readColorEntry("Color1", &tmp1);
      set->usrCol2=kdesktop.readColorEntry("Color2", &tmp2);
      // write the color scheme filename and the contrast, default 7, otherwise from file
      config=KGlobal::config();
      config->setGroup("KDE");
      set->colorFile=config->readEntry("colorScheme", "<default>");
      set->contrast=config->readNumEntry("contrast", 7);
      config->setGroup( "General" );
       kdDebug() << "KStylePage::getColors(): "<< set->colorFile << endl;
    }

    set->foreground=config->readColorEntry( "foreground", &black );
    set->background=config->readColorEntry( "background", &widget );
    set->windowForeground=config->readColorEntry( "windowForeground", &black );
    kdDebug() << "KStylePage::getColors(): "<< set->windowForeground.name() << endl;
    set->windowBackground=config->readColorEntry( "windowBackground", &white );
    set->selectForeground=config->readColorEntry( "selectForeground", &white );
    set->selectBackground=config->readColorEntry( "selectBackground", &kde2Blue );
    set->buttonForeground=config->readColorEntry( "buttonForeground", &black );
    set->buttonBackground=config->readColorEntry( "buttonBackground", &button );
    set->linkColor=config->readColorEntry( "linkColor", &link );
    set->visitedLinkColor=config->readColorEntry( "visitedLinkColor", &visitedLink );

	// it's necessary to set the group, when reading from globalrc
	if(!colorfile)
		config->setGroup( "WM" );

    set->activeForeground=config->readColorEntry("activeForeground", &white);
    set->inactiveForeground=config->readColorEntry("inactiveForeground", &black);
    set->activeBackground=config->readColorEntry("activeBackground", &kde2Blue);
    set->inactiveBackground=config->readColorEntry("inactiveBackground", &widget);
    set->activeBlend=config->readColorEntry("activeBlend", &kde2Blue);
    set->inactiveBlend=config->readColorEntry("inactiveBlend", &widget);
    set->activeTitleBtnBg=config->readColorEntry("activeTitleBtnBg", &widget);
    set->inactiveTitleBtnBg=config->readColorEntry("inactiveTitleBtnBg", &widget);

}
#include "kstylepage.moc"
