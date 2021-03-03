/* This file is part of KDE PIM
   Copyright (C) 1999 Don Sanders <sanders@kde.org>

   License: BSD
*/

#include "entryeditorwidget.h"
#include <qlayout.h>
#include <qvbox.h>
#include <qwidget.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qgrid.h>
#include <qgroupbox.h>
#include <qtabwidget.h>

#include "namevaluewidget.h"
#include "attributes.h"
#include "contactentry.h"
#include "datepickerdialog.h"
#include <klocale.h>
#include <kapp.h> // for kapp->palette()
#include <kglobal.h>
#include <kdebug.h>
#include <kseparator.h>

ContactDialog::ContactDialog( const QString & title, QWidget *parent, const char *name, ContactEntry *ce, bool modal )
  : QDialog( parent, name, modal ), vs( 0 ), vp( 0 )
{
    ce ? this->ce = ce : this->ce = new ContactEntry();
    if (ce->find( "N" ))
      curName = *ce->find( "N");
    setCaption( title );

    QVBoxLayout *vb = new QVBoxLayout( this, 5 );
    tabs = new QTabWidget( this );
    vb->addWidget( tabs );
    tabs->setMargin( 4 );

    setupTab1();
    setupTab2();
    setupTab3();

    QHBoxLayout *hb = new QHBoxLayout( vb, 5 );
    hb->addStretch( 1 );
    pbOk = new QPushButton( i18n("&OK"), this );
    hb->addWidget( pbOk, 0 );
    pbOk->setDefault( true );
    pbOk->setAutoDefault( true );
    QPushButton *pbCancel = new QPushButton( i18n("Cancel"), this );
    hb->addWidget( pbCancel, 0 );

    connect( pbOk, SIGNAL( clicked() ), this, SLOT( ok()));
    connect( pbCancel, SIGNAL( clicked() ), this, SLOT( reject()));
    leFullName->setFocus();

    ce->touch();
}

void ContactDialog::ok()
{
  pbOk->setFocus();
  accept();
  emit accepted();
}

ContactEntry* ContactDialog::entry()
{
  return ce;
}

