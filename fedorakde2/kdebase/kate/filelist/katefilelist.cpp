/***************************************************************************
                          katefilelist.cpp  -  description
                             -------------------
    begin                : Mon Feb 5 2001
    copyright            : (C) 2001 by Christoph Cullmann
    email                : crossfire@babylon2k.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "katefilelist.h"
#include "katefilelist.moc"

#include "../document/katedocument.h"
#include "../document/katedocmanager.h"
#include "../view/kateview.h"
#include "../view/kateviewmanager.h"
#include "../mainwindow/katemainwindow.h"

#include <qapplication.h>

#include <kiconloader.h>
#include <klocale.h>

KateFileList::KateFileList (KateDocManager *_docManager, KateViewManager *_viewManager, QWidget * parent, const char * name ):  KListBox (parent, name)
{
  docManager = _docManager;
  viewManager = _viewManager;
  tooltip = new KFLToolTip( this );

  for (uint i = 0; i < docManager->docCount(); i++)
  {
    slotDocumentCreated (docManager->nthDoc(i));
    slotModChanged (docManager->nthDoc(i));
  }

  connect(docManager,SIGNAL(documentCreated(KateDocument *)),this,SLOT(slotDocumentCreated(KateDocument *)));
  connect(docManager,SIGNAL(documentDeleted(uint)),this,SLOT(slotDocumentDeleted(uint)));

  connect(this,SIGNAL(highlighted(QListBoxItem *)),this,SLOT(slotActivateView(QListBoxItem *)));
  connect(this,SIGNAL(selected(QListBoxItem *)), this,SLOT(slotActivateView(QListBoxItem *)));

  connect(viewManager,SIGNAL(viewChanged()), this,SLOT(slotViewChanged()));

  connect(this,SIGNAL(rightButtonPressed ( QListBoxItem *, const QPoint & )), this,SLOT(slotMenu ( QListBoxItem *, const QPoint & )));
}

KateFileList::~KateFileList ()
{
}

void KateFileList::slotDocumentCreated (KateDocument *doc)
{
  insertItem (new KateFileListItem (doc->docID(), SmallIcon("null"), doc->docName()) );
  connect(doc,SIGNAL(modStateChanged(KateDocument *)),this,SLOT(slotModChanged(KateDocument *)));
  connect(doc,SIGNAL(nameChanged(KateDocument *)),this,SLOT(slotNameChanged(KateDocument *)));
}

void KateFileList::slotDocumentDeleted (uint docID)
{
  for (uint i = 0; i < count(); i++)
  {
    if (((KateFileListItem *) item (i)) ->docID() == docID)
    {
      if (count() > 1)
        removeItem( i );
      else
        clear();
    }
  }
}

void KateFileList::slotActivateView( QListBoxItem *item )
{
  viewManager->activateView( ((KateFileListItem *)item)->docID() );
}

void KateFileList::slotModChanged (KateDocument *doc)
{
  if (!doc) return;

  uint i;

  if( doc->isModified() )
  {
    for (i = 0; i < count(); i++)
    {
      if (((KateFileListItem *) item (i)) ->docID() == doc->docID())
      {
        ((KateFileListItem *)item(i))->setPixmap(SmallIcon("modified"));
        ((KateFileListItem *)item(i))->setBold(true);

        triggerUpdate(false);
        break;
      }
    }
  }
  else
  {
    for (i = 0; i < count(); i++)
    {
      if (((KateFileListItem *) item (i)) ->docID() == doc->docID())
      {
        ((KateFileListItem *)item(i))->setPixmap(SmallIcon("null"));
        ((KateFileListItem *)item(i))->setBold(false);

        triggerUpdate(false);
        break;
      }
    }
  }
}

void KateFileList::slotNameChanged (KateDocument *doc)
{
  if (!doc) return;

  for (uint i = 0; i < count(); i++)
  {
    if (((KateFileListItem *) item (i)) ->docID() == doc->docID())
    {
      ((KateFileListItem *)item(i))->setText(doc->docName());
      triggerUpdate(false);
      break;
    }
  }
}

void KateFileList::slotViewChanged ()
{
  if (!viewManager->activeView()) return;

  KateView *view = viewManager->activeView();

  for (uint i = 0; i < count(); i++)
  {
    if (((KateFileListItem *) item (i)) ->docID() == ((KateDocument *)view->doc())->docID())
    {
      setCurrentItem (i);
      if ( !isSelected( item(i) ) )
        setSelected( i, true );
      break;
    }
  }
}

void KateFileList::slotMenu ( QListBoxItem *item, const QPoint &p )
{
  if (!item)
    return;

  QPopupMenu *menu = (QPopupMenu*) ((KMainWindow *)topLevelWidget ())->factory()->container("filelist_popup", (KMainWindow *)topLevelWidget ());
  menu->exec(p);
}

void KateFileList::tip( const QPoint &p, QRect &r, QString &str )
{
  KateFileListItem *i = (KateFileListItem*)itemAt( p );
  r = itemRect( i );

  if( i != NULL && r.isValid() )
    str = docManager->docWithID(i->docID())->url().prettyURL();
  else
    str = "";
}

KateFileListItem::KateFileListItem( uint docID, const QPixmap &pix, const QString& text): QListBoxItem()
{
  _bold=false;
  myDocID = docID;
  setPixmap(pix);
  setText( text );
}

KateFileListItem::~KateFileListItem()
{
}

uint KateFileListItem::docID ()
{
  return myDocID;
}


void KateFileListItem::setText(const QString &text)
{
  QListBoxItem::setText(text);
}

void KateFileListItem::setPixmap(const QPixmap &pixmap)
{
  pm=pixmap;
}

void KateFileListItem::setBold(bool bold)
{
  bold=bold;
}

int KateFileListItem::height( const QListBox* lb ) const
{
  int h;

  if ( text().isEmpty() )
    h = pm.height();
  else
    h = QMAX( pm.height(), lb->fontMetrics().lineSpacing() + 1 );

  return QMAX( h, QApplication::globalStrut().height() );
}

int KateFileListItem::width( const QListBox* lb ) const
{
  if ( text().isEmpty() )
    return QMAX( pm.width() + 6, QApplication::globalStrut().width() );

  return QMAX( pm.width() + lb->fontMetrics().width( text() ) + 6, QApplication::globalStrut().width() );
}

void KateFileListItem::paint( QPainter *painter )
{
  painter->drawPixmap( 3, 0, pm );
  QFont f=painter->font();
  f.setBold(_bold);
  painter->setFont(f);

  if ( !text().isEmpty() )
  {
    QFontMetrics fm = painter->fontMetrics();
    int yPos;                       // vertical text position

    if ( pm.height() < fm.height() )
      yPos = fm.ascent() + fm.leading()/2;
    else
      yPos = pm.height()/2 - fm.height()/2 + fm.ascent();

    painter->drawText( pm.width() + 5, yPos, text() );
  }
}

/////////////////////////////////////////////////////////////////////
// KateFileList::KFLToolTip implementation

KateFileList::KFLToolTip::KFLToolTip( QWidget *parent )
  : QToolTip( parent )
{
}

void KateFileList::KFLToolTip::maybeTip( const QPoint &p )
{
  QString str;
  QRect r;

  ((KateFileList*)parentWidget())->tip( p, r, str );

  if( !str.isEmpty() && r.isValid() )
    tip( r, str );
}

