/* This file is part of KDE PIM
    Copyright (C) 1999 Don Sanders <sanders@kde.org>

    License: BSD
*/

#include "namevaluewidget.h"
#include <qlayout.h>
#include <qstringlist.h>
#include <qcombobox.h>
#include "contactentry.h"

#include <klocale.h>
#include <kdebug.h>

NameValueSheet::NameValueSheet( QWidget *parent,
				int rows,
				QStringList name,
				QStringList entryField,
				ContactEntry *ce )
 : QFrame( parent ), lCell( 0 ), rows( rows )
{
  temp = new QLabel( i18n( "Name" ) + i18n ( "Name" ), 0, "temp" );
  minNameWidth = temp->sizeHint().width();
  minNameWidth = fontMetrics().width( i18n( "Name" ) + i18n ( "Name" ) ) + 8;
  minNameHeight = temp->sizeHint().height();
  int minWidth;
  int positiveRows;
  lCell = temp;
  if (rows < 1)
    positiveRows = 1;
  else
    positiveRows = rows;

  QGridLayout *lay2 = new QGridLayout( this, positiveRows, 2, 0 );
  lay2->setSpacing( -1 );
  for( int i = 0; i < rows; ++i ) {
    lCell = new QLabel( name[i], this );
    lCell->setFrameStyle( QFrame::Box | QFrame::Plain );
    lCell->updateGeometry();
    minWidth = fontMetrics().width( name[i] ) + 8;
    kdDebug() << "minWidth " << minWidth << endl;
    //    minWidth = lCell->sizeHint().width();
    if (minWidth > minNameWidth)
      minNameWidth = minWidth;
    lCell->setMinimumWidth( minNameWidth );

    lay2->addWidget( lCell, i, 0 );

    QFrame *leFrame = new QFrame( this );
    leFrame->setMargin( 0 );
    leFrame->setFrameStyle( QFrame::Box | QFrame::Plain );
    QBoxLayout *leBox = new QBoxLayout( leFrame, QBoxLayout::LeftToRight, 2, 0 );
    QLineEdit *leCell = new ContactLineEdit( leFrame, entryField[i].ascii(), ce );
    leFrame->setBackgroundColor( leCell->backgroundColor() );
    leCell->setFrame( false );
    leBox->addWidget( leCell );
    lay2->addWidget( leFrame, i, 1 );
  }
  if (rows == 0) {
    QFrame *filler = new QFrame( this );
    lay2->addWidget( filler, 0, 1 );
  }
  setMaximumHeight( (lCell->height() - verticalTrim*2) * rows );
  kdDebug() << "minNameWidth " << minNameWidth << endl;
}

NameValueSheet::~NameValueSheet()
{
  delete temp;
}

QSize NameValueSheet::cellSize()
{
  kdDebug() << "cellSize " << minNameWidth << endl;
  if (rows == 0)
    return QSize( minNameWidth, minNameHeight - verticalTrim );
  //  return QSize( lCell->size().width(), lCell->size().height() - verticalTrim );
  return QSize( minNameWidth, lCell->size().height() - verticalTrim );
}

NameValueFrame::NameValueFrame( QWidget *parent, NameValueSheet* vs )
 : QScrollView( parent ), vs( vs )
{
  setFrameStyle( QFrame::WinPanel | QFrame::Sunken  );
  lName = new QLabel( i18n( "Name" ), this );
  lName->setFrameStyle( QFrame::WinPanel | QFrame::Raised );
  lName->setMinimumSize( lName->sizeHint() );
  lValue = new QLabel( i18n( "Value" ), this );
  lValue->setFrameStyle( QFrame::WinPanel | QFrame::Raised );
  lValue->setMinimumSize( lValue->sizeHint () );

  setMargins( 0, lName->sizeHint().height() - 1, 0, 0 );
  enableClipper( true );
  setHScrollBarMode( QScrollView::AlwaysOff );
  addChild( vs );
  setResizePolicy( QScrollView::AutoOne );
  viewport()->setBackgroundColor( vs->backgroundColor() );
}