void ContactDialog::setupTab1()
{
  QFrame *tab1 = new QFrame( 0, "tab1" );
  tab1->setFrameStyle( QFrame::NoFrame );
  QGridLayout *tab1lay = new QGridLayout( tab1, 8, 5 );
  tab1lay->setSpacing( 5 );
  tab1->setMargin( 5 );

/////////////////////////////////
// The Name/Job title group

  // First row
  QPushButton *pbFullName = new QPushButton( i18n( "&Full Name..." ), tab1 );
  leFullName = new ContactLineEdit( tab1, ".AUXCONTACT-N", ce );
  leFullName->setText( curName );

  connect( ce, SIGNAL( changed() ), this, SLOT( parseName() ));
  connect( pbFullName, SIGNAL( clicked()), this, SLOT( newNameDialog()));
  tab1lay->addWidget( pbFullName, 0, 0 );
  tab1lay->addWidget( leFullName, 0, 1 );

  QFrame * filler1 = new QFrame( tab1, "filler1" );
  filler1->setFrameStyle( QFrame::NoFrame );
  filler1->setMinimumWidth( 1 );
  tab1lay->addWidget( filler1, 0, 2 );

  QLabel *lJobTitle = new QLabel( i18n( "&Job title:" ), tab1 );
  QLineEdit *leJobTitle = new ContactLineEdit( tab1, "ROLE", ce );
  lJobTitle->setBuddy( leJobTitle );
  tab1lay->addWidget( lJobTitle, 0, 3 );
  tab1lay->addWidget( leJobTitle, 0, 4 );

  // Second row
  QLabel *lCompany = new QLabel( i18n( "&Company:" ), tab1 );
  QLineEdit *leCompany = new ContactLineEdit( tab1, "ORG", ce );
  lCompany->setBuddy( leCompany );
  curCompany = leCompany->text();
  connect( ce, SIGNAL( changed() ), this, SLOT( monitorCompany() ));
  tab1lay->addWidget( lCompany, 1, 0 );
  tab1lay->addWidget( leCompany, 1, 1 );

  QFrame * filler2 = new QFrame( tab1, "filler2" );
  filler2->setFrameStyle( QFrame::NoFrame );
  filler2->setMinimumWidth( 1 );
  tab1lay->addWidget( filler2, 1, 2 );

  QLabel *lFileAs = new QLabel( i18n( "F&ile as:" ), tab1 );
  cbFileAs = new FileAsComboBox( tab1, "X-FileAs", ce );
  QString sFileAs;
  if (ce->find( "X-FileAs" ))
    sFileAs = *ce->find( "X-FileAs" );
  updateFileAs();
  if (sFileAs != "")
    ce->replace( "X-FileAs", new QString( sFileAs ));
  connect( cbFileAs, SIGNAL( textChanged( const QString& ) ), cbFileAs, SLOT( updateContact() ));
  tab1lay->addWidget( lFileAs, 1, 3 );
  tab1lay->addWidget( cbFileAs, 1, 4 );

  lFileAs->setBuddy( cbFileAs );
// End the Name/Job title group
////////////////////////////////

  // Horizontal bar (rather verbose)
  KSeparator* bar1 = new KSeparator( KSeparator::HLine, tab1);
  tab1lay->addMultiCellWidget( bar1, 2, 2, 0, 4 );
  tab1lay->addRowSpacing(2, 10);

////////////////////////////////
// The Email/Webpage group
  ContactComboBox *cbEmail = new ContactComboBox( tab1 );
  cbEmail->insertItem( i18n( "Email" ), "EMAIL" );
  cbEmail->insertItem( i18n( "Email 2" ), "X-E-mail2" );
  cbEmail->insertItem( i18n( "Email 3" ), "X-E-mail3" );
  QLineEdit *leEmail = new ContactLineEdit( tab1, "EMAIL", ce );
  cbEmail->setBuddy( leEmail );
  tab1lay->addWidget( cbEmail, 3, 0 );
  tab1lay->addWidget( leEmail, 3, 1 );

  QFrame *filler3 = new QFrame( tab1, "filler3" );
  filler3->setFrameStyle( QFrame::NoFrame );
  filler3->setMinimumWidth( 1 );
  tab1lay->addWidget( filler3, 3, 2 );

  QLabel *lWebPage = new QLabel( i18n( "&Web page:" ), tab1 );
  QLineEdit *leWebPage = new ContactLineEdit( tab1, "WEBPAGE", ce );
  lWebPage->setBuddy( leWebPage );
  tab1lay->addWidget( lWebPage, 3, 3 );
  tab1lay->addWidget( leWebPage, 3, 4 );

// End the Email/Webpage group
///////////////////////////////

  // Horizontal bar (rather verbose)
  KSeparator* bar2 = new KSeparator( KSeparator::HLine, tab1);
  tab1lay->addMultiCellWidget( bar2, 4, 4, 0, 4 );
  tab1lay->addRowSpacing(4, 10);

///////////////////////////////
// The Address/Phone group

  // Use a box to keep the widgets fixed vertically in place
  QBoxLayout *lay1 = new QBoxLayout( QBoxLayout::Down, 10, "lay1" );

  QPushButton *pbAddress = new QPushButton( i18n( "Add&ress..." ), tab1 );
  connect( pbAddress, SIGNAL( clicked()), this, SLOT( newAddressDialog()));
  lay1->addWidget( pbAddress, 0 );
  cbAddress = new ContactComboBox( tab1 );

  QStringList addresses;
  addresses += i18n( "Business" );
  addresses += i18n( "Home" );
  addresses += i18n( "Other" );
  addresses.sort();
  QString addressName;
  addressName = Attributes::instance()->nameToField( addresses[0] );
  cbAddress->insertItem( addresses[0], addressName );
  addressName = Attributes::instance()->nameToField( addresses[1] );
  cbAddress->insertItem( addresses[1], addressName );
  addressName = Attributes::instance()->nameToField( addresses[2] );
  cbAddress->insertItem( addresses[2], addressName );
  cbAddress->setCurrentItem( addresses.findIndex( i18n( "Business" )));

  lay1->addWidget( cbAddress, 0 );
  lay1->addStretch( 1 );  // Fix the other widgets in place
  tab1lay->addLayout( lay1, 5, 0 );

  // Perhaps the address "subfields" (city, postal, country) should be cleared
  // when this control loses focus. They aren't at the moment.
  QMultiLineEdit *mleAddress = new ContactMultiLineEdit( tab1, "X-BusinessAddress", ce );
  mleAddress->setWordWrap( QMultiLineEdit::WidgetWidth );
  cbAddress->setBuddy( mleAddress );
  tab1lay->addWidget( mleAddress, 5, 1 );

  QFrame *filler4 = new QFrame( tab1, "filler4" );
  filler4->setFrameStyle( QFrame::NoFrame );
  filler4->setMinimumWidth( 1 );
  tab1lay->addWidget( filler4, 5, 2 );

  QLabel *lPhone = new QLabel( i18n( "Phone" ) + ":", tab1 );
  lPhone->setAlignment( QLabel::AlignTop | QLabel::AlignLeft );
  tab1lay->addWidget( lPhone, 5, 3 );

  QBoxLayout *lay2 = new QBoxLayout( QBoxLayout::TopToBottom, 1 );
  lay2->setSpacing( 10 );
  const int numRows = 4;

  QStringList namePhone;
  QStringList fieldPhone;
  Attributes::instance()->nameFieldList( 8, &namePhone, &fieldPhone );

  int iPhone[4];
  iPhone[0] = fieldPhone.findIndex( "X-BusinessPhone" );
  iPhone[1] = fieldPhone.findIndex( "X-HomePhone" );
  iPhone[2] = fieldPhone.findIndex( "X-BusinessFax" );
  iPhone[3] = fieldPhone.findIndex( "X-MobilePhone" );

  QGridLayout *layhGrid = new QGridLayout( numRows, 2 );
  layhGrid->setSpacing( 10 );
  for ( int row = 0; row < numRows; row++ ) {
    QFrame *hGrid = tab1;

    ContactComboBox *cbPhone = new ContactComboBox( hGrid );
    //    for (int i =0; sPhone[i] != ""; ++i )
    //      cbPhone->insertItem( i18n( sPhone[i] ), vPhone[i] );
    for( int i = 0; i < (int)namePhone.count(); ++i )
      cbPhone->insertItem(  namePhone[i], fieldPhone[i] );
    cbPhone->setCurrentItem( iPhone[row] );
    cbPhone->setMinimumSize( cbPhone->sizeHint() );
    layhGrid->addWidget( cbPhone, row, 0 );

    QLineEdit *ed = new ContactLineEdit( hGrid, fieldPhone[iPhone[row]].ascii(), ce );
    ed->setMinimumSize( ed->sizeHint());
    cbPhone->setBuddy( ed );
    layhGrid->addWidget( ed, row ,1 );
  }
  lay2->addLayout( layhGrid, 0 );
  lay2->addStretch( 1 ) ;
  tab1lay->addLayout( lay2, 5, 4 );

// End The Address/Phone group
///////////////////////////////

  // Horizontal bar
  KSeparator* bar3 = new KSeparator( KSeparator::HLine, tab1);
  tab1lay->addMultiCellWidget( bar3, 6, 6, 0, 4 );
  tab1lay->addRowSpacing(6, 10);

//////////////////////
// The Note group
  // Interestingly this doesn't have an equivalent in tab3
  QMultiLineEdit *mleNotes = new ContactMultiLineEdit( tab1, "X-Notes", ce );
  mleNotes->setWordWrap( QMultiLineEdit::WidgetWidth );
  mleNotes->setMinimumSize( mleNotes->sizeHint() );
  mleNotes->resize( mleNotes->sizeHint() );
  tab1lay->addMultiCellWidget( mleNotes, 7, 7, 0, 4 );
// End the Note group
//////////////////////

  tab1lay->activate(); // required
  tabs->addTab( tab1, i18n( "&General" ));
}

