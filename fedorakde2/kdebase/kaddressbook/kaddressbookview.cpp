// $Id: kaddressbookview.cpp,v 1.5 2001/07/13 07:31:38 mlaurent Exp $

#include <stdlib.h> // for atoi

#include <qtabwidget.h>
#include <qlayout.h>
#include <qvbox.h>
#include <qgroupbox.h>
#include <qbuttongroup.h>
#include <qlistbox.h>
#include <qwidget.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qpixmap.h>
#include <qimage.h>
#include <qcombobox.h>
#include <qtooltip.h>
#include <qdialog.h>
#include <qheader.h>
#include <qapplication.h>
#include <qclipboard.h>
#include <qcheckbox.h>
#include <qradiobutton.h>
#include <qdragobject.h>
#include <qevent.h>
#include <qurl.h>

#include <kurl.h>
#include <kdebug.h>
#include <klocale.h>
#include <kconfig.h>
#include <kapp.h>
#include <kcolorbtn.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <kurlrequester.h>
#include <klineedit.h>

#include "kaddressbookview.h"
#include "entryeditorwidget.h"
#include "viewoptions.h"
#include "selectfields.h"
#include "attributes.h"
#include "contactentry.h"
#include "contactentrylist.h"
#include "browserentryeditor.h"
#include "undocmds.h"
#include "contactlistview.h"

void KAddressBookView::selectNames( QStringList newFields )
{
  field.clear();
  fieldWidth.clear();
  if (newFields.count() == 0) {
    newFields += "X-FileAs";
    newFields += "EMAIL";
    newFields += "X-BusinessPhone";
    newFields += "X-HomePhone";
  }

  QStringList::Iterator it;
  for(it = newFields.begin(); it != newFields.end(); ++it)
  {
    field += *it;
    if (*it == "X-FileAs")
      fieldWidth += 180;
    else if (*it == "EMAIL")
      fieldWidth += 160;
    else
      fieldWidth += 120;
  }
}

ContactEntryList *KAddressBookView::contactEntryList()
{
  return cel;
}

ContactListView *KAddressBookView::listView()
{
  return mListView;
}

QStringList *KAddressBookView::fields()
{
  return &field;
}

void KAddressBookView::showSelectNameDialog()
{
  SelectFields* sel = new SelectFields( field, this, "select", TRUE );
  if (!sel->exec())
    return;
  // Can't undo this action maybe should show a warning.
  //1  UndoStack::instance()->clear();
  //1  RedoStack::instance()->clear();

  selectNames( sel->chosenFields() );
  reconstructListView();
  delete sel;
}

void KAddressBookView::defaultSettings()
{
  selectNames( QStringList() );
  mListView->backPixmapOn = false;
  mListView->underline = true;
  mListView->autUnderline = true;
  mListView->tooltips_ = true;
  reconstructListView();
}

void KAddressBookView::reconstructListView()
{
  cbField->clear();
  for ( uint i = 0; i < field.count(); i++ )
    cbField->insertItem( Attributes::instance()->fieldToName( field[i]));

  QObject::disconnect( iSearch, SIGNAL( textChanged( const QString& )),
		       mListView, SLOT( incSearch( const QString& )));
  QObject::disconnect( cbField, SIGNAL( activated( int )),
		       mListView, SLOT( setSorting( int )));
  delete mListView;

  setupListView();
  mListView->setSorting( 0, true );

  QObject::connect( iSearch, SIGNAL( textChanged( const QString& )),
		    mListView, SLOT( incSearch( const QString& )));
  QObject::connect( cbField, SIGNAL( activated( int )),
		    mListView, SLOT( setSorting( int )));
  mainLayout->addWidget( mListView );
  mainLayout->activate();
  mListView->show();
}

