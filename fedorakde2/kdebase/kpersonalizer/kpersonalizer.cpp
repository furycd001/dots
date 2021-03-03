/***************************************************************************
                          kpersonalizer.cpp  -  description
                             -------------------
    begin                : Die Mai 22 17:24:18 CEST 2001
    copyright            : (C) 2001 by Ralf Nolden
    email                : nolden@kde.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include <qpushbutton.h>

#include <qlabel.h>
#include <qstring.h>
#include <qstringlist.h>

#include <ksimpleconfig.h>
#include <kglobal.h>
#include <kstddirs.h>
#include <klocale.h>
#include <kapp.h>
#include <klistview.h>
#include <krun.h>
#include <kmessagebox.h>
#include <kconfig.h>

#include <stdlib.h>

#include <kdebug.h>

#include "klocaleadv.h"

#include "kpersonalizer.h"
#include "kpersonalizer.moc"


KPersonalizer::KPersonalizer(QWidget *parent, const char *name)
    : KWizard(parent, name, true)
{
    // first, reset the startup from true (see desktop file in share/autostart) to false
    setCaption(kapp->caption());
    kapp->config()->setGroup("General");
    if(kapp->config()->readBoolEntry("FirstLogin", true))
      ktip=true;
    else
      ktip=false;
    os_dirty = eye_dirty = style_dirty=false;
    kapp->config()->writeEntry("FirstLogin", false);
    kapp->config()->sync();

   countrypage= new KCountryPage(this);
   addPage( countrypage, i18n( "Step 1: Introduction" ) );

   ospage= new KOSPage(this);
   addPage(ospage, i18n( "Step 2: I want it My Way ..." ) );

   eyecandy= new KEyeCandyPage(this);
   addPage( eyecandy, i18n( "Step 3: Eyecandy-O-Meter" ) );

   stylepage= new KStylePage(this);
   addPage( stylepage, i18n( "Step 4: Everybody loves Themes" ) );

   refinepage=new KRefinePage(this);
   addPage( refinepage, i18n( "Step 5: Time to Refine" ) );

   cancelButton()->setText(i18n("S&kip Wizard"));

   setFinishEnabled(QWizard::page(4), true);

   locale = new KLocaleAdvanced("kpersonalizer");
   locale->setLanguage("C");

   connect(ospage, SIGNAL(selectedOS(const QString&)), stylepage, SLOT(presetStyle(const QString& )));

}

KPersonalizer::~KPersonalizer()
{
}


void KPersonalizer::next()
{
    if(currentPage()==countrypage)
      countrypage->save(countrypage->cb_country, countrypage->cb_language);
    else if(currentPage()==ospage){
      os_dirty=true;  // set the dirty flag, changes done that need reverting
      ospage->save();
    }
    else if(currentPage()==eyecandy){
      eye_dirty=true;  // set the dirty flag, changes done that need reverting
      eyecandy->save();
    }
    else if(currentPage()==stylepage){
      style_dirty=true;  // set the dirty flag, changes done that need reverting
      stylepage->save();
    }
    QWizard::next();

}

void KPersonalizer::back()
{
    QWizard::back();
}

/** calls the save() functions of the abstract page classes. */
void KPersonalizer::applySettings(){
//    ospage->save();
//    eyecandy->save();
}

bool KPersonalizer::askClose(){
  QString text;
	text = i18n("Do you really want to abort the personalizer?\n"
                      "To return to personalizer, please press 'Cancel'.\n"
                      "'Keep' will keep all changes you made so far in the personalizer.\n"
	                    "'Dismiss' will revert ALL settings to the KDE preset default values,\n"
	                    "respectively as they were before the wizard started,\n"
	                    "except your country and language selection.\n\n"
	                    "If you want to restart this wizard, select\n"
	                    "K->System->Settings Wizard.");
    int status = KMessageBox::warningYesNoCancel(this,  text, i18n("Really quit?"), i18n("&Keep"), i18n("&Dismiss"));
    if(status==KMessageBox::Yes){
      return true;
		}
		if(status==KMessageBox::No){
      setDefaults();
			return true;
		}
    else
      return false;
}
/** the cancel button is connected to the reject() slot of QDialog,
 *  so we have to reimplement this here to add a dialogbox to ask if we
 *  really want to quit the wizard.
 */
void KPersonalizer::reject(){
  if (askClose()){
    runKTip();
    exit(0);
  }
}

void KPersonalizer::closeEvent(QCloseEvent* e){
  if (askClose()){
      	exit(0);
  }
  else
       e->ignore();
}

/** maybe call a dialog that the wizard has finished.
  * Calls applySettings() to save the current selection.
  * Call KTip afterwards.
  */
void KPersonalizer::accept(){
  applySettings();
  runKTip();
  exit(0);
}

/** calls all save functions after resetting all features/ OS/ theme selections to KDE default */
void KPersonalizer::setDefaults(){

// TODO: KCountryPage Default (maybe ? The user may need his mother language anyway

  if(os_dirty)
    ospage->save(false);
  if(eye_dirty)
    eyecandy->save(false);
  if(style_dirty)
    stylepage->save(false);
}


/** sets ktip's rc file to true and starts ktip */
void KPersonalizer::runKTip(){
  if(ktip){ // only run if we set this to true in the constructor. then kpersonalizer didn't run before
	// set the kdewizardrc file entry to true. The global one is set to false for starting kpersonalizer
	KConfig ktiprc("kdewizardrc");
    ktiprc.setGroup("General");
    ktiprc.writeEntry("TipsOnStart", true);

    KRun::runCommand("ktip");
  }
}
