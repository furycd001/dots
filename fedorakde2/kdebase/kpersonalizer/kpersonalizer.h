/***************************************************************************
                          kpersonalizer.h  -  description
                             -------------------
    begin                : Die Mai 22 17:24:18 CEST 2001
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

#ifndef KPERSONALIZER_H
#define KPERSONALIZER_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <kapp.h>
#include <kwizard.h>

#include "kcountrypage.h"
#include "kospage.h"
#include "keyecandypage.h"
#include "kstylepage.h"
#include "krefinepage.h"


class KLanguageCombo;
class KLocaleAdvanced;

/** KPersonalizer is the base class of the porject */
class KPersonalizer : public KWizard
{
  Q_OBJECT
  public:
    /** construtor */
    KPersonalizer(QWidget* parent=0, const char *name=0);
    /** destructor */
    ~KPersonalizer();

    virtual void next();
    virtual void back();

public slots: // Public slots
  /** calls the save() functions of the abstract page classes. */
  virtual void applySettings();
  /** sets ktip's rc file to true and starts ktip */
  void runKTip();
  /** calls all save functions after resetting all features/ OS/ theme selections to KDE default */
  void setDefaults();

private:
  KCountryPage* countrypage;
  KOSPage* ospage;
  KEyeCandyPage* eyecandy;
  KStylePage* stylepage;
  KRefinePage* refinepage;
  KLocaleAdvanced* locale;
  bool ktip, os_dirty, eye_dirty, style_dirty;
public slots: // Public slots

  /** the cancel button is connected to the reject() slot of QDialog,
   *  so we have to reimplement this here to add a dialogbox to
   *  ask if we really want to quit the wizard.
   */
  void reject();

  /** maybe call a dialog that the wizard has finished.
  * Calls applySettings() to save the current selection.
  */
  void accept();


protected: // Protected methods
  // the close button on the titlebar sets e->accept() which we don´t want.
  virtual void closeEvent(QCloseEvent*);
  bool askClose();


};

#endif
