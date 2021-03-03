/****************************************************************************
** $Id: qt/src/dialogs/qwizard.cpp   2.3.2   edited 2001-08-18 $
**
** Implementation of QWizard class.
**
** Created : 990124
**
** Copyright (C) 1999-2000 Trolltech AS.  All rights reserved.
**
** This file is part of the dialogs module of the Qt GUI Toolkit.
**
** This file may be distributed under the terms of the Q Public License
** as defined by Trolltech AS of Norway and appearing in the file
** LICENSE.QPL included in the packaging of this file.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** Licensees holding valid Qt Enterprise Edition or Qt Professional Edition
** licenses may use this file in accordance with the Qt Commercial License
** Agreement provided with the Software.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/pricing.html or email sales@trolltech.com for
**   information about Qt Commercial License Agreements.
** See http://www.trolltech.com/qpl/ for QPL licensing information.
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#include "qwizard.h"

#ifndef QT_NO_WIZARD

#include "qlayout.h"
#include "qpushbutton.h"
#include "qlabel.h"
#include "qwidgetstack.h"
#include "qapplication.h"
#include "qvector.h"
#include "qpainter.h"
#include "qaccel.h"

// NOT REVISED
/*! \class QWizard qwizard.h

  \ingroup abstractwidgets
  \ingroup organizers

  \brief The QWizard class provides a framework for easily writing wizards.

  A wizard is a dialog that consists of a sequential number of steps,
  each consisting of a single page.  QWizard provides a title for each
  page, and "Next", "Back", "Finish", "Cancel" and "Help" buttons, as
  appropriate.

*/


class QWizardPrivate
{
public:
    struct Page {
	Page( QWidget * widget, const QString & title ):
	    w( widget ), t( title ), back( 0 ),
	    backEnabled( TRUE ), nextEnabled( TRUE ), finishEnabled( FALSE ),
	    helpEnabled( TRUE ),
	    appropriate( TRUE )
	{}
	QWidget * w;
	QString t;
	QWidget * back;
	bool backEnabled;
	bool nextEnabled;
	bool finishEnabled;
	bool helpEnabled;
	bool appropriate;
    };

    QVBoxLayout * v;
    Page * current;
    QWidgetStack * ws;
    QVector<Page> pages;
    QLabel * title;
    QPushButton * backButton;
    QPushButton * nextButton;
    QPushButton * finishButton;
    QPushButton * cancelButton;
    QPushButton * helpButton;

    QFrame * hbar1, * hbar2;

    QAccel * accel;
    int backAccel;
    int nextAccel;

    Page * page( const QWidget * w )
    {
	if ( !w )
	    return 0;
	int i = pages.size();
	while( --i >= 0 && pages[i] && pages[i]->w != w ) { }
	return i >= 0 ? pages[i] : 0;
    }

};


/*!  Constructs an empty wizard dialog. */

QWizard::QWizard( QWidget *parent, const char *name, bool modal,
		  WFlags f )
    : QDialog( parent, name, modal, f )
{
    d = new QWizardPrivate();
    d->current = 0; // not quite true, but...
    d->ws = new QWidgetStack( this );
    d->pages.setAutoDelete( TRUE );
    d->title = 0;

    // create in nice tab order
    d->nextButton = new QPushButton( this, "next" );
    d->finishButton = new QPushButton( this, "finish" );
    d->helpButton = new QPushButton( this, "help" );
    d->backButton = new QPushButton( this, "back" );
    d->cancelButton = new QPushButton( this, "cancel" );

    d->ws->installEventFilter( this );

    d->v = 0;
    d->hbar1 = 0;
    d->hbar2 = 0;

    d->cancelButton->setText( tr( "Cancel" ) );
    d->backButton->setText( tr( "< Back" ) );
    d->nextButton->setText( tr( "Next >" ) );
    d->finishButton->setText( tr( "Finish" ) );
    d->helpButton->setText( tr( "Help" ) );

    d->nextButton->setDefault( TRUE );

    connect( d->backButton, SIGNAL(clicked()),
	     this, SLOT(back()) );
    connect( d->nextButton, SIGNAL(clicked()),
	     this, SLOT(next()) );
    connect( d->finishButton, SIGNAL(clicked()),
	     this, SLOT(accept()) );
    connect( d->cancelButton, SIGNAL(clicked()),
	     this, SLOT(reject()) );
    connect( d->helpButton, SIGNAL(clicked()),
	     this, SLOT(help()) );

    d->accel = new QAccel( this, "arrow-key accel" );
    d->backAccel = d->accel->insertItem( Qt::ALT + Qt::Key_Left );
    d->accel->connectItem( d->backAccel, this, SLOT(back()) );
    d->nextAccel = d->accel->insertItem( Qt::ALT + Qt::Key_Right );
    d->accel->connectItem( d->nextAccel, this, SLOT(next()) );
}


