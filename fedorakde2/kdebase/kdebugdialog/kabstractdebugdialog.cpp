/* This file is part of the KDE libraries
    Copyright (C) 2000 David Faure <faure@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
*/

#include "kabstractdebugdialog.h"
#include <kconfig.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <kapp.h>
#include <klocale.h>

KAbstractDebugDialog::KAbstractDebugDialog( QWidget *parent, const char *name, bool modal )
    : KDialog( parent, name, modal )
{
    pConfig = new KConfig( "kdebugrc" );
}

KAbstractDebugDialog::~KAbstractDebugDialog()
{
    delete pConfig;
}

void KAbstractDebugDialog::buildButtons( QVBoxLayout * topLayout )
{
  QHBoxLayout *hbox = new QHBoxLayout( KDialog::spacingHint() );
  topLayout->addLayout( hbox );
  pHelpButton = new QPushButton( i18n("&Help"), this );
  hbox->addWidget( pHelpButton );
  hbox->addStretch(10);
  pOKButton = new QPushButton( i18n("&OK"), this );
  hbox->addWidget( pOKButton );
  pCancelButton = new QPushButton( i18n("&Cancel"), this );
  hbox->addWidget( pCancelButton );

  int w1 = pHelpButton->sizeHint().width();
  int w2 = pOKButton->sizeHint().width();
  int w3 = pCancelButton->sizeHint().width();
  int w4 = QMAX( w1, QMAX( w2, w3 ) );

  pHelpButton->setFixedWidth( w4 );
  pOKButton->setFixedWidth( w4 );
  pCancelButton->setFixedWidth( w4 );

  connect( pHelpButton, SIGNAL( clicked() ), SLOT( slotShowHelp() ) );
  connect( pOKButton, SIGNAL( clicked() ), SLOT( accept() ) );
  connect( pCancelButton, SIGNAL( clicked() ), SLOT( reject() ) );
}

void KAbstractDebugDialog::slotShowHelp()
{
  if (kapp)
    kapp->invokeHTMLHelp( "kdelibs/kdebug/index.html" );
}

#include "kabstractdebugdialog.moc"
