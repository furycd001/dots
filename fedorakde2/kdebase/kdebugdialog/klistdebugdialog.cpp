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

// $Id: klistdebugdialog.cpp,v 1.5 2001/03/17 10:29:06 faure Exp $

#include "klistdebugdialog.h"
#include <kconfig.h>
#include <kapp.h>
#include <kdebug.h>
#include <qlayout.h>
#include <qscrollview.h>
#include <qvbox.h>
#include <klocale.h>

KListDebugDialog::KListDebugDialog( QStringList areaList, QWidget *parent, const char *name, bool modal )
  : KAbstractDebugDialog( parent, name, modal )
{
  setCaption(i18n("Debug Settings"));

  QVBoxLayout *lay = new QVBoxLayout( this, KDialog::marginHint(), KDialog::spacingHint() );
  QScrollView * scrollView = new QScrollView( this );
  lay->addWidget( scrollView );

  QVBox *box = new QVBox( scrollView->viewport() );
  scrollView->addChild( box );

  QStringList::Iterator it = areaList.begin();
  for ( ; it != areaList.end() ; ++it )
  {
      QString data = (*it).simplifyWhiteSpace();
      int space = data.find(" ");
      if (space == -1)
          kdError() << "No space:" << data << endl;

      QString areaNumber = data.left(space);
      //kdDebug() << areaNumber << endl;
      QCheckBox * cb = new QCheckBox( data, box, areaNumber.latin1() );
      boxes.append( cb );
  }

  buildButtons( lay );
  load();
  resize( 300, 400 );
}

void KListDebugDialog::load()
{
  QListIterator<QCheckBox> it ( boxes );
  for ( ; it.current() ; ++it )
  {
      pConfig->setGroup( (*it)->name() ); // Group name = debug area code = cb's name

      int setting = pConfig->readNumEntry( "InfoOutput", 2 );
      switch (setting) {
        case 4: // off
          (*it)->setChecked(false);
          break;
        case 2: //shell
          (*it)->setChecked(true);
          break;
        case 3: //syslog
        case 1: //msgbox
        case 0: //file
        default:
          (*it)->setNoChange();
          /////// Uses the triState capability of checkboxes
          ////// Note: it seems some styles don't draw that correctly (BUG)
          break;
      }
  }
}

void KListDebugDialog::save()
{
  QListIterator<QCheckBox> it ( boxes );
  for ( ; it.current() ; ++it )
  {
      pConfig->setGroup( (*it)->name() ); // Group name = debug area code = cb's name
      if ( (*it)->state() != QButton::NoChange )
      {
          int setting = (*it)->isChecked() ? 2 : 4;
          pConfig->writeEntry( "InfoOutput", setting );
      }
  }
  //sync done by main.cpp
}

void KListDebugDialog::activateArea( QCString area, bool activate )
{
  QListIterator<QCheckBox> it ( boxes );
  for ( ; it.current() ; ++it )
  {
      if ( area == (*it)->name()  // debug area code = cb's name
          || (*it)->text().find( QString::fromLatin1(area) ) != -1 ) // area name included in cb text
      {
          (*it)->setChecked( activate );
          return;
      }
  }
}

#include "klistdebugdialog.moc"