void ContactDialog::setupTab2()
{
  // Use a boxlayout to keep the widgets position fixed vertically
  QFrame *v2 = new QFrame( this );
  QBoxLayout *lay2 = new QBoxLayout( v2, QBoxLayout::TopToBottom, 5, 5, "h3BoxLayout" );
  lay2->setSpacing( 10 );

  const int numRows = 9;
  QString sLabel[numRows] = { i18n( "D&epartment:" ), i18n( "&Office:" ),
			      i18n( "&Profession:" ),
			      i18n( "Assistant's &Name:" ),
			      i18n( "&Managers's Name:" ),
			      i18n( "Birthday" ), i18n( "Anniversary" ),
			      i18n( "Ni&ckname:" ), i18n( "&Spouse's Name:" )
  };
  QString entryField[numRows] = {
    "X-Department", "X-Office", "X-Profession", "X-AssistantsName",
    "X-ManagersName", "BDAY", "X-Anniversary", "NICKNAME", "X-SpousesName"
  };

  QLabel *(label[numRows]);
  QPushButton *(pbDate[2]);
  QSize size = QSize( 0, 0 );
  QSize size2 = QSize( 0, 0 );

  for ( int row = 0; row < 5; row++ ) {
    QGrid *hGrid = new QGrid ( 2, QGrid::Horizontal, v2 );
    hGrid->setSpacing( 10 );
    label[row] = new QLabel( sLabel[row], hGrid );
    size = size.expandedTo( label[row]->sizeHint() );
    QLineEdit *ed = new ContactLineEdit( hGrid, entryField[row].ascii(), ce );
    label[row]->setBuddy( ed );
    lay2->addWidget( hGrid, 0 );
  }

  KSeparator* f3 = new KSeparator( KSeparator::HLine, v2);
  lay2->addWidget( f3, 0 );

  for ( int row = 5; row < 7; row++ ) {
    QFrame *v3 = new QFrame( v2 );
    QBoxLayout *lay3 = new QBoxLayout( v3, QBoxLayout::LeftToRight, 1, 1, "h3BoxLayout" );
    lay3->setSpacing( 10 );

    label[row] = new QLabel( sLabel[row], v3 );
    lay3->addWidget( label[row] );
    size = size.expandedTo( label[row]->sizeHint() );
    QLineEdit *ed = new ContactLineEdit( v3, entryField[row].ascii(), ce );
    ed->setMaximumSize( ed->sizeHint() );
    lay3->addWidget( ed, 0 );
    label[row]->setBuddy( ed );
    pbDate[row - 5] = new QPushButton( i18n("Select..."), v3 );
    lay3->addWidget( pbDate[row - 5] );
    size2 = size2.expandedTo( pbDate[row - 5]->sizeHint() );
    lay3->addStretch( 1 ) ;
    lay2->addWidget( v3 );
  }
    connect( pbDate[0], SIGNAL( clicked()), this, SLOT( pickBirthDate() ));
    connect( pbDate[1], SIGNAL( clicked()), this, SLOT( pickAnniversaryDate() ));

  KSeparator* f5 = new KSeparator( KSeparator::HLine, v2);
  lay2->addWidget( f5, 0 );

  for ( int row = 7; row < 9; row++ ) {
    QGrid *hGrid = new QGrid ( 2, QGrid::Horizontal, v2 );
    hGrid->setSpacing( 10 );
    label[row] = new QLabel( sLabel[row], hGrid );
    size = size.expandedTo( label[row]->sizeHint() );
    QLineEdit *ed = new ContactLineEdit( hGrid, entryField[row].ascii(), ce );
    label[row]->setBuddy( ed );
    lay2->addWidget( hGrid, 0 );
  }

  for ( int row = 0; row < numRows; row++ ) {
    label[row]->resize( size );
    label[row]->setMinimumSize( size );
  }
  pbDate[0]->setMinimumSize( size2 );
  pbDate[1]->setMinimumSize( size2 );

  lay2->addStretch( 1 ) ;
  tabs->addTab( v2, i18n( "&Details" ));
}

