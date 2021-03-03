/***************************************************************************
                          kcmkonsole.cpp - control module for konsole
                             -------------------
    begin                : mar apr 17 16:44:59 CEST 2001
    copyright            : (C) 2001 by Andrea Rizzi
    email                : rizzi@kde.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "kcmkonsole.h"
#include "kcmkonsoledialog.h"

#include <kfontdialog.h>
#include <kdialog.h>
#include <qlayout.h>
#include <kglobal.h>
#include <klocale.h>
#include <kconfig.h>
#include <kdebug.h>
#include "schemaeditor.h"

#include <qcheckbox.h>
#include <qcombobox.h>
#include <qspinbox.h>

KCMKonsole::KCMKonsole(QWidget * parent, const char *name)
:KCModule(parent, name)
{
    QVBoxLayout *topLayout = new QVBoxLayout(this);
    dialog = new KCMKonsoleDialog(this);
    dialog->SpinBox1->setMinValue(1);
    dialog->SpinBox1->setLineStep(100);
    dialog->show();
    topLayout->add(dialog);
    load();


    connect(dialog->fontPB, SIGNAL(clicked()), this, SLOT(setupFont()));
    connect(dialog->fullScreenCB,SIGNAL(toggled(bool)),this,SLOT(configChanged()));
    connect(dialog->showMenuBarCB,SIGNAL(toggled(bool)),this,SLOT(configChanged()));
    connect(dialog->warnCB,SIGNAL(toggled(bool)),this,SLOT(configChanged()));
    connect(dialog->scrollBarCO,SIGNAL(activated(int)),this,SLOT(configChanged()));
    connect(dialog->showFrameCB,SIGNAL(toggled(bool)),this,SLOT(configChanged()));
    connect(dialog->terminalLE,SIGNAL(textChanged(const QString &)),this,SLOT(configChanged()));
    connect(dialog->terminalCB,SIGNAL(toggled(bool)),this,SLOT(configChanged()));
    connect(dialog->historyCB,SIGNAL(toggled(bool)),this,SLOT(configChanged()));

}

void KCMKonsole::load()
{

    KConfig *config = new KConfig("konsolerc", false, true);
    config->setDesktopGroup();


    dialog->historyCB->setChecked(config->readBoolEntry("historyenabled",true));

    dialog->fullScreenCB->setChecked(config->readBoolEntry("Fullscreen",false));
    dialog->showMenuBarCB->setChecked(config->readEntry("MenuBar","Enabled") == "Enabled");
    dialog->warnCB->setChecked(config->readBoolEntry("WarnQuit",true));
    dialog->showFrameCB->setChecked(config->readBoolEntry("has frame",true));
    dialog->scrollBarCO->setCurrentItem(config->readNumEntry("scrollbar",1));
    dialog->fontCO->setCurrentItem(config->readNumEntry("font",3));
    currentFont = config->readFontEntry("defaultfont");
    dialog->SpinBox1->setValue(config->readNumEntry("history",1000));

    dialog->SchemaEditor1->setSchema(config->readEntry("schema"));

    config = new KConfig("kdeglobals", false, true);
    config->setGroup("General");
    dialog->terminalLE->setText(config->readEntry("TerminalApplication","konsole2"));
    dialog->terminalCB->setChecked(config->readEntry("TerminalApplication","konsole2")!="konsole2");

    emit changed(false);
}

void KCMKonsole::load(const QString & /*s*/)
{

}

void KCMKonsole::setupFont()
{

    // Example string, may be nice somthing like "[root@localhost]$ rm / -rf"
    QString example = i18n("[root@localhost]$ ");
    if (KFontDialog::getFontAndText(currentFont, example))
    {
	dialog->fontCO->setCurrentItem(0);
        configChanged();
    }
}



void KCMKonsole::configChanged()
{
emit changed(true);
}

void KCMKonsole::save()
{

    KConfig *config = new KConfig("konsolerc", false, true);
    config->setDesktopGroup();


    config->writeEntry("historyenabled", dialog->historyCB->isChecked());
    config->writeEntry("history", dialog->SpinBox1->text());
    config->writeEntry("Fullscreen", dialog->fullScreenCB->isChecked());
    config->writeEntry("MenuBar", dialog->showMenuBarCB->isChecked()? "Enabled" : "Disabled");
    config->writeEntry("WarnQuit", dialog->warnCB->isChecked());
    config->writeEntry("has frame", dialog->showFrameCB->isChecked());
    config->writeEntry("scrollbar", dialog->scrollBarCO->currentItem());
    config->writeEntry("font", dialog->fontCO->currentItem());

    config->writeEntry("defaultfont", currentFont);

    config->writeEntry("schema", dialog->SchemaEditor1->schema());

    config->sync();		//is it necessary ?

    config = new KConfig("kdeglobals", false, true);
    config->setGroup("General");
    config->writeEntry("TerminalApplication",dialog->terminalCB->isChecked()?dialog->terminalLE->text():"konsole2");
    
    config->sync();		//is it necessary ?

    emit changed(false);

}

void KCMKonsole::defaults()
{


    dialog->historyCB->setChecked(true);

    dialog->fullScreenCB->setChecked(false);
    dialog->showMenuBarCB->setChecked(true);
    dialog->warnCB->setChecked(true);
    dialog->showFrameCB->setChecked(true);
    dialog->scrollBarCO->setCurrentItem(true);
    dialog->terminalCB->setChecked(false);
    
    // Check if -e is needed, I do not think so
    dialog->terminalLE->setText("xterm");  //No need for i18n
    dialog->fontCO->setCurrentItem(4);

    configChanged();

}

QString KCMKonsole::quickHelp() const
{
    return i18n("<h1>konsole</h1> With this module you can configure konsole, the KDE terminal"
		" application. In this module you can configure the generic konsole options, "
		"that you can configure using the RMB too, and you can edit the konsole schema.");
}


const KAboutData * KCMKonsole::aboutData() const
{
 
 KAboutData *ab=new KAboutData( "kcmkonsole", I18N_NOOP("KCM Konsole"),
    "0.2",I18N_NOOP("KControl module for konsole configuration"), KAboutData::License_GPL,
    "(c) 2001, Andrea Rizzi", 0, 0, "rizzi@kde.org");
 
  ab->addAuthor("Andrea Rizzi",0, "rizzi@kde.org");
 return ab;
      
}

extern "C" {
    KCModule *create_konsole(QWidget * parent, const char *name) {
	KGlobal::locale()->insertCatalogue("kcmkonsole");
	return new KCMKonsole(parent, name);
    };
}



#include "kcmkonsole.moc"
