/* This file is part of KDE PIM
   Copyright (C) 1999 Don Sanders <sanders@kde.org>

   License: BSD
*/

#ifndef CONTACTENTRYLIST_H 
#define CONTACTENTRYLIST_H 

#include <qtextstream.h>
#include <qlist.h>
#include <qstringlist.h>
#include <addressbook.h>
#include "contactentry.h"


class KabAPI;
//class AddressBook;
//class AddressBook::Entry;

typedef QDictIterator<ContactEntry> ContactEntryListIterator;

class ContactEntryList
{
public:
  ContactEntryList();
  ~ContactEntryList();

  void commit();
  void refresh();
  QString insert( ContactEntry *item );
  void unremove( const QString &key, ContactEntry *item );
  void remove( const QString &key );
  /** Removes all items in the trash from the list */
  void emptyTrash();
  ContactEntry* find( const QString &key );
  void replace( const QString &key, ContactEntry *item );
  QStringList keys() const;
  QDict<ContactEntry> getDict() const;
  
 protected:
  ContactEntry *KabEntryToContactEntry( AddressBook::Entry entry );
  AddressBook::Entry ContactEntryToKabEntry( ContactEntry *entry, AddressBook::Entry );

  KabAPI *addrBook;
  QStringList removedKeys;
  QDict<ContactEntry> ceDict;
};


/*
class ContactEntryList : public QDict<ContactEntry>
{
public:
  ContactEntryList();
  //  ContactEntryList( const QString &filename );
  //  QString key();
  QString insert( ContactEntry *item );
  void unremove( const QString &key, ContactEntry *item );
  void save();
  void load();

protected:
  long kkey;
  QList<ContactEntry> list;
};
*/


#endif
