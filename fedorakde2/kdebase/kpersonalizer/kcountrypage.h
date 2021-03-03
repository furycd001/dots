/***************************************************************************
                          kcountrypage.h  -  description
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

#ifndef KCOUNTRYPAGE_H
#define KCOUNTRYPAGE_H

#include "kcountrypagedlg.h"

class KLanguageButton;
class KLocaleAdvanced;
class QStringList;

/**Abstract class for the first wizard page. Sets the according selection on save()
  *@author Ralf Nolden
  */

class KCountryPage : public KCountryPageDlg  {
   Q_OBJECT
public:
    KCountryPage(QWidget *parent=0, const char *name=0);
    ~KCountryPage();

    void loadCountryList(KLanguageButton *combo);
    void loadLanguageList(KLanguageButton *combo, const QStringList &first);
    /** No descriptions */
    void save(KLanguageButton *comboCountry, KLanguageButton *comboLang);

    KLocaleAdvanced *locale;

    bool languageSet;

private:
    QStringList langs;

public slots:
    void setLangForCountry(int i);
};

#endif
