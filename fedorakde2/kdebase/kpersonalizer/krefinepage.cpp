/***************************************************************************
                          krefinepage.cpp  -  description
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
#include <qpushbutton.h>

#include <krun.h>
#include <kconfig.h>
#include <kstddirs.h>
#include <kapp.h>
#include <kglobal.h>
#include "krefinepage.h"


KRefinePage::KRefinePage(QWidget *parent, const char *name ) : KRefinePageDlg(parent,name) {
   px_finishSidebar->setPixmap(locate("data", "kpersonalizer/pics/step5.png"));
   connect( pb_kcontrol, SIGNAL(clicked()), this, SLOT(startKControl()) );
}
KRefinePage::~KRefinePage(){
}
/** starts kcontrol via krun when the user presses the
start control center button on page 5. */
void KRefinePage::startKControl(){
  KRun::runCommand("kcontrol2");
}
#include "krefinepage.moc"