/*! Destructs the object and frees any allocated resources, including,
of course, all pages and controllers.
*/

QWizard::~QWizard()
{
    delete d;
}


/*!  \reimp  */

void QWizard::show()
{
    if ( d->current )
	showPage( d->current->w );
    else if ( pageCount() > 0 )
	showPage( d->pages[0]->w );
    else
	showPage( 0 );

    QDialog::show();
}


/*! \reimp */

void QWizard::setFont( const QFont & font )
{
    QApplication::postEvent( this, new QEvent( QEvent::LayoutHint ) );
    QDialog::setFont( font );
}


/*!  Adds \a page to the end of the wizard, titled \a title.
*/

void QWizard::addPage( QWidget * page, const QString & title )
{
    if ( !page )
	return;
    if ( d->page( page ) ) {
#if defined(CHECK_STATE)
	qWarning( "QWizard::addPage(): already added %s/%s to %s/%s",
		  page->className(), page->name(),
		  className(), name() );
#endif
	return;
    }
    int i = d->pages.size();
    QWizardPrivate::Page * p = new QWizardPrivate::Page( page, title );
    p->backEnabled = ( i > 0 );

    d->ws->addWidget( page, i );
    d->pages.resize( i+1 );
    d->pages.insert( i, p );
}

/*!
  \fn void QWizard::selected(const QString&)
  This signal is emitted when the page changes, signalling
  the title of the page.
*/


/*!  Makes \a page be the displayed page and emits the selected() signal. */

void QWizard::showPage( QWidget * page )
{
    QWizardPrivate::Page * p = d->page( page );
    if ( p ) {
	setBackEnabled( p->back != 0 );
	setNextEnabled( TRUE );
	d->ws->raiseWidget( page );
	d->current = p;
    }

    layOut();
    updateButtons();
    emit selected( p ? p->t : QString::null );
}


/*!  Returns the number of pages in the wizard. */

int QWizard::pageCount() const
{
    return d->pages.count();
}


/*!
  Called when the user clicks the Back button, this function shows
  the page which the user saw prior to the current one.
*/
void QWizard::back()
{
    if ( d->current && d->current->back )
	showPage( d->current->back );
}


/*!
  Called when the user clicks the Next button, this function shows
  the next appropriate page.
*/
void QWizard::next()
{
    if ( nextButton()->isEnabled() ) {
	int i = 0;
	while( i < (int)d->pages.size() && d->pages[i] &&
	    d->current && d->pages[i]->w != d->current->w )
	    i++;
	i++;
	while( i <= (int)d->pages.size()-1 &&
	    ( !d->pages[i] || !appropriate( d->pages[i]->w ) ) )
	    i++;
	// if we fell of the end of the world, step back
	while ( i > 0 && (i >= (int)d->pages.size() || !d->pages[i] ) )
	    i--;
	if ( d->pages.at( i ) && d->pages.at( i ) != d->current ) {
	    d->pages[i]->back = d->current ? d->current->w : 0;
	    showPage( d->pages[i]->w );
	}
    }
}


/*!
  \fn void QWizard::helpClicked()
  This signal is emitted when the user clicks on the help button.
*/

/*!  This slot either makes the wizard help you, if it can.  The only
way it knows is to emit the helpClicked() signal.
*/

void QWizard::help()
{
    QWidget * page = d->ws->visibleWidget();
    if ( !page )
	return;

#if 0
    if ( page->inherits( "QWizardPage" ) )
	emit ((QWizardPage *)page)->helpClicked();
#endif
    emit helpClicked();
}


void QWizard::setBackEnabled( bool enable )
{
    d->backButton->setEnabled( enable );
    d->accel->setItemEnabled( d->backAccel, enable );
}


void QWizard::setNextEnabled( bool enable )
{
    d->nextButton->setEnabled( enable );
    d->accel->setItemEnabled( d->nextAccel, enable );
}


void QWizard::setHelpEnabled( bool enable )
{
    d->helpButton->setEnabled( enable );
}


/*!
  \obsolete
*/
void QWizard::setFinish( QWidget *, bool )
{
    // Didn't do anything.
}


/*!
  Enables or disables the "Back" button for pages \a w in the wizard.
  By default, all pages have this button.
*/
void QWizard::setBackEnabled( QWidget * w, bool enable )
{
    QWizardPrivate::Page * p = d->page( w );
    if ( !p )
	return;

    p->backEnabled = enable;
    updateButtons();
}


