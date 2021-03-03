#ifndef PABCONTACT_H 
#define PABCONTACT_H 

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif 

#include "entryeditorwidget.h"

class ContactEntry;
class QWidget;

class PabContactDialog : public ContactDialog
{
  Q_OBJECT

public:
  PabContactDialog( const QString & title, QWidget *parent, 
		    const char *name,
		    QString entryKey,
		    ContactEntry* entry,
		    bool modal = FALSE );
  virtual ~PabContactDialog();
 
signals:
  virtual void change( QString entryKey , ContactEntry *ce );

protected slots:
  virtual void accept();

protected:
  QString entryKey_; 
};

class PabNewContactDialog : public ContactDialog
{
  Q_OBJECT

public:
  PabNewContactDialog( const QString & title, QWidget *parent, 
		       const char *name, 
		       bool modal = FALSE );
  virtual ~PabNewContactDialog();

signals:
  virtual void add( ContactEntry* ce );

protected slots:
  virtual void accept();
};

#endif // PABWIDGET_H 
