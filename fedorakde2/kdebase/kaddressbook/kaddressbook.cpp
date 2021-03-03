/*
 * kaddressbook.cpp
 *
 * Copyright (C) 1999 Don Sanders <dsanders@kde.org>
 */
#include "kaddressbook.h"
#include "browserentryeditor.h"

#include <qkeycode.h>

#include <kiconloader.h>
#include <klocale.h>
#include <kglobal.h>
#include <kmenubar.h>
#include <kconfig.h>
#include <kaccel.h>
#include <kdebug.h>
#include <kfiledialog.h>
#include <kaction.h>
#include <kstdaction.h>

#include "undo.h"
#include "kaddressbookview.h"
#include "contactentry.h"
#include "contactentrylist.h"
#include "contactlistview.h"
#include "kimportdialog.h"

class ContactImportDialog : public KImportDialog {
  public:
    ContactImportDialog(ContactEntryList *list,QWidget *parent)
        : KImportDialog(parent), mList(list), mEntry(0)
    {
      mFirstName = new KImportColumn(this,i18n("First Name"),1);
      mLastName = new KImportColumn(this,i18n("Last Name"),1);
//      mFullName = new KImportColumn(this,i18n("Full Name"));
      mEmail = new KImportColumn(this,i18n("Email"));
      mPhoneHome = new KImportColumn(this,i18n("Phone Home"));
      mPhoneBusiness = new KImportColumn(this,i18n("Phone Business"));
      mPhoneMobile = new KImportColumn(this,i18n("Phone Mobile"));
      mFaxHome = new KImportColumn(this,i18n("Fax Home"));
      mFaxBusiness = new KImportColumn(this,i18n("Fax Business"));
      mJobTitle = new KImportColumn(this,i18n("Job Title"));
      mCompany = new KImportColumn(this,i18n("Company"));

      mAddressHomeCity = new KImportColumn(this,i18n("Home Address City"));
      mAddressHomeStreet = new KImportColumn(this,i18n("Home Address Street"));
      mAddressHomeZip = new KImportColumn(this,i18n("Home Address Postal Code"));
      mAddressHomeState = new KImportColumn(this,i18n("Home Address State"));
      mAddressHomeCountry = new KImportColumn(this,i18n("Home Address Country"));

      mAddressBusinessCity = new KImportColumn(this,i18n("Business Address City"));
      mAddressBusinessStreet = new KImportColumn(this,i18n("Business Address Street"));
      mAddressBusinessZip = new KImportColumn(this,i18n("Business Address Postal Code"));
      mAddressBusinessState = new KImportColumn(this,i18n("Business Address State"));
      mAddressBusinessCountry = new KImportColumn(this,i18n("Business Address Country"));

      registerColumns();
    }

    void convertRow()
    {
      mEntry = new ContactEntry;

      mEntry->setFirstName(mFirstName->convert());
      mEntry->setLastName(mLastName->convert());
//      mEntry->setFullName(mFullName->convert());
      mEntry->setEmail(mEmail->convert());
      mEntry->setJobTitle(mJobTitle->convert());
      mEntry->setBusinessPhone(mPhoneBusiness->convert());
      mEntry->setHomePhone(mPhoneHome->convert());
      mEntry->setMobilePhone(mPhoneMobile->convert());
      mEntry->setHomeFax(mFaxHome->convert());
      mEntry->setBusinessFax(mFaxBusiness->convert());
      mEntry->setCompany(mCompany->convert());

      ContactEntry::Address *address = mEntry->getHomeAddress();
      address->setStreet(mAddressHomeStreet->convert());
      address->setCity(mAddressHomeCity->convert());
      address->setState(mAddressHomeState->convert());
      address->setZip(mAddressHomeZip->convert());
      address->setCountry(mAddressHomeCountry->convert());

      address = mEntry->getBusinessAddress();
      address->setStreet(mAddressBusinessStreet->convert());
      address->setCity(mAddressBusinessCity->convert());
      address->setState(mAddressBusinessState->convert());
      address->setZip(mAddressBusinessZip->convert());
      address->setCountry(mAddressBusinessCountry->convert());

      mList->insert(mEntry);
    }

