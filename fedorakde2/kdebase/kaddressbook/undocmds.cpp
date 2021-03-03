// $Id: undocmds.cpp,v 1.2 2001/07/05 15:32:21 mlaurent Exp $

#include <qtextstream.h>
#include <qapp.h>
#include <qclipboard.h>

#include <klocale.h>
#include <kdebug.h>

#include "kaddressbookview.h"
#include "contactentry.h"
#include "contactentrylist.h"
#include "contactlistview.h"

#include "undocmds.h"

/////////////////////////////////
// PwDelete Methods

PwDeleteCommand::PwDeleteCommand( KAddressBookView *pw,
				  QString entryKey,
				  ContactEntry *ce )
  : pw( pw ), entryKey( entryKey ), ce( new ContactEntry( *ce ))
{
  redo();
}

PwDeleteCommand::~PwDeleteCommand()
{
  delete ce;
}

QString PwDeleteCommand::name()
{
  return i18n( "Delete" );
}

void PwDeleteCommand::undo()
{
  ContactEntryList* cel = pw->contactEntryList();
  cel->unremove( entryKey, new ContactEntry( *ce ));
  ContactListViewItem *plvi;
  plvi = new ContactListViewItem( entryKey, pw->listView(), pw->fields() );
  plvi->refresh();
  pw->listView()->resort();
  pw->listView()->setCurrentItem( plvi );
}

void PwDeleteCommand::redo()
{
  ContactEntryList *cel = pw->contactEntryList();
  cel->remove( entryKey );
  delete pw->listView()->getItem( entryKey );
}

/////////////////////////////////
// PwPaste Methods

PwPasteCommand::PwPasteCommand( KAddressBookView *pw, QString clipboard )
{
  this->pw = pw;
  this->clipboard = clipboard;

  QTextIStream clipStream( &clipboard );
  ContactEntryList* cel = pw->contactEntryList();
  while (!clipStream.eof()) {
    ContactEntry *newEntry = new ContactEntry( clipStream );
    QString key = cel->insert( newEntry );
    keyList.append( key );
    ContactListViewItem* plvi = pw->addEntry( key );
    pw->listView()->setSelected( plvi, true );
  }
}

QString PwPasteCommand::name()
{
  return i18n( "Paste" );
}

void PwPasteCommand::undo()
{
  ContactEntryList* cel = pw->contactEntryList();
  QStringList::Iterator it;
  for( it = keyList.begin(); it != keyList.end(); ++it ) {
    cel->remove( *it );
    delete pw->listView()->getItem( *it );
  }
}

void PwPasteCommand::redo()
{
  QTextIStream clipStream( &clipboard );
  ContactEntryList* cel = pw->contactEntryList();
  QStringList::Iterator it;
  for( it = keyList.begin(); it != keyList.end(); ++it ) {
    if (clipStream.eof())
      break;
    ContactEntry *newEntry = new ContactEntry( clipStream );
    cel->unremove( *it, newEntry );
    ContactListViewItem* plvi = pw->addEntry( *it );
    pw->listView()->setSelected( plvi, true );
  }
  pw->listView()->resort();
}

/////////////////////////////////
// PwNew Methods

PwNewCommand::PwNewCommand( KAddressBookView *pw, ContactEntry *ce )
  : pw( pw ), ce( ce )
{
  ContactEntryList* cel = pw->contactEntryList();
  entryKey = cel->insert( new ContactEntry( *ce ));
  ContactListViewItem *plvi;
  plvi = new ContactListViewItem( entryKey, pw->listView(), pw->fields() );
  plvi->refresh();
  pw->listView()->resort();
}

QString PwNewCommand::name()
{
  return i18n( "New Entry" );
}

void PwNewCommand::undo()
{
  ContactEntryList *cel = pw->contactEntryList();
  ContactEntry *tempce = cel->find( entryKey );
  if (!tempce) { // Another process deleted it already(!)
      kdDebug()<< "PwNewCommand::undo() Associated ContactEntry not found.\n";
      kdDebug()<< "Unable to undo insert\n";
  }
  else
    ce = new ContactEntry( *tempce );
  ContactListViewItem *plvi = pw->listView()->getItem( entryKey );
  if (plvi)
    delete plvi;
  else // Should never happen
      kdDebug()<< "PwNewCommand::undo() missing listViewItem.\n";
  cel->remove( entryKey );
}