void ContactDialog::setupTab3()
{
    QStringList names;
    QStringList fields;
    QString tmp;
    QFrame *tab3 = new QFrame( this );
    QBoxLayout *t3lay = new QBoxLayout( tab3, QBoxLayout::TopToBottom, 5, 5 );

    QFrame *row1 = new QFrame( tab3 );
    QBoxLayout *lay1 = new QBoxLayout( row1, QBoxLayout::LeftToRight, 1, 1 );
    lay1->setSpacing( 10 );
    QLabel *lSelectFrom = new QLabel( i18n("&Select from:"), row1 );
    lay1->addWidget( lSelectFrom, 0 );
    cbSelectFrom = new QComboBox( false, row1 );
    lay1->addWidget( cbSelectFrom, 0 );
    lay1->addStretch( 1 );  // Fix the other widgets in place
    lSelectFrom->setBuddy( cbSelectFrom );
    t3lay->addWidget( row1, 0);

    for (int i = 0;
	 tmp = Attributes::instance()->fieldListName( i ), tmp != "";
	 ++i )
      cbSelectFrom->insertItem( tmp );
    cbSelectFrom->insertItem( i18n( "User-defined fields in folder" ));

    cbSelectFrom->setCurrentItem( cbSelectFrom->count() - 1 );
    fields = ce->custom();
    fields.sort();
    for (int i = 0; i < (int)fields.count(); ++i )
      names += fields[i].mid( 9 );

    vs = new NameValueSheet( 0, names.count(), names, fields, ce );
    vp = new NameValueFrame( tab3, vs );
    connect( cbSelectFrom, SIGNAL( activated(int)), this, SLOT( setSheet(int)));
    t3lay->addWidget( vp, 1);

    QFrame *row3 = new QFrame( tab3 );
    QBoxLayout *lay3 = new QBoxLayout( row3, QBoxLayout::LeftToRight, 1, 1 );
    lay3->setSpacing( 10 );
    QPushButton *pbNew = new QPushButton( i18n( "&New..." ), row3 );
    connect( pbNew, SIGNAL( clicked()), this, SLOT( newFieldDialog()));
    lay3->addWidget( pbNew, 0 );
    lay3->addStretch( 1 );  // Fix the other widgets in place
    t3lay->addWidget( row3, 0);
    tabs->addTab( tab3, i18n( "&All fields" ));
}

void ContactDialog::pickBirthDate()
{
  DatePickerDialog* datePicker=new DatePickerDialog( i18n( "Select Birthday" ), this);
  datePicker->setDate(QDate::currentDate());
  if(datePicker->exec())
    ce->replace( "BDAY", new QString( datePicker->getDate().toString()));
    // ce autoDelete will clean it up
  delete datePicker;
}

void ContactDialog::pickAnniversaryDate()
{
  DatePickerDialog* datePicker=new DatePickerDialog( i18n( "Select Anniversary" ), this);
  datePicker->setDate(QDate::currentDate());
  if(datePicker->exec())
    ce->replace( "X-Anniversary", new QString( datePicker->getDate().toString()));
    // ce autoDelete will clean it up
  delete datePicker;
}

void ContactDialog::newAddressDialog()
{
  QDialog *ad = new AddressDialog( this, cbAddress->currentEntryField(), ce, true );
  ad->exec();
}

void ContactDialog::newFieldDialog()
{
  NewFieldDialog *fd = new NewFieldDialog( this, true );
  if (fd->exec()) {
    ce->replace( fd->field(), new QString( fd->value() ));
    cbSelectFrom->setCurrentItem( 9 );
    setSheet( 9 );
  }
}

// We want to update the fileas field using updateFileAs but not
// the automatically the parse the name into its components
// with parseName
void ContactDialog::newNameDialog()
{
  kdDebug() << "newNameDialog " << leFullName->text() << endl;
  if (((ce->find( ".AUXCONTACT-N" ))
       && (leFullName->text() != *ce->find( ".AUXCONTACT-N" ))) ||
      (!ce->find( ".AUXCONTACT-N" )))
    {
      ce->replace( ".AUXCONTACT-N", new QString( leFullName->text() ));
      parseName();
    }
  QDialog *nd = new NameDialog( this, ce, true );
  if (nd->exec()) {
    if (ce->find( "N" )) {
      curName = *ce->find( "N" );
      ce->replace( ".AUXCONTACT-N", new QString( curName ));
    }
    else {
      curName = "";
      leFullName->setText( "");
    }
    updateFileAs();
  }
}

void ContactDialog::monitorCompany()
{
  const QString *org = ce->find( "ORG" );
  if (org)
    if (*org != curCompany) {
      curCompany = *org;
      updateFileAs();
    }
}

void ContactDialog::updateFileAs()
{
    kdDebug()<< "updateFileAs\n";
  cbFileAs->clear();
  QString surnameFirst;
  if (ce->find( "N" )) {
    cbFileAs->insertItem( *ce->find( "N" ));
    cbFileAs->setCurrentItem( 0 );
    //    cbFileAs->updateContact();
    if (ce->find( "X-LastName" )) {
      surnameFirst += *ce->find( "X-LastName" );
      if ((ce->find( "X-FirstName" )) || (ce->find( "X-MiddleName" )))
	surnameFirst += ", ";
      if (ce->find( "X-FirstName" ))
	surnameFirst += *ce->find( "X-FirstName" )  + " ";
      if (ce->find( "X-MiddleName" ))
	surnameFirst += *ce->find( "X-MiddleName" );
      surnameFirst = surnameFirst.simplifyWhiteSpace();
      if (surnameFirst != *ce->find( "N" ))
	cbFileAs->insertItem( surnameFirst );
    } else
      surnameFirst = *ce->find( "N" );
    if (ce->find( "ORG" )) {
      cbFileAs->insertItem( *ce->find( "ORG" ) + " (" + surnameFirst + ")");
      cbFileAs->insertItem( surnameFirst + " (" + *ce->find( "ORG" ) + ")");
    }
  }
  else if (ce->find( "ORG" )) {
    cbFileAs->insertItem( *ce->find( "ORG" ) );
    cbFileAs->setCurrentItem( 0 );
    //    cbFileAs->updateContact();
  }
}