  private:
    KImportColumn *mFirstName;
    KImportColumn *mLastName;
//    KImportColumn *mFullName;
    KImportColumn *mEmail;
    KImportColumn *mJobTitle;
    KImportColumn *mPhoneBusiness;
    KImportColumn *mPhoneHome;
    KImportColumn *mPhoneMobile;
    KImportColumn *mFaxBusiness;
    KImportColumn *mFaxHome;
    KImportColumn *mCompany;

    KImportColumn *mAddressHomeStreet;
    KImportColumn *mAddressHomeCity;
    KImportColumn *mAddressHomeState;
    KImportColumn *mAddressHomeZip;
    KImportColumn *mAddressHomeCountry;

    KImportColumn *mAddressBusinessStreet;
    KImportColumn *mAddressBusinessCity;
    KImportColumn *mAddressBusinessState;
    KImportColumn *mAddressBusinessZip;
    KImportColumn *mAddressBusinessCountry;

    ContactEntryList *mList;
    ContactEntry *mEntry;
};


KAddressBook::KAddressBook() : KMainWindow(0), DCOPObject("KAddressBookIface")
{
  setCaption( i18n( "Address Book Browser" ));
  mDocument = new ContactEntryList();
  mView = new KAddressBookView( mDocument, this, "KAddressBook" );

  // tell the KMainWindow that this is indeed the main widget
  setCentralWidget(mView);

  initActions();

  // we do want a status bar
  statusBar()->show();
  connect( kapp, SIGNAL( aboutToQuit() ), this, SLOT( saveConfig() ) );

  readConfig();
}

void KAddressBook::initActions()
{
  KAction *action;

  // File menu
  new KAction(i18n("&Sync"), CTRL+Key_S, this, SLOT(slotSave()),
              actionCollection(),"file_sync");
  new KAction(i18n("&New Contact"),"filenew",CTRL+Key_N,this,
              SLOT(slotNewContact()),actionCollection(),"file_new_contact");
  /*  p->insertItem(i18n("New &Group"), this, SLOT(newGroup()), CTRL+Key_G); */
  KStdAction::mail(mView,SLOT(sendMail()),actionCollection());
  new KAction(i18n("&Properties..."),"edit",0,mView, SLOT(properties()),
              actionCollection(),"file_properties");
  new KAction(i18n("&Import List..."),0,this,SLOT(importCSV()),
              actionCollection(),"file_import_csv");
  new KAction(i18n("&Export List..."),0,this,SLOT(exportCSV()),
              actionCollection(),"file_export_csv");
  KStdAction::quit(this,SLOT(close()),actionCollection());

  // Edit menu
  KStdAction::undo(this,SLOT(undo()),actionCollection());
  KStdAction::redo(this,SLOT(redo()),actionCollection());
  KStdAction::cut(mView,SLOT(cut()),actionCollection());
  KStdAction::copy(mView,SLOT(copy()),actionCollection());
  KStdAction::paste(mView,SLOT(paste()),actionCollection());
  KStdAction::selectAll(mView,SLOT(selectAll()),actionCollection());
  new KAction(i18n("&Delete"),"editdelete",0,mView,SLOT(clear()),
              actionCollection(),"edit_delete");
#if 0
  edit->setItemEnabled( undoId, false );
  edit->setItemEnabled( redoId, false );
  QObject::connect( edit, SIGNAL( aboutToShow() ), this, SLOT( updateEditMenu() ));
#endif

  // View menu
  new KAction(i18n("Choose Fields..."),0,mView,SLOT(showSelectNameDialog()),
              actionCollection(),"view_choose_fields");
  new KAction(i18n("Options..."),0,mView,SLOT(viewOptions()),
              actionCollection(),"view_options");
  new KAction(i18n("Restore Defaults"),0,mView,SLOT(defaultSettings()),
              actionCollection(),"view_defaults");
  //  v->insertItem(i18n("Refresh"), mView, SLOT(refresh()), Key_F5 );

  createGUI();
}