KAddressBookView::KAddressBookView( ContactEntryList *cel,
		      QWidget *parent,
		      const char *name )
  : QWidget( parent, name ), cel( cel )
{
  readConfig();
  mainLayout = new QVBoxLayout( this, 2 );

  QBoxLayout *searchLayout = new QHBoxLayout( mainLayout, 2 );

  QLabel *liSearch = new QLabel( i18n( "&Incremental Search" ), this );
  liSearch->resize( liSearch->sizeHint() );
  searchLayout->addWidget( liSearch, 0 );

  iSearch = new QLineEdit( this );
  iSearch->resize( iSearch->sizeHint() );
  searchLayout->addWidget( iSearch, 0 );
  liSearch->setBuddy( iSearch );

  // Create a non-editable Combobox and a label below...
  cbField = new QComboBox( FALSE, this );
  cbField->resize( iSearch->sizeHint() );
  searchLayout->addWidget( cbField, 0 );

  for ( uint i = 0; i < field.count(); i++ )
    cbField->insertItem( Attributes::instance()->fieldToName( field[i]));
  setupListView();
  mListView->resort();

  mainLayout->addWidget( mListView );
  mainLayout->activate();

  QObject::connect( iSearch, SIGNAL( textChanged( const QString& )),
		    mListView, SLOT( incSearch( const QString& )));
  QObject::connect( cbField, SIGNAL( activated( int )),
		    mListView, SLOT( setSorting( int )));
  QObject::connect( mListView, SIGNAL( returnPressed( QListViewItem *)),
		    this, SLOT( properties()));
}

KAddressBookView::~KAddressBookView()
{
    kdDebug()<< "Destroying KAddressBookView\n";
}

void KAddressBookView::setupListView()
{
  mListView = new ContactListView( this );
  QObject::connect( mListView, SIGNAL( selectionChanged() ),
		    this, SLOT( selectionChanged() ) );
  QObject::connect( mListView, SIGNAL( doubleClicked( QListViewItem* ) ),
		    this, SLOT( itemSelected( QListViewItem* ) ) );
  repopulate();
}

void KAddressBookView::repopulate()
{
  mListView->clear();
  for ( uint i = 0; i < field.count(); i++ )
    mListView->addColumn( Attributes::instance()->fieldToName( field[i] ),
			 fieldWidth[i] );

  //xxx
  QStringList keys = cel->keys();
  for ( QStringList::Iterator it = keys.begin(); it != keys.end(); ++it )
    addEntry( *it );

  /*
  QDictIterator<ContactEntry> it(*cel);
  while (it.current()) {
    addEntry( it.currentKey() );
    ++it;
  }
  */
}

ContactListViewItem* KAddressBookView::addEntry( QString entryKey )
{
  ContactListViewItem *item = new ContactListViewItem( entryKey, mListView, &field );
  item->refresh();
  return item;
}

// Will have to insert into cel and save key
void KAddressBookView::addNewEntry( ContactEntry *ce )
{
  PwNewCommand *command = new PwNewCommand( this, ce );
  UndoStack::instance()->push( command );
  RedoStack::instance()->clear();
}

void KAddressBookView::selectionChanged()
{
}

void KAddressBookView::selectAll()
{
  QListViewItem *item;
  for(item = mListView->firstChild(); item; item = item->itemBelow())
    mListView->setSelected( item, true );
}

void KAddressBookView::properties()
{
  itemSelected( mListView->currentItem() );
}

QString KAddressBookView::selectedEmails()
{
  bool first = true;
  QString emailAddrs;
  QListViewItem *item;
  for(item = mListView->firstChild(); item; item = item->itemBelow()) {
    if (!mListView->isSelected( item ))
      continue;
    ContactListViewItem *plvi = dynamic_cast< ContactListViewItem* >(item);
    if (!plvi)
      continue;
    QString entryKey = plvi->entryKey();
    ContactEntry *ce = plvi->getEntry();
    if (!ce)
      continue;
    if (!ce->find( "EMAIL" ))
      continue;
    QString email = *ce->find( "EMAIL" );
    if (email.isEmpty())
      continue;
    email.stripWhiteSpace();

    QString sFileAs;
    if (ce->find( "N" )) {
      sFileAs = *ce->find( "N" );
      sFileAs.stripWhiteSpace();
      sFileAs += " ";
    }

    if (!first)
      emailAddrs += ", ";
    else
      first = false;

    emailAddrs += sFileAs + "<" + email + ">";
  }
  return emailAddrs;
}