// We don't want to reparse the "N" field if the newNameDialog
// has been used to enter the name
void ContactDialog::parseName()
{
    kdDebug()<< "parseName()\n";
  if (!ce->find( ".AUXCONTACT-N" ))
    return;
  //  qDebug( ".AUX" + *ce->find( ".AUXCONTACT-N" ) + " curname " + curName);
  if (*ce->find( ".AUXCONTACT-N" ) == curName)
    return;
  curName = (*ce->find( ".AUXCONTACT-N" )).simplifyWhiteSpace();
  //  qDebug( "curName " + curName );
  ce->replace( ".AUXCONTACT-N", new QString( curName ));
  QString name = curName;
  QString prefix;
  QString suffix;
  QString first;
  QString middle;
  QString last;

  name = name.simplifyWhiteSpace();
  if (name.find( i18n( "the" ), 0, false ) != 0) {
    QString sTitle[] = {
      i18n( "Doctor" ), i18n( "Dr." ), i18n( "Dr" ), i18n( "Miss" ),
      i18n ( "Mr." ), i18n( "Mr" ), i18n( "Mrs." ), i18n( "Mrs" ),
      i18n( "Ms." ), i18n( "Ms" ), i18n( "Professor" ), i18n( "Prof." ),
      ""
     };
    QString sSuffix[] = {
      i18n( "III" ), i18n( "II" ), i18n( "I" ), i18n( "Junior" ),
      i18n( "Jr." ), i18n( "Senior" ), i18n( "Sr." ),
      ""
    };

    for (int i =0; sTitle[i] != ""; ++i )
      if (name.find( sTitle[i] + " ", 0, false ) == 0) {
	prefix = sTitle[i];
	name = name.right( name.length() - prefix.length() - 1 );
	name = name.simplifyWhiteSpace();
	break;
      }

    for (int i =0; sSuffix[i] != ""; ++i ) {
      QString tSuffix = sSuffix[i];
      int pos = name.length() - tSuffix.length() - 1;
      if ((pos > 0) && (name.findRev( " " + tSuffix, -1, false ) == pos)) {
	suffix = tSuffix;
	name = name.left( pos );
	name = name.simplifyWhiteSpace();
	break;
      }
    }
    if (name.find( " ", 0 ) > 0 ) {
      int pos = name.findRev( " ", -1 );
      last = name.mid( pos + 1);
      name = name.left( pos );
      name = name.simplifyWhiteSpace();
      if (name.find( " ", 0 ) > 0 ) {
	pos = name.find( " ", 0 );
	first = name.left( pos );
	name = name.mid( pos + 1 );
	name = name.simplifyWhiteSpace();
	middle = name;
      }
      else
	first = name;
    }
    else
      last = name;
  }
  ce->replace( "N", new QString( curName ));
  ce->replace( "X-Title", new QString( prefix ) );
  ce->replace( "X-FirstName", new QString( first ) );
  ce->replace( "X-MiddleName", new QString( middle ) );
  ce->replace( "X-LastName", new QString( last ) );
  ce->replace( "X-Suffix", new QString( suffix ) );

  updateFileAs();
}