/*!
  Enables or disables the "Next" button for pages \a w in the wizard.
  By default, all pages have this button.
*/
void QWizard::setNextEnabled( QWidget * w, bool enable )
{
    QWizardPrivate::Page * p = d->page( w );
    if ( !p )
	return;

    p->nextEnabled = enable;
    updateButtons();
}


/*!
  Enables or disables the "Finish" button for pages \a w in the wizard.
  By default, \e no pages have this button.
*/
void QWizard::setFinishEnabled( QWidget * w, bool enable )
{
    QWizardPrivate::Page * p = d->page( w );
    if ( !p )
	return;

    p->finishEnabled = enable;
    updateButtons();
}


/*!
  Enables or disables the "Help" button for pages \a w in the wizard.
  By default, all pages have this button.
*/
void QWizard::setHelpEnabled( QWidget * w, bool enable )
{
    QWizardPrivate::Page * p = d->page( w );
    if ( !p )
	return;

    p->helpEnabled = enable;
    updateButtons();
}


/*!  This virtual function returns TRUE if \a w is appropriate for
display in the current context of the wizard, and FALSE if QWizard
should go on.

It is called when the Next button is clicked.

\warning The last page of a wizard will be displayed if nothing else wants
to, and the Next button was enabled when the user clicked.

The default implementation returns whatever was set using
setAppropriate().  The ultimate default is TRUE.
*/

bool QWizard::appropriate( QWidget * w ) const
{
    QWizardPrivate::Page * p = d->page( w );
    return p ? p->appropriate : TRUE;
}


/*!
  Sets whether it is appropriate for \a w to be displayed in the
  current context of the wizard.

  \sa appropriate()
*/
void QWizard::setAppropriate( QWidget * w, bool enable )
{
    QWizardPrivate::Page * p = d->page( w );
    if ( p )
	p->appropriate = enable;
}


void QWizard::updateButtons()
{
    if ( !d->current )
	return;

    setBackEnabled( d->current->backEnabled && d->current->back != 0 );
    setNextEnabled( d->current->nextEnabled );
    d->finishButton->setEnabled( d->current->finishEnabled );
    d->helpButton->setEnabled( d->current->helpEnabled );

    if ( ( d->current->finishEnabled && !d->finishButton->isVisible() ) ||
	 ( d->current->backEnabled && !d->backButton->isVisible() ) ||
	 ( d->current->nextEnabled && !d->nextButton->isVisible() ) ||
	 ( d->current->helpEnabled && !d->helpButton->isVisible() ) )
	layOut();
}


/*!  Returns a pointer to the page currently being displayed by the
wizard.  The wizard does its best to make sure that this value is
never 0, but if you try hard enough it can be.
*/

QWidget * QWizard::currentPage() const
{
    return d->ws->visibleWidget();
}


/*!  Returns the title of \a page.
*/

QString QWizard::title( QWidget * page ) const
{
    QWizardPrivate::Page * p = d->page( page );
    return p ? p->t : QString::null;
}

/*!  Sets the title for the \a page to \a title.
*/

void QWizard::setTitle( QWidget *page, const QString &t )
{
    QWizardPrivate::Page * p = d->page( page );
    if ( p )
	p->t = t;
    if ( page == currentPage() )
	d->title->setText( t );	
}


/*!
  Returns the Back button of the dialog.

  By default, this button is connected to the back()
  slot, which is virtual.
*/
QPushButton * QWizard::backButton() const
{
    return d->backButton;
}


/*!
  Returns the Next button of the dialog.

  By default, this button is connected to the next()
  slot, which is virtual.
*/
QPushButton * QWizard::nextButton() const
{
    return d->nextButton;
}


/*!
  Returns the Finish button of the dialog.

  By default, this button is connected to the QDialog::accept() slot,
  which is virtual so you may reimplement it in a QWizard subclass.
*/
QPushButton * QWizard::finishButton() const
{
    return d->finishButton;
}


/*!
  Returns the Cancel button of the dialog.

  By default, this button is connected to the QDialog::reject() slot,
  which is virtual so you may reimplement it in a QWizard subclass.
*/
QPushButton * QWizard::cancelButton() const
{
    return d->cancelButton;
}


/*!
  Returns the Help button of the dialog.

  By default, this button is connected to the help() slot.
*/
QPushButton * QWizard::helpButton() const
{
    return d->helpButton;
}


/*!  This virtual function is responsible for adding the bottom
divider and buttons below it.

\a layout is the vertical layout of the entire wizard.
*/

