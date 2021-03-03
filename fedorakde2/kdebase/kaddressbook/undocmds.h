#ifndef UNDOCMDS_H
#define UNDOCMDS_H
// $Id: undocmds.h,v 1.1 2001/05/28 22:32:07 cschumac Exp $
//
// Commands for undo/redo functionality.

#include <qstring.h>
#include <qstringlist.h>

#include "undo.h"

class KAddressBookView;
class ContactEntry;

class PwDeleteCommand : public Command
{
public:
  PwDeleteCommand( KAddressBookView *pw, QString entryKey, ContactEntry *ce );
  virtual ~PwDeleteCommand();
  virtual QString name();
  virtual void undo();
  virtual void redo();

private:
  KAddressBookView* pw;
  QString entryKey;
  ContactEntry *ce;
};

// all commands need to be changed so that instead of referencing
// a pabListViewItem they reference an entry key
class PwPasteCommand : public Command
{
public:
  PwPasteCommand( KAddressBookView *pw, QString clipboard );
  virtual QString name();
  virtual void undo();
  virtual void redo();

private:
  KAddressBookView *pw;
  QStringList keyList;
  QString clipboard;
};

class PwCutCommand : public Command
{
public:
  PwCutCommand( KAddressBookView *pw );
  virtual QString name();
  virtual void undo();
  virtual void redo();

private:
  KAddressBookView *pw;
  QStringList keyList;
  QString clipText;
  QString oldText;
};

class PwNewCommand : public Command
{
public:
  PwNewCommand( KAddressBookView *pw, ContactEntry *ce );
  virtual QString name();
  virtual void undo();
  virtual void redo();

private:
  KAddressBookView *pw;
  QString entryKey;
  ContactEntry *ce;
};

class PwEditCommand : public Command
{
public:
  PwEditCommand( KAddressBookView *pw, 
		 QString entryKey,
		 ContactEntry *oldCe, 
		 ContactEntry *newCe );
  virtual ~PwEditCommand();
  virtual QString name();
  virtual void undo();
  virtual void redo();

private:
  KAddressBookView *pw;
  QString entryKey;
  ContactEntry *oldCe;
  ContactEntry *newCe;
};

#endif