AddressDialog::AddressDialog( QWidget *parent,
			      QString entryField,
			      ContactEntry *ce,
			      bool modal )
 : QDialog( parent, "", modal ), entryField( entryField), ce( ce )
{
  QString sCountry[] = {
    i18n( "Afghanistan" ), i18n( "Albania" ), i18n( "Algeria" ),
    i18n( "American Samoa" ), i18n( "Andorra" ), i18n( "Angola" ),
    i18n( "Anguilla" ), i18n( "Antarctica" ), i18n( "Antigua and Barbuda" ),
    i18n( "Argentina" ), i18n( "Armenia" ), i18n( "Aruba" ),
    i18n( "Ashmore and Cartier Islands" ), i18n( "Australia" ),
    i18n( "Austria" ), i18n( "Azerbaijan" ), i18n( "Bahamas" ),
    i18n( "Bahrain" ), i18n( "Bangladesh" ), i18n( "Barbados" ),
    i18n( "Belarus" ), i18n( "Belgium" ), i18n( "Belize" ),
    i18n( "Benin" ), i18n( "Bermuda" ), i18n( "Bhutan" ),
    i18n( "Bolivia" ), i18n( "Bosnia and Herzegovina" ), i18n( "Botswana" ),
    i18n( "Brazil" ), i18n( "Brunei" ), i18n( "Bulgaria" ),
    i18n( "Burkina Faso" ), i18n( "Burundi" ), i18n( "Cambodia" ),
    i18n( "Cameroon" ), i18n( "Canada" ), i18n( "Cape Verde" ),
    i18n( "Cayman Islands" ), i18n( "Central African Republic" ),
    i18n( "Chad" ), i18n( "Chile" ), i18n( "China" ), i18n( "Colombia" ),
    i18n( "Comoros" ), i18n( "Congo" ), i18n( "Congo, Dem. Rep." ),
    i18n( "Costa Rica" ), i18n( "Croatia" ),
    i18n( "Cuba" ), i18n( "Cyprus" ), i18n( "Czech Republic" ),
    i18n( "Denmark" ), i18n( "Djibouti" ),
    i18n( "Dominica" ), i18n( "Dominican Republic" ), i18n( "Ecuador" ),
    i18n( "Egypt" ), i18n( "El Salvador" ), i18n( "Equatorial Guinea" ),
    i18n( "Eritrea" ), i18n( "Estonia" ), i18n( "England" ),
    i18n( "Ethiopia" ), i18n( "European Union" ), i18n( "Faroe Islands" ),
    i18n( "Fiji" ), i18n( "Finland" ), i18n( "France" ),
    i18n( "French Polynesia" ), i18n( "Gabon" ), i18n( "Gambia" ),
    i18n( "Georgia" ), i18n( "Germany" ), i18n( "Ghana" ),
    i18n( "Greece" ), i18n( "Greenland" ), i18n( "Grenada" ),
    i18n( "Guam" ), i18n( "Guatemala" ), i18n( "Guinea" ),
    i18n( "Guinea-Bissau" ), i18n( "Guyana" ), i18n( "Haiti" ),
    i18n( "Holland" ), i18n( "Honduras" ), i18n( "Hong Kong" ),
    i18n( "Hungary" ), i18n( "Iceland" ), i18n( "India" ), i18n( "Indonesia" ),
    i18n( "Iran" ), i18n( "Iraq" ), i18n( "Ireland" ),
    i18n( "Israel" ), i18n( "Italy" ), i18n( "Ivory Coast" ),
    i18n( "Jamaica" ), i18n( "Japan" ), i18n( "Jordan" ),
    i18n( "Kazakhstan" ), i18n( "Kenya" ), i18n( "Kiribati" ),
    i18n( "Korea, North" ), i18n( "Korea, South" ),
    i18n( "Kuwait" ), i18n( "Kyrgyzstan" ), i18n( "Laos" ),
    i18n( "Latvia" ), i18n( "Lebanon" ), i18n( "Lesotho" ),
    i18n( "Liberia" ), i18n( "Libya" ), i18n( "Liechtenstein" ),
    i18n( "Lithuania" ), i18n( "Luxembourg" ), i18n( "Macau" ),
    i18n( "Madagascar" ), i18n( "Malawi" ), i18n( "Malaysia" ),
    i18n( "Maldives" ), i18n( "Mali" ), i18n( "Malta" ),
    i18n( "Marshall Islands" ), i18n( "Martinique" ), i18n( "Mauritania" ),
    i18n( "Mauritius" ), i18n( "Mexico" ),
    i18n( "Micronesia, Federated States Of" ), i18n( "Moldova" ),
    i18n( "Monaco" ), i18n( "Mongolia" ), i18n( "Montserrat" ),
    i18n( "Morocco" ), i18n( "Mozambique" ), i18n( "Myanmar" ),
    i18n( "Namibia" ),
    i18n( "Nauru" ), i18n( "Nepal" ), i18n( "Netherlands" ),
    i18n( "Netherlands Antilles" ), i18n( "New Caledonia" ),
    i18n( "New Zealand" ), i18n( "Nicaragua" ), i18n( "Niger" ),
    i18n( "Nigeria" ), i18n( "Niue" ), i18n( "North Korea" ),
    i18n( "Northern Ireland" ), i18n( "Northern Mariana Islands" ),
    i18n( "Norway" ), i18n( "Oman" ), i18n( "Pakistan" ), i18n( "Palau" ),
    i18n( "Palestinian" ), i18n( "Panama" ), i18n( "Papua New Guinea" ),
    i18n( "Paraguay" ), i18n( "Peru" ), i18n( "Philippines" ),
    i18n( "Poland" ), i18n( "Portugal" ), i18n( "Puerto Rico" ),
    i18n( "Qatar" ), i18n( "Romania" ), i18n( "Russia" ), i18n( "Rwanda" ),
    i18n( "St. Kitts and Nevis" ), i18n( "St. Lucia" ),
    i18n( "St. Vincent and the Grenadines" ), i18n( "San Marino" ),
    i18n( "Sao Tome and Principe" ), i18n( "Saudi Arabia" ),
    i18n( "Senegal" ), i18n( "Serbia & Montenegro" ), i18n( "Seychelles" ),
    i18n( "Sierra Leone" ), i18n( "Singapore" ), i18n( "Slovakia" ),
    i18n( "Slovenia" ), i18n( "Solomon Islands" ), i18n( "Somalia" ),
    i18n( "South Africa" ), i18n( "South Korea" ), i18n( "Spain" ),
    i18n( "Sri Lanka" ), i18n( "St. Kitts and Nevis" ), i18n( "Sudan" ),
    i18n( "Suriname" ), i18n( "Swaziland" ), i18n( "Sweden" ),
    i18n( "Switzerland" ), i18n( "Syria" ), i18n( "Taiwan" ),
    i18n( "Tajikistan" ), i18n( "Tanzania" ), i18n( "Thailand" ),
    i18n( "Tibet" ), i18n( "Togo" ), i18n( "Tonga" ),
    i18n( "Trinidad and Tobago" ), i18n( "Tunisia" ), i18n( "Turkey" ),
    i18n( "Turkmenistan" ), i18n( "Turks and Caicos Islands" ),
    i18n( "Tuvalu" ), i18n( "Uganda " ), i18n( "Ukraine" ),
    i18n( "United Arab Emirates" ), i18n( "United Kingdom" ),
    i18n( "United States" ), i18n( "Uruguay" ), i18n( "Uzbekistan" ),
    i18n( "Vanuatu" ), i18n( "Vatican City" ), i18n( "Venezuela" ),
    i18n( "Vietnam" ), i18n( "Western Samoa" ), i18n( "Yemen" ),
    i18n( "Yugoslavia" ), i18n( "Zaire" ), i18n( "Zambia" ),
    i18n( "Zimbabwe" ),
    ""
  };

  setCaption( i18n( "Address" ));
  QGridLayout *hb = new QGridLayout( this, 1, 2, 10 );
  hb->setSpacing( 5 );

  QGroupBox *gb = new QGroupBox( this );
  gb->setTitle( i18n( "Address details" ));
  QGridLayout *lay = new QGridLayout( gb, 6, 2, 12 );
  lay->setSpacing( 5 );
  lay->addWidget( new QFrame( gb ), 0, 0 );
  lay->addWidget( new QFrame( gb ), 0, 1 );

  QLabel *lStreet = new QLabel( i18n( "Street" ), gb );
  lStreet->setAlignment( QLabel::AlignTop | QLabel::AlignLeft );
  lay->addWidget( lStreet, 1, 0 );
  mleStreet = new QMultiLineEdit( gb );
  lay->addWidget( mleStreet, 1, 1 );
  if (ce->find( entryField + "Street" ))
    mleStreet->setText( *ce->find( entryField + "Street" ));
  mleStreet->setMinimumSize( mleStreet->sizeHint() );
  lay->addWidget( new QLabel( i18n( "City" ), gb ), 2, 0 );
  leCity = new QLineEdit( gb );
  if (ce->find( entryField + "City" ))
    leCity->setText( *ce->find( entryField + "City" ));
  lay->addWidget( leCity, 2, 1 );
  lay->addWidget( new QLabel( i18n( "State/Province" ), gb ), 3, 0 );
  leState = new QLineEdit( gb );
  if (ce->find( entryField + "State" ))
    leState->setText( *ce->find( entryField + "State" ));
  lay->addWidget( leState, 3, 1 );
  lay->addWidget( new QLabel( i18n( "Zip/Postal Code" ), gb ), 4, 0 );
  lePostal = new QLineEdit( gb );
  if (ce->find( entryField + "PostalCode" ))
    lePostal->setText( *ce->find( entryField + "PostalCode" ));
  lay->addWidget( lePostal, 4, 1 );

  lay->addWidget( new QLabel( i18n("Country"), gb ), 5, 0 );
  cbCountry = new QComboBox( true, gb );
  QString curCountry;
  QStringList sCountryList;
  int cbNum = -1;
  if (ce->find( entryField + "Country" ))
    curCountry = *ce->find( entryField + "Country" );
  for (int i =0; sCountry[i] != ""; ++i )
    sCountryList.append( sCountry[i] );
  sCountryList.sort();
  cbCountry->insertStringList( sCountryList );
  cbNum = sCountryList.findIndex( curCountry );
  cbCountry->setAutoCompletion( true );
  lay->addWidget( cbCountry, 5, 1 );

  QString country = KGlobal::locale()->country();
  // Try to guess the country the user is in depending
  // on their preferred language.
  // Imperfect but the best I could do.

  QString GuessLanguage[] = {
    "C", "en", "en_AU", "en_UK", "en_NZ", "en_ZA", "da",
    "de", "el", "es", "fi", "fr", "he",
    "hu", "hr", "is", "it", "ko", "nl",
    "no", "pl", "pt", "pt_BR", "ro", "ru",
    "sv", "tr", "zh_CN.GB2312", "zh_TW.Big5", "et",
    ""
  };
  QCString GuessCountry[] = {
    "United States", "United States", "Australia", "United Kingdom",
    "New Zealand", "South Africa", "Denmark",
    "Germany", "Greece", "Spain", "Finland", "French", "Israel",
    "Hungary", "Croatia", "Iceland", "Italy", "South Korea", "Holland",
    "Norway", "Poland", "Portugal", "Brazil", "Romania", "Russia",
    "Sweden", "Turkey", "China", "Taiwan", "Estonia",
    ""
  };

  int langNum = -1;
  if (cbNum == -1) {
    for (langNum =0; country != GuessLanguage[langNum]; ++langNum )
      if (GuessLanguage[langNum] == "")
	break;
    if (GuessLanguage[langNum] != "")
      cbNum = sCountryList.findIndex( i18n( GuessCountry[langNum] ) );
    if (sCountry[cbNum] == "")
      cbNum = -1;
  }
  if (cbNum != -1)
    cbCountry->setCurrentItem( cbNum );
  if (curCountry != "")
    cbCountry->setEditText( curCountry );
  hb->addWidget( gb, 0, 0 );

  QFrame *tf = new QFrame( this );
  QVBoxLayout *lay1 = new QVBoxLayout( tf, 10 );
  QPushButton *pbOk = new QPushButton( i18n("OK"), tf );
  lay1->addWidget( pbOk, 0 );
  QPushButton *pbCancel = new QPushButton( i18n("Cancel"), tf );
  lay1->addWidget( pbCancel, 0 );
  lay1->addStretch( 1 );  // Fix the other widgets in place

  hb->addWidget( tf, 0, 1 );
  hb->activate();
  connect( pbOk, SIGNAL( clicked() ), this, SLOT( AddressOk()));
  connect( pbCancel, SIGNAL( clicked() ), this, SLOT( reject()));
}

