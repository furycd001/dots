/***************************************************************************
                          krefinepage.h  -  description
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

#ifndef KREFINEPAGE_H
#define KREFINEPAGE_H

#include "krefinepagedlg.h"

/**Abstract class for the final settings refinement. Intends to call KControl for now
  *@author Ralf Nolden
  */

class KRefinePage : public KRefinePageDlg  {
   Q_OBJECT
public: 
	KRefinePage(QWidget *parent=0, const char *name=0);
	~KRefinePage();
public slots: // Public slots
  /** starts kcontrol via krun when the user presses the
start control center button on page 5. */
  void startKControl();
};

#endif