void KAddressBookView::updateContact( QString addr, QString name )
{
  ContactEntryList *cel = contactEntryList();
  QStringList keys = cel->keys();
  for ( QStringList::Iterator it = keys.begin(); it != keys.end(); ++it ) {
    ContactEntry *ce = cel->find( *it );
    if (ce)
      if (ce->find("EMAIL")  && ((*ce->find("EMAIL")).stripWhiteSpace() == addr)) {
	if (!name.isEmpty())
	  ce->replace( "N", new QString( name ) );
	QString title = i18n( "Address Book Entry Editor" );
	PabContactDialog *cd = new PabContactDialog( title, this, 0, *it, ce );
	QObject::connect( cd, SIGNAL( change( QString, ContactEntry* ) ),
			  this, SLOT( change( QString, ContactEntry* ) ));
	cd->show();
	return;
      }
  }

  ContactDialog *cd = new PabNewContactDialog( i18n( "Address Book Entry Editor" ), this, 0);
  ContactEntry *ce = cd->entry();
  if (!name.isEmpty())
    ce->replace( ".AUXCONTACT-N", new QString(name) );
  ce->replace( "EMAIL", new QString( addr ) );
  connect( cd, SIGNAL( add( ContactEntry* ) ),
	   this, SLOT( addNewEntry( ContactEntry* ) ));
  cd->parseName();
  cd->show();
}

void KAddressBookView::addEmail(const QString& aStr)
{
  int i, j, len;
  QString partA, partB, result;
  char endCh = '>';

  i = aStr.find('<');
  if (i<0)
  {
    i = aStr.find('(');
    endCh = ')';
  }
  if (i<0) {
    updateContact( aStr, "" );
    return;
  }
  partA = aStr.left(i).stripWhiteSpace();
  j = aStr.find(endCh,i+1);
  if (j<0) {
    updateContact( aStr, "" );
    return;
  }
  partB = aStr.mid(i+1, j-i-1).stripWhiteSpace();

  if (partA.find('@') >= 0 && !partB.isEmpty()) result = partB;
  else if (!partA.isEmpty()) result = partA;
  else result = aStr;

  len = result.length();
  if (result[0]=='"' && result[len-1]=='"')
    result = result.mid(1, result.length()-2);
  else if (result[0]=='<' && result[len-1]=='>')
    result = result.mid(1, result.length()-2);
  else if (result[0]=='(' && result[len-1]==')')
    result = result.mid(1, result.length()-2);

  updateContact( partB, result );
}

void KAddressBookView::sendMail()
{
  QString emailAddrs = selectedEmails();
  kapp->invokeMailer( emailAddrs, "" );
}

void KAddressBookView::itemSelected( QListViewItem *item )
{
  if ( item == 0 )
    return;

  ContactListViewItem *plvi = dynamic_cast< ContactListViewItem* >(item);

  if (plvi) {
    QString title = i18n( "Address Book Entry Editor" );
    QString entryKey = plvi->entryKey();
    ContactEntry *ce = cel->find( entryKey );
    if (!ce) { // Another process deleted it(!)
        kdDebug()<< "KAddressBookView::itemSelected Associated entry not found\n" ;
      return;
    }
    PabContactDialog *cd = new PabContactDialog( title, this, 0, entryKey, ce );
    QObject::connect( cd, SIGNAL( change( QString, ContactEntry* ) ),
		      this, SLOT( change( QString, ContactEntry* ) ));
    cd->show();
  }

  item->setSelected( TRUE );

  item->repaint();
}