void AddressDialog::AddressOk()
{
  QString newAddress = mleStreet->text() + "\n" + leCity->text() + "\n" + leState->text() + "\n" + lePostal->text() + "\n" + cbCountry->currentText();

  ce->replace( entryField, new QString( newAddress ));
  ce->replace( entryField + "City", new QString( leCity->text() ));
  ce->replace( entryField + "Country", new QString( cbCountry->currentText() ));
  ce->replace( entryField + "PostalCode", new QString( lePostal->text() ));
  ce->replace( entryField + "State", new QString( leState->text() ));
  ce->replace( entryField + "Street", new QString( mleStreet->text() ));

  accept();
}

NameDialog::NameDialog( QWidget *parent, ContactEntry *ce, bool modal )
  : QDialog( parent, "", modal ), ce( ce )
{
  QStringList sTitle;
  QStringList sSuffix;

  sTitle += i18n( "Dr." );
  sTitle += i18n( "Miss" );
  sTitle += i18n( "Mr." );
  sTitle += i18n( "Mrs." );
  sTitle += i18n( "Ms." );
  sTitle += i18n( "Prof." );
  sTitle.sort();

  sSuffix += i18n( "I" );
  sSuffix += i18n( "II" );
  sSuffix += i18n( "III" );
  sSuffix += i18n( "Jr." );
  sSuffix += i18n( "Sr." );
  sSuffix.sort();

  setCaption( i18n( "Full Name" ));
  QGridLayout *hb = new QGridLayout( this, 1, 2, 10 );
  hb->setSpacing( 5 );

  QGroupBox *gb = new QGroupBox( this );
  gb->setTitle( i18n("Name details") );
  QGridLayout *lay = new QGridLayout( gb, 6, 2, 12 );
  lay->setSpacing( 5 );
  lay->addWidget( new QFrame( gb ), 0, 0 );
  lay->addWidget( new QFrame( gb ), 0, 1 );

  lay->addWidget( new QLabel( i18n("Title"), gb ),1,0);
  cbTitle = new QComboBox( true, gb );
  for ( int i = 0; i < (int)sTitle.count(); ++i )
    cbTitle->insertItem( sTitle[i] );
  if (ce->find( "X-Title" ))
    cbTitle->setEditText( *ce->find( "X-Title" ));
  else
    cbTitle->setEditText( "" );
  lay->addWidget( cbTitle,1,1 );

  lay->addWidget( new QLabel( i18n("First"), gb ), 2,0);
  leFirst = new QLineEdit( gb );
  if (ce->find( "X-FirstName" ))
    leFirst->setText( *ce->find( "X-FirstName" ));
  else
    leFirst->setText( "" );
  lay->addWidget( leFirst, 2, 1 );
  leFirst->setMinimumSize( leFirst->sizeHint() );

  lay->addWidget( new QLabel( i18n("Middle"), gb ), 3, 0 );
  leMiddle = new QLineEdit( gb );
  if (ce->find( "X-MiddleName" ))
    leMiddle->setText( *ce->find( "X-MiddleName" ));
  else
    leMiddle->setText( "" );
  lay->addWidget( leMiddle,3 ,1 );

  lay->addWidget( new QLabel( i18n("Last"), gb ), 4, 0 );
  leLast = new QLineEdit( gb );
  if (ce->find( "X-LastName" ))
    leLast->setText( *ce->find( "X-LastName" ));
  else
    leLast->setText( "" );
  lay->addWidget( leLast, 4, 1 );

  lay->addWidget( new QLabel( i18n("Suffix"), gb ), 5, 0 );
  cbSuffix = new QComboBox( true, gb );
  for ( int i = 0; i < (int)sSuffix.count(); ++i )
    cbSuffix->insertItem( sSuffix[i] );
  if (ce->find( "X-Suffix" ))
    cbSuffix->setEditText( *ce->find( "X-Suffix" ));
  else
    cbSuffix->setEditText( "" );
  lay->addWidget( cbSuffix, 5, 1 );

  hb->addWidget( gb, 0, 0 );

  QFrame *tf = new QFrame( this );
  QVBoxLayout *lay1 = new QVBoxLayout( tf, 10 );
  QPushButton *pbOk = new QPushButton( i18n("OK"), tf );
  lay1->addWidget( pbOk, 0 );
  QPushButton *pbCancel = new QPushButton( i18n("Cancel"), tf );
  lay1->addWidget( pbCancel, 0 );
  lay1->addStretch( 1 );  // Fix the other widgets in place

  hb->addWidget( tf, 0, 1 );
  hb->activate();
  connect( pbOk, SIGNAL( clicked() ), this, SLOT( NameOk() ));
  connect( pbCancel, SIGNAL( clicked()), this, SLOT( reject() ));
  leFirst->setFocus();
}