#include <kapp.h>
void NameValueFrame::setSheet( NameValueSheet* vs )
{
  this->vs = vs;
  vs->setMinimumSize( vs->sizeHint() );
  addChild( vs );
  showChild( vs, true );
  kapp->processEvents(1);
  kdDebug() << "XvisibleWidth " << visibleWidth() << endl;
  kdDebug() << "XvisibleWidth " << visibleWidth() << endl;
  resizeContents( vs->width(), vs->height() );
  lName->setMinimumSize( vs->cellSize().width(), lName->height() );
  lName->resize( vs->cellSize().width(), lName->height() );
  lName->updateGeometry();
  lValue->setMinimumSize( visibleWidth() - lName->width(), lName->height() );
  lValue->resize( visibleWidth() - lName->width(), lName->height() );
  lName->move( 2, 2 );
  lValue->move( lName->width() + 2, 2 );
  vs->resize( visibleWidth(), vs->height() );

  kdDebug() << "cellWidth " << vs->cellSize().width() << endl;
  kdDebug() << "visibleWidth " << visibleWidth() << endl;
}

void NameValueFrame::resizeEvent(QResizeEvent* e)
{
  QScrollView::resizeEvent( e );
  vs->resize( visibleWidth(), vs->height() );
  lName->resize( vs->cellSize() );
  lValue->resize( visibleWidth() - lName->width(), lName->height() );
  lName->move( 2, 2 );
  lValue->move( lName->width() + 2, 2 );
}

ContactLineEdit::ContactLineEdit( QWidget * parent,
				  const char * name,
				  ContactEntry *ce )
 : QLineEdit( parent, name ), ce( ce )
{
  if (ce->find( name ))
    setText( *ce->find( name ));
  connect( ce, SIGNAL( changed() ), this, SLOT( sync() ));
}

void ContactLineEdit::focusOutEvent ( QFocusEvent * )
{	
  ce->replace( QString( name()), new QString( text()) );
}

void ContactLineEdit::setName( const char *name )
{
  setText( "" );
  QLineEdit::setName( name );
  sync();
}

void ContactLineEdit::sync()
{
  const QString *value = ce->find( name() );
  if ((value) && (*value != text()))
    setText( *value );
}

ContactMultiLineEdit::ContactMultiLineEdit( QWidget * parent,
					    const char * name,
					    ContactEntry *ce )
 : QMultiLineEdit( parent, name ), ce( ce )
{
  connect( ce, SIGNAL( changed() ), this, SLOT( sync() ));
}

void ContactMultiLineEdit::focusOutEvent( QFocusEvent * )
{	
  ce->replace( QString( name()), new QString( text()) );
}

void ContactMultiLineEdit::setName( const char *name )
{
  setText( "" );
  QMultiLineEdit::setName( name );
  sync();
}

void ContactMultiLineEdit::sync()
{
  const QString *value = ce->find( name() );
  if ((value) && (*value != text()))
    setText( *value );
}

FileAsComboBox::FileAsComboBox( QWidget * parent,
				const char * name,
				ContactEntry *ce )
 : QComboBox( true, parent, name ), ce( ce )
{
  connect( ce, SIGNAL( changed() ), this, SLOT( sync() ));
}

void FileAsComboBox::updateContact()
{	
  kdDebug() << "FileAsComboBox::focusOutEvent" << endl;
  kdDebug() << currentText() << endl;
  ce->replace( QString( name()), new QString( currentText()) );
}

void FileAsComboBox::setName( const char *name )
{
  setEditText( "" );
  QComboBox::setName( name );
  sync();
}

void FileAsComboBox::sync()
{
  const QString *value = ce->find( name() );
  if ((value) && (*value != currentText()))
    setEditText( *value );
}

ContactComboBox::ContactComboBox( QWidget *parent )
 : QComboBox( false, parent), buddy( 0 )
{}

void ContactComboBox::setBuddy( QWidget *buddy )
{
  this->buddy = buddy;
  connect( this, SIGNAL( activated(int)), this, SLOT( updateBuddy(int)));
}

void ContactComboBox::insertItem( const QString & text, const QString & vText )
{
  QComboBox::insertItem( text );
  vlEntryField.append( vText );
}

void ContactComboBox::updateBuddy( int index )
{
  if (index < (int)vlEntryField.count())
    if (buddy)
      buddy->setName( vlEntryField[index].ascii() );
};

QString ContactComboBox::currentEntryField()
{
  if (currentItem() < (int)vlEntryField.count())
    return vlEntryField[currentItem()];
  else
    return "";
};

#include "namevaluewidget.moc"