void KAddressBook::newContact()
{
  kdDebug() << "KAddressBook::newContact() start" << endl;
  ContactDialog *cd = new PabNewContactDialog( i18n( "Address Book Entry Editor" ), this, 0 );
  connect( cd, SIGNAL( add( ContactEntry* ) ),
	   mView, SLOT( addNewEntry( ContactEntry* ) ));
  cd->show();
  kdDebug() << "KAddressBook::newContact() stop" << endl;
}

void KAddressBook::addEmail( QString addr )
{
  mView->addEmail( addr );
  return;
}

void KAddressBook::addEntry(ContactEntry newEntry)
{
  mView->addNewEntry(new ContactEntry(newEntry) );
}

QStringList KAddressBook::getKeys() const
{
  return mDocument->keys();
}

QDict<ContactEntry> KAddressBook::getEntryDict() const
{
  return mDocument->getDict();
}

void  KAddressBook::changeEntry( QString key, ContactEntry changeEntry)
{
  mDocument->replace(key, new ContactEntry(changeEntry));
  mView->listView()->getItem(key)->refresh();
}

void  KAddressBook::removeEntry( QString key )
{
  mDocument->remove(key);
  delete mView->listView()->getItem(key);
}

void KAddressBook::save()
{
  mDocument->commit();
  mDocument->refresh();
  mView->saveConfig();
  mView->readConfig();
  mView->reconstructListView();
  //xxx  mDocument->save( "entries.txt" );
  UndoStack::instance()->clear();
  RedoStack::instance()->clear();
}

void KAddressBook::readConfig()
{
  KConfig *config = kapp->config();
  int w, h;
  config->setGroup("Geometry");
  QString str = config->readEntry("Browser", "");
  if (!str.isEmpty() && str.find(',')>=0)
  {
    sscanf(str.local8Bit(),"%d,%d",&w,&h);
    resize(w,h);
  }
  applyMainWindowSettings( config );
}

void KAddressBook::saveConfig()
{
  mView->saveConfig();
  KConfig *config = kapp->config();

  config->setGroup("Geometry");
  QRect r = geometry();
  QString s;
  s.sprintf("%i,%i", r.width(), r.height());
  config->writeEntry("Browser", s);
  saveMainWindowSettings( config );
  config->sync();
}

KAddressBook::~KAddressBook()
{
  saveConfig();
  delete mDocument;
}

void KAddressBook::saveCe() {
  kdDebug() << "saveCe()" << endl;
  //xxx  ce->save( "entry.txt" );
}

void KAddressBook::saveProperties(KConfig *)
{
  // the 'config' object points to the session managed
  // config file.  anything you write here will be available
  // later when this app is restored
  
  //what I want to save
  //windowsize
  //background image/underlining color/alternating color1,2
  //chosen fields
  //chosen fieldsWidths

  // e.g., config->writeEntry("key", var);
}

void KAddressBook::readProperties(KConfig *)
{
  // the 'config' object points to the session managed
  // config file.  this function is automatically called whenever
  // the app is being restored.  read in here whatever you wrote
  // in 'saveProperties'

  // e.g., var = config->readEntry("key");
}

void KAddressBook::undo()
{
  kdDebug() << "KAddressBook::undo()" << endl;
  UndoStack::instance()->undo();
}

void KAddressBook::redo()
{
  RedoStack::instance()->redo();
}

void KAddressBook::exit()
{
  kapp->quit();
}