void KAddressBookView::change( QString entryKey, ContactEntry *ce )
{
  ContactListViewItem *plvi = mListView->getItem( entryKey );
  ContactEntry *oldce = cel->find( entryKey );
  if (plvi && oldce) {
    PwEditCommand *command = new PwEditCommand( this, entryKey, oldce, ce );
    UndoStack::instance()->push( command );
    RedoStack::instance()->clear();
  }
  else {
    PwNewCommand *command = new PwNewCommand( this, ce );
    UndoStack::instance()->push( command );
    RedoStack::instance()->clear();
  }
}

void KAddressBookView::cut()
{
  PwCutCommand *command = new PwCutCommand( this );
  UndoStack::instance()->push( command );
  RedoStack::instance()->clear();
}

void KAddressBookView::copy()
{
  QListViewItem *item;
  QString clipText;
  QTextOStream clipStream( &clipText );
  for(item = mListView->firstChild(); item; item = item->itemBelow()) {
    if (!mListView->isSelected( item ))
      continue;
    ContactListViewItem *lvi = dynamic_cast< ContactListViewItem* >(item);
    if (lvi) {
      ContactEntry *ce = lvi->getEntry();
      if (ce)
	ce->save( clipStream );
    }
  }
  QClipboard *cb = QApplication::clipboard();
  cb->setText( clipText );
}

void KAddressBookView::paste()
{
  QClipboard *cb = QApplication::clipboard();
  PwPasteCommand *command = new PwPasteCommand( this, cb->text() );
  UndoStack::instance()->push( command );
  RedoStack::instance()->clear();
}

void KAddressBookView::clear()
{
    kdDebug()<< "clear\n";
  QListViewItem *item = mListView->currentItem();
  ContactListViewItem *lvi = dynamic_cast< ContactListViewItem* >(item);
  if (lvi) {
    QString entryKey = lvi->entryKey();
    ContactEntry *ce = lvi->getEntry();
    PwDeleteCommand *command = new PwDeleteCommand( this, entryKey, ce );
    UndoStack::instance()->push( command );
    RedoStack::instance()->clear();
  }
}

void KAddressBookView::saveConfig()
{
  KConfig *config = kapp->config();

  mListView->saveConfig();

  config->setGroup("Browser");

  QStringList actualFields;
  QStringList actualWidths;
  for(uint i = 0; i < field.count(); ++i) {
    int act = mListView->header()->mapToLogical( i );
    actualFields += field[act];
    int size = mListView->header()->cellSize( act );
    actualWidths += QString().setNum( size );
  }

  config->writeEntry("fields", actualFields );
  config->writeEntry("fieldWidths", actualWidths );
}

void KAddressBookView::readConfig()
{
  KConfig *config = kapp->config();

  config->setGroup("Browser");
  field = config->readListEntry("fields" );
  selectNames( field );
  QStringList fieldWidthStr = config->readListEntry("fieldWidths" );
  fieldWidth.clear();
  QStringList::Iterator it;
  for(it = fieldWidthStr.begin(); it != fieldWidthStr.end(); ++it)
    fieldWidth += atoi( (*it).ascii() );
  while (fieldWidth.count() < field.count())
    fieldWidth += 120;
}

void KAddressBookView::viewOptions()
{
  ViewOptions *vo = new ViewOptions( mListView->backPixmapOn,
				     mListView->backPixmap,
				     mListView->underline,
				     mListView->autUnderline,
				     mListView->cUnderline,
				     mListView->tooltips(),
				     this,
				     "ViewOptions",
				     true );

  if (!vo->exec())
    return;
  mListView->backPixmapOn = vo->ckBackPixmap->isChecked();
  mListView->backPixmap = vo->pixmapPath->lineEdit()->text();
  mListView->underline = vo->ckUnderline->isChecked();
  mListView->autUnderline = vo->ckAutUnderline->isChecked();
  mListView->cUnderline = vo->kcbUnderline->color();
  mListView->tooltips_ = vo->ckTooltips->isChecked();
  mListView->loadBackground();
  mListView->saveConfig();
  mListView->triggerUpdate();
  delete vo;
}


#include "kaddressbookview.moc"