void PwNewCommand::redo()
{
  ContactEntryList* cel = pw->contactEntryList();
  cel->unremove( entryKey, new ContactEntry( *ce ));
  ContactListViewItem *plvi;
  plvi = new ContactListViewItem( entryKey, pw->listView(), pw->fields() );
  plvi->refresh();
  pw->listView()->resort();
}

/////////////////////////////////
// PwEdit Methods

PwEditCommand::PwEditCommand( KAddressBookView *pw,
			      QString entryKey,
			      ContactEntry *oldCe,
			      ContactEntry *newCe )
{
  this->pw = pw;
  this->entryKey = entryKey;
  this->oldCe = new ContactEntry( *oldCe );
  this->newCe = new ContactEntry( *newCe );
  redo();
}

PwEditCommand::~PwEditCommand()
{
  delete oldCe;
  delete newCe;
}

QString PwEditCommand::name()
{
  return i18n( "Entry Edit" );
}

void PwEditCommand::undo()
{
  ContactEntryList *cel = pw->contactEntryList();
  cel->replace( entryKey, new ContactEntry( *oldCe ));

  ContactListViewItem *plvi = pw->listView()->getItem( entryKey );
  if (plvi)
    plvi->refresh();
  delete new QListViewItem( plvi->parent() ); //force resort
  pw->listView()->resort();  //grossly inefficient?
}

void PwEditCommand::redo()
{
  ContactEntryList *cel = pw->contactEntryList();
  cel->replace( entryKey, new ContactEntry( *newCe ));

  ContactListViewItem *plvi = pw->listView()->getItem( entryKey );
  if (plvi)
    plvi->refresh();
  delete new QListViewItem( plvi->parent() ); //force resort
  pw->listView()->resort();  //grossly inefficient?
}

/////////////////////////////////
// PwCut Methods

PwCutCommand::PwCutCommand( KAddressBookView *pw )
{
  this->pw = pw;
  QTextOStream clipStream( &clipText );
  ContactListView *listView = pw->listView();
  QListViewItem *item;
  ContactEntryList *cel = pw->contactEntryList();
  for(item = listView->firstChild(); item; item = item->itemBelow()) {
    if (!listView->isSelected( item ))
      continue;
    ContactListViewItem *plvi = dynamic_cast< ContactListViewItem* >(item);
    if (!plvi)
      continue;
    QString entryKey = plvi->entryKey();
    ContactEntry *ce = plvi->getEntry();
    if (!ce)
      continue;
    ce->save( clipStream );
    cel->remove( entryKey );
    delete plvi;
    keyList.append( entryKey );
  }
  QClipboard *cb = QApplication::clipboard();
  oldText = cb->text();
  cb->setText( clipText );
}

QString PwCutCommand::name()
{
  return i18n( "Cut" );
}

void PwCutCommand::undo()
{
  QTextIStream clipStream( &clipText );
  ContactEntryList* cel = pw->contactEntryList();
  QStringList::Iterator it;
  for( it = keyList.begin(); it != keyList.end(); ++it ) {
    if (clipStream.eof())
      break;
    ContactEntry *newEntry = new ContactEntry( clipStream );
    cel->unremove( *it, newEntry );
    ContactListViewItem* plvi = pw->addEntry( *it );
    pw->listView()->setSelected( plvi, true );
  }
  pw->listView()->resort();
  QClipboard *cb = QApplication::clipboard();
  cb->setText( oldText );
}

void PwCutCommand::redo()
{
  ContactEntryList* cel = pw->contactEntryList();
  QStringList::Iterator it;
  for( it = keyList.begin(); it != keyList.end(); ++it ) {
    delete pw->listView()->getItem( *it );
    cel->remove( *it );
  }
  QClipboard *cb = QApplication::clipboard();
  cb->setText( clipText );
}