void QWizard::layOutButtonRow( QHBoxLayout * layout )
{
    bool hasHelp = FALSE;
    bool hasEarlyFinish = FALSE;

    int i = d->pages.size() - 2;
    while ( !hasEarlyFinish && i >= 0 ) {
	if ( d->pages[i] && d->pages[i]->finishEnabled )
	    hasEarlyFinish = TRUE;
	i--;
    }
    i = 0;
    while ( !hasHelp && i < (int)d->pages.size() ) {
	if ( d->pages[i] && d->pages[i]->helpEnabled )
	    hasHelp = TRUE;
	i++;
    }

    QBoxLayout * h = new QBoxLayout( QBoxLayout::LeftToRight );
    layout->addLayout( h );

    h->addWidget( d->cancelButton );

    h->addStretch( 42 );

    h->addWidget( d->backButton );

    h->addSpacing( 6 );

    if ( hasEarlyFinish ) {
	d->nextButton->show();
	d->finishButton->show();
	h->addWidget( d->nextButton );
	h->addSpacing( 12 );
	h->addWidget( d->finishButton );
    } else if ( d->pages.size() == 0 ||
		d->current->finishEnabled ||
		d->current == d->pages[d->pages.size()-1] ) {
	d->nextButton->hide();
	d->finishButton->show();
	h->addWidget( d->finishButton );
    } else {
	d->nextButton->show();
	d->finishButton->hide();
	h->addWidget( d->nextButton );
    }

    // if last page is disabled - show finished btn. at lastpage-1
    i = d->pages.count()-1;
    if ( i >= 0 && !appropriate( d->pages.at( i )->w ) &&
	 d->current == d->pages.at( d->pages.count()-2 ) ) {
	d->nextButton->hide();
	d->finishButton->show();
	h->addWidget( d->finishButton );
    }

    if ( hasHelp ) {
        h->addSpacing( 12 );
        h->addWidget( d->helpButton );
    } else {
        d->helpButton->hide();
    }
}


/*!  This virtual function is responsible for laying out the title row
and adding the vertical divider between the title and the wizard page.
\a layout is the vertical layout for the wizard, \a title is the title
for this page, and this function is called every time \a title
changes.
*/

void QWizard::layOutTitleRow( QHBoxLayout * layout, const QString & title )
{
    if ( !d->title )
	d->title = new QLabel( this );
    d->title->setText( title );
    layout->addWidget( d->title, 10 );
    d->title->repaint();
}


/*

*/

void QWizard::layOut()
{
    delete d->v;
    d->v = new QVBoxLayout( this, 6, 0, "top-level layout" );

    QHBoxLayout * l;
    l = new QHBoxLayout( 6 );
    d->v->addLayout( l, 0 );
    layOutTitleRow( l, d->current ? d->current->t : QString::null );

    if ( ! d->hbar1 ) {
	d->hbar1 = new QFrame( this, "<hr>", 0, TRUE );
	d->hbar1->setFrameStyle( QFrame::Sunken + QFrame::HLine );
	d->hbar1->setFixedHeight( 12 );
    }

    d->v->addWidget( d->hbar1 );

    d->v->addWidget( d->ws, 10 );

    if ( ! d->hbar2 ) {
	d->hbar2 = new QFrame( this, "<hr>", 0, TRUE );
	d->hbar2->setFrameStyle( QFrame::Sunken + QFrame::HLine );
	d->hbar2->setFixedHeight( 12 );
    }
    d->v->addWidget( d->hbar2 );

    l = new QHBoxLayout( 6 );
    d->v->addLayout( l );
    layOutButtonRow( l );
    d->v->activate();
}


/*! \reimp */

bool QWizard::eventFilter( QObject * o, QEvent * e )
{
    if ( o == d->ws && e && e->type() == QEvent::ChildRemoved ) {
	QChildEvent * c = (QChildEvent*)e;
	if ( c->child() && c->child()->isWidgetType() )
	    removePage( (QWidget *)c->child() );
    }
    return QDialog::eventFilter( o, e );
}


/*!  Removes \a page from this wizard.  Does not delete
  \a page. If \a page is currently being displayed, QWizard will
  display something else.
*/

void QWizard::removePage( QWidget * page )
{
    if ( !page )
	return;

    int i = d->pages.size();
    while( --i >= 0 && d->pages[i] && d->pages[i]->w != page ) { }
    if ( i < 0 )
	return;
    d->pages.remove( i );
    for ( uint j = i; j < d->pages.size() - 1; ++j )
	d->pages.insert( j, d->pages.take( j + 1 ) );
    d->pages.resize( d->pages.size() - 1 );
    d->ws->removeWidget( page );
    if ( pageCount() > 0 )
	showPage( QWizard::page( 0 ) );
}


/*!
  Returns a pointer to page a position \a pos, or 0 if \a pos is out of range.
  The first page has position 0.
*/

QWidget* QWizard::page( int pos ) const
{
    if ( pos >= pageCount() || pos < 0 )
      return 0;

    return d->pages[ pos ]->w;
}

#endif // QT_NO_WIZARD