void NameDialog::polish()
{
  setMaximumHeight( height() );
}

void NameDialog::NameOk()
{
  QString name = cbTitle->currentText() + " " +
    leFirst->text() + " " +
    leMiddle->text() + " " +
    leLast->text() + " "
    + cbSuffix->currentText();
  kdDebug() << "NameOk " << name << endl;
  ce->replace( "N", new QString( name.simplifyWhiteSpace() ));
  ce->replace( "X-Title", new QString( cbTitle->currentText() ));
  ce->replace( "X-FirstName", new QString( leFirst->text() ));
  ce->replace( "X-MiddleName", new QString( leMiddle->text() ));
  ce->replace( "X-LastName", new QString( leLast->text() ));
  ce->replace( "X-Suffix", new QString( cbSuffix->currentText() ));
  accept();
}

void ContactDialog::setSheet(int sheet)
{
  QStringList names, fields;
  if (!Attributes::instance()->nameFieldList( sheet, &names, &fields )) {
    fields = ce->custom();
    fields.sort();
    for (int i = 0; i < (int)fields.count(); ++i )
      names += fields[i].mid( 9 );
  }
  delete vs;
  vs = new NameValueSheet( 0, (int)names.count(), names, fields, ce );
  vp->setSheet( vs );
}

NewFieldDialog::NewFieldDialog( QWidget *parent, bool modal )
  : QDialog( parent, "", modal )
    {
  setCaption( i18n( "Create Custom Field" ));

  QGridLayout *hbl = new QGridLayout( this, 3, 2, 10 );
  hbl->setSpacing( 5 );

  QLabel *lField = new QLabel( i18n( "Field name" ), this );
  hbl->addWidget( lField, 0, 0 );
  lField->setAlignment( QLabel::AlignTop | QLabel::AlignLeft );
  leField = new QLineEdit( this );
  hbl->addWidget( leField, 0, 1 );
  QLabel *lValue = new QLabel( i18n( "Value" ), this );
  hbl->addWidget( lValue, 1, 0 );
  lValue->setAlignment( QLabel::AlignTop | QLabel::AlignLeft );
  leValue = new QLineEdit( this );
  hbl->addWidget( leValue, 1, 1 );

  QHBox *tf = new QHBox( this );
  tf->setSpacing( 10 );
  QPushButton *pbOk = new QPushButton( i18n("OK"), tf );
  QPushButton *pbCancel = new QPushButton( i18n("Cancel"), tf );

  hbl->addMultiCellWidget( tf, 2, 2, 0, 1, QGridLayout::AlignRight );

  hbl->activate();
  setMinimumSize( sizeHint() );
  resize( sizeHint() );
  setMaximumHeight( height() );

  connect( pbOk, SIGNAL( clicked() ), this, SLOT( accept() ));
  connect( pbCancel, SIGNAL( clicked()), this, SLOT( reject() ));
};

QString NewFieldDialog::field() const
{
  return "X-CUSTOM-" + leField->text();
}

QString NewFieldDialog::value() const
{
  return leValue->text();
}
#include "entryeditorwidget.moc"
