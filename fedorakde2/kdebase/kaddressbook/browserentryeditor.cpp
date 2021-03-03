#include "browserentryeditor.h"
#include "contactentry.h"

////////////////////////////
// PabContactDialog Methods

PabContactDialog::PabContactDialog( const QString & title, QWidget *parent, 
				    const char *name, 
				    QString entryKey,
				    ContactEntry *entry,
				    bool modal )
  : ContactDialog( title, parent, name, new ContactEntry( *entry ) , modal ),
    entryKey_( entryKey )
{
}

PabContactDialog::~PabContactDialog()
{
  delete ce;
}

void PabContactDialog::accept()
{
  emit change( entryKey_, ce );
  ContactDialog::accept();
}

////////////////////////////
// PabNewContactDialog Methods

PabNewContactDialog::PabNewContactDialog( const QString & title, QWidget *parent, 
					  const char *name, 
					  bool modal )
  : ContactDialog( title, parent, name, new ContactEntry(), modal )
{}

PabNewContactDialog::~PabNewContactDialog()
{
  delete ce;
}

void PabNewContactDialog::accept()
{
  emit add( ce );
  ContactDialog::accept();
}
#include "browserentryeditor.moc"