void KAddressBook::updateEditMenu()
{
// TODO: convert to KActions
#if 0
  kdDebug() << "UpdateEditMenu()" << endl;
  UndoStack *undo = UndoStack::instance();
  RedoStack *redo = RedoStack::instance();

  if (undo->isEmpty())
    edit->changeItem( undoId, i18n( "Undo" ) );
  else
    edit->changeItem( undoId, i18n( "Undo" ) + " " + undo->top()->name() );
  edit->setItemEnabled( undoId, !undo->isEmpty() );

  if (redo->isEmpty())
    edit->changeItem( redoId, i18n( "Redo" ) );
  else
    edit->changeItem( redoId, i18n( "Redo" ) + " " + redo->top()->name() );
  edit->setItemEnabled( redoId, !redo->isEmpty() );
#endif
}

void KAddressBook::importCSV()
{
  kdDebug() << "KAddressBook::importCSV()" << endl;

  ContactImportDialog *dialog = new ContactImportDialog(mDocument,this);

  dialog->exec();

  mView->reconstructListView();
  
  delete dialog;
}

void KAddressBook::exportCSV()
{
  QString fileName = KFileDialog::getSaveFileName("addressbook.csv");

  QFile outFile(fileName);
  if ( outFile.open(IO_WriteOnly) ) {    // file opened successfully
    QTextStream t( &outFile );        // use a text stream

    QStringList keys = mDocument->keys();
    QStringList::ConstIterator it = keys.begin();
    QStringList::ConstIterator end = keys.end();

    t << "\"First Name\",";
    t << "\"Last Name\",";
    t << "\"Middle Name\",";
    t << "\"Name Prefix\",";
    t << "\"Job Title\",";
    t << "\"Company\",";
    t << "\"Email\",";
    t << "\"Nickname\",";
    t << "\"Note\",";
    t << "\"Business Phone\",";
    t << "\"Home Phone\",";
    t << "\"Mobile Phone\",";
    t << "\"Home Fax\",";
    t << "\"Business Fax\",";
    t << "\"Pager\",";
    t << "\"Street Home Address\",";
    t << "\"City Home Address\",";
    t << "\"State Home Address\",";
    t << "\"Zip Home Address\",";
    t << "\"Country Home Address\",";
    t << "\"Street Business Address\",";
    t << "\"City Business Address\",";
    t << "\"State Business Address\",";
    t << "\"Zip Business Address\",";
    t << "\"Country Business Address\"";
    t << "\n";

    while(it != end) {
      ContactEntry *entry = mDocument->find(*it);

      t << "\"" << entry->getFirstName() << "\",";
      t << "\"" << entry->getLastName() << "\",";
      t << "\"" << entry->getMiddleName() << "\",";
      t << "\"" << entry->getNamePrefix() << "\",";
      t << "\"" << entry->getJobTitle() << "\",";
      t << "\"" << entry->getCompany() << "\",";
      t << "\"" << entry->getEmail() << "\",";
      t << "\"" << entry->getNickname() << "\",";
      t << "\"" << entry->getNote() << "\",";
      t << "\"" << entry->getBusinessPhone() << "\",";
      t << "\"" << entry->getHomePhone() << "\",";
      t << "\"" << entry->getMobilePhone() << "\",";
      t << "\"" << entry->getHomeFax() << "\",";
      t << "\"" << entry->getBusinessFax() << "\",";
      t << "\"" << entry->getPager() << "\",";
      ContactEntry::Address *address;
      address = entry->getHomeAddress();
      t << "\"" << address->getStreet() << "\",";
      t << "\"" << address->getCity() << "\",";
      t << "\"" << address->getState() << "\",";
      t << "\"" << address->getZip() << "\",";
      t << "\"" << address->getCountry() << "\",";
      address = entry->getBusinessAddress();
      t << "\"" << address->getStreet() << "\",";
      t << "\"" << address->getCity() << "\",";
      t << "\"" << address->getState() << "\",";
      t << "\"" << address->getZip() << "\",";
      t << "\"" << address->getCountry() << "\",";
      t << "\n";

      ++it;
    }

    outFile.close();
  }
}

#include "kaddressbook.moc"
