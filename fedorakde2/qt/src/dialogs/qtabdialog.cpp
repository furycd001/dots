/****************************************************************************
** $Id: qt/src/dialogs/qtabdialog.cpp   2.3.2   edited 2001-01-26 $
**
** Implementation of QTabDialog class
**
** Created : 960825
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
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

#include "qtabdialog.h"

#ifndef QT_NO_TABDIALOG

#include "qobjectlist.h"
#include "qobjectdict.h"
#include "qtabbar.h"
#include "qtabwidget.h"
#include "qpushbutton.h"
#include "qpainter.h"
#include "qpixmap.h"
#include "qapplication.h"
#include "qtabwidget.h"
#include "qwidgetstack.h"
#include "qlayout.h"

// NOT REVISED
/*!
  \class QTabDialog qtabdialog.h

  \brief The QTabDialog class provides a stack of tabbed widgets.

  \ingroup dialogs

  A tabbed dialog is one in which several "pages" are available, and
  the user selects which page to see and use by clicking on its tab,
  or by pressing the indicated Alt-(letter) key combination.

  QTabDialog does not provide more than one row of tabs, and does not
  provide tabs along the sides or bottom of the pages.  It also does
  not offer any way to find out which page is currently visible or to
  set the visible page.

  QTabDialog provides an OK button and optionally Apply, Cancel,
  Defaults, and Help buttons.

  The normal way to use QTabDialog is to do the following in the
  constructor: <ol> <li> Create a QTabDialog. <li> Create a QWidget
  for each of the pages in the tab dialog, insert children into it,
  set up geometry management for it, and use addTab() to set up a tab
  and keyboard accelerator for it. <li> Set up the buttons for the tab
  dialog (Apply, Cancel and so on). <li> Connect to the
  signals and slots. </ol>

  If you don't call addTab(), the page you have created will not be
  visible.  Please don't confuse the object name you supply to the
  QWidget constructor and the tab label you supply to addTab():
  addTab() takes a name which indicates an accelerator and is
  meaningful and descriptive to the user, while the widget name is
  used primarily for debugging.

  Almost all applications have to connect the applyButtonPressed()
  signal to something.  applyButtonPressed() is emitted when either OK
  or Apply is clicked, and your slot must copy the dialog's state into
  the application.

  There are also several other signals which may be useful. <ul> <li>
  cancelButtonPressed() is emitted when the user clicks Cancel.  <li>
  defaultButtonPressed() is emitted when the user clicks Defaults;
  <li>
  helpButtonPressed() is emitted when the user clicks Help;
  the
  slot it is connected to should reset the state of the dialog to the
  application defaults.  <li> aboutToShow() is emitted at the start of
  show(); if there is any chance that the state of the application may
  change between the creation of the tab dialog and the time it show()
  is called, you must connect this signal to a slot which resets the
  state of the dialog. <li> currentChanged() is emitted when the user
  selects some page. </ul>

  Each tab is either enabled or disabled at any given time.  If a tab
  is enabled, the tab text is drawn in black and the user can select
  that tab.  If it is disabled, the tab is drawn in a different way
  and the user can not select that tab.  Note that even though a tab
  is disabled, the page can still be visible, for example if all of
  the tabs happen to be disabled.

  While tab dialogs can be a very good way to split up a complex
  dialog, it's also very easy to make a royal mess out of a tab
  dialog.  Here is some advice.  For more, see e.g. the <a
  href="http://world.std.com/~uieweb/tabbed.htm">UIE web page on tab
  dialogs.</a>

  <ol><li> Make sure that each page forms a logical whole which is
  adequately described by the label on the tab.

  If two related functions are on different pages, users will often
  not find one of the functions, or will spend far too long searching
  for it.

  <li> Do not join several independent dialogs into one tab dialog.
  Several aspects of one complex dialog is acceptable (such as the
  various aspects of "preferences") but a tab dialog is no substitute
  for a pop-up menu leading to several smaller dialogs.

  The OK button (and the other buttons) apply to the \e entire dialog.
  If the tab dialog is really several independent smaller dialogs,
  users often press Cancel to cancel just the changes he/she has made
  on the current page: Many users will treat that page as independent
  of the other pages.

  <li> Do not use tab dialogs for frequent operations.  The tab dialog
  is probably the most complex widget in common use at the moment, and
  subjecting the user to this complexity during his/her normal use of
  your application is most often a bad idea.

  The tab dialog is good for complex operations which have to be
  performed seldom, like Preferences dialogs.  Not for common
  operations, like setting left/right alignment in a word processor.
  (Often, these common operations are actually independent dialogs and
  should be treated as such.)

  The tab dialog is not a navigational aid, it is an organizational
  aid.  It is a good way to organize aspects of a complex operation
  (such as setting up caching and proxies in a web browser), but a bad
  way to navigate towards a simple operation (such as emptying the
  cache in a web browser - emptying the cache is \e not part of
  setting up the cache, it is a separate and independent operation).

  <li> The changes should take effect when the user presses Apply or
  OK.  Not before.

  Providing Apply, Cancel or OK buttons on the individual pages is
  likely to weaken the users' mental model of how tab dialogs work.
  If you think a page needs its own buttons, consider making it a
  separate dialog.

  <li> There should be no implicit ordering of the pages.  If there
  is, it is probably better to use a wizard dialog.

  If some of the pages seem to be ordered and others not, perhaps they
  ought not to be joined in a tab dialog.

  </ol>

  Most of the functionality in QTabDialog is provided by a QTabWidget.

  <img src=qtabdlg-m.png> <img src=qtabdlg-w.png>

  \sa QDialog
*/

/*!
  \fn void QTabDialog::selected( const QString &tabLabel );
  \obsolete

  This signal is emitted whenever a tab is selected (raised),
  including during the first show().

  \sa raise()
*/

/*! \fn void QTabDialog::currentChanged( QWidget* );

  This signal is emitted whenever the current page changes.

  \sa currentPage(), showPage(), tabLabel()
*/


// add comments about delete, ok and apply

struct QTabPrivate
{
    QTabPrivate();

    QTabWidget* tw;

    QPushButton * ok;
    QPushButton * cb;
    QPushButton * db;
    QPushButton * hb;
    QPushButton * ab;

    QBoxLayout * tll;
};

QTabPrivate::QTabPrivate()
	: tw(0),
	  ok(0), cb(0), db(0), hb(0), ab(0),
	  tll(0)
{ }

/*!
  Constructs a QTabDialog with only an Ok button.
*/

QTabDialog::QTabDialog( QWidget *parent, const char *name, bool modal,
			WFlags f )
    : QDialog( parent, name, modal, f )
{
    d = new QTabPrivate;
    CHECK_PTR( d );

    d->tw = new QTabWidget( this, "tab widget" );
    connect ( d->tw, SIGNAL ( selected( const QString& ) ), this, SIGNAL( selected( const QString& ) ) );
    connect ( d->tw, SIGNAL ( currentChanged( QWidget* ) ), this, SIGNAL( currentChanged( QWidget* ) ) );

    d->ok = new QPushButton( this, "ok" );
    CHECK_PTR( d->ok );
    d->ok->setText( tr("OK") );
    d->ok->setDefault( TRUE );
    connect( d->ok, SIGNAL(clicked()),
	     this, SIGNAL(applyButtonPressed()) );
    connect( d->ok, SIGNAL(clicked()),
	     this, SLOT(accept()) );
}


/*!
  Destructs the tab dialog.
*/

QTabDialog::~QTabDialog()
{
    delete d;
}


/*!
  Sets the font for the tabs to \e font.

  If the widget is visible, the display is updated with the new font
  immediately.  There may be some geometry changes, depending on the
  size of the old and new fonts.
*/

void QTabDialog::setFont( const QFont & font )
{
    QDialog::setFont( font );
    setSizes();
}


/*!
  \fn void QTabDialog::applyButtonPressed();

  This signal is emitted when the Apply or OK buttons are clicked.

  It should be connected to a slot (or several slots) which change the
  application's state according to the state of the dialog.

  \sa cancelButtonPressed() defaultButtonPressed() setApplyButton()
*/


/*!
  Returns TRUE if the tab dialog has a Defaults button, FALSE if not.

  \sa setDefaultButton() defaultButtonPressed() hasApplyButton()
  hasCancelButton()
*/

bool QTabDialog::hasDefaultButton() const
{
     return d->db != 0;
}


/*!
  Returns TRUE if the tab dialog has a Help button, FALSE if not.

  \sa setHelpButton() helpButtonPressed() hasApplyButton()
  hasCancelButton()
*/

bool QTabDialog::hasHelpButton() const
{
     return d->hb != 0;
}


/*!
  \fn void QTabDialog::cancelButtonPressed();

  This signal is emitted when the Cancel button is clicked.  It is
  automatically connected to QDialog::reject(), which will hide the
  dialog.

  The Cancel button should not change the application's state in any
  way, so generally you should not need to connect it to any slot.

  \sa applyButtonPressed() defaultButtonPressed() setCancelButton()
*/


/*!
  Returns TRUE if the tab dialog has a Cancel button, FALSE if not.

  \sa setCancelButton() cancelButtonPressed() hasApplyButton()
  hasDefaultButton()
*/

bool QTabDialog::hasCancelButton() const
{
     return d->cb != 0;
}


/*!
  \fn void QTabDialog::defaultButtonPressed();

  This signal is emitted when the Defaults button is pressed.  It
  should reset the dialog (but not the application) to the "factory
  defaults."

  The application's state should not be changed until the user clicks
  Apply or OK.

  \sa applyButtonPressed() cancelButtonPressed() setDefaultButton()
*/


/*!
  \fn void QTabDialog::helpButtonPressed();

  This signal is emitted when the Help button is pressed.  It
  should give instructions about how to use the dialog.

  \sa applyButtonPressed() cancelButtonPressed() setHelpButton()
*/


/*!
  Returns TRUE if the tab dialog has an Apply button, FALSE if not.

  \sa setApplyButton() applyButtonPressed() hasCancelButton()
  hasDefaultButton()
*/

bool QTabDialog::hasApplyButton() const
{
    return d->ab != 0;
}


/*!
  Returns TRUE if the tab dialog has an OK button, FALSE if not.

  \sa setOkButton() hasApplyButton() hasCancelButton()
  hasDefaultButton()
*/

bool QTabDialog::hasOkButton() const
{
    return d->ok != 0;
}


/*!
  \fn void QTabDialog::aboutToShow()

  This signal is emitted by show() when it's time to set the state of
  the dialog's contents.  The dialog should reflect the current state
  of the application when if appears; if there is any chance that the
  state of the application can change between the time you call
  QTabDialog::QTabDialog() and QTabDialog::show(), you should set the
  dialog's state in a slot and connect this signal to it.

  This applies mainly to QTabDialog objects that are kept around
  hidden rather than being created, show()n and deleted afterwards.

  \sa applyButtonPressed(), show(), cancelButtonPressed()
*/


/*!\reimp
*/
void QTabDialog::show()
{
    //   Reimplemented in order to delay show()'ing of every page
    //   except the initially visible one, and in order to emit the
    //   aboutToShow() signal.
    if ( topLevelWidget() == this )
	d->tw->setFocus();
    emit aboutToShow();
    setSizes();
    setUpLayout();
    QDialog::show();
}


/*!
  Ensures that the selected tab's page is visible and appropriately sized.
*/

void QTabDialog::showTab( int i )
{
    d->tw->showTab( i );
}


/*!
  Adds another tab and page to the tab view.

  The tab will be labelled \a label and \a child constitutes the new
  page.  Note the difference between the widget name (which you supply
  to widget constructors and to e.g. setTabEnabled()) and the tab
  label: The name is internal to the program and invariant, while the
  label is shown on screen and may vary according to e.g. language.

  \a label is written in the QButton style, where &P makes Qt create
  an accelerator key on Alt-P for this page.  For example:

  \code
    td->addTab( graphicsPane, "&Graphics" );
    td->addTab( soundPane, "&Sound" );
  \endcode

  If the user presses Alt-S the sound page of the tab dialog is shown,
  if the user presses Alt-P the graphics page is shown.

  If you call addTab() after show(), the screen will flicker and the
  user will be confused.
*/

void QTabDialog::addTab( QWidget * child, const QString &label )
{
    d->tw->addTab( child, label );
}



/*!
  Adds another tab and page to the tab view.

  This function is the same as addTab() but with an additional
  iconset.
 */
void QTabDialog::addTab( QWidget *child, const QIconSet& iconset, const QString &label)
{
    d->tw->addTab( child, iconset, label );
}

/*!
  This is a lower-level method for adding tabs, similar to the other
  addTab() method.  It is useful if you are using setTabBar() to set a
  QTabBar subclass with an overridden QTabBar::paint() routine for a
  subclass of QTab.
*/
void QTabDialog::addTab( QWidget * child, QTab* tab )
{
    d->tw->addTab( child, tab );
}

/*!
  Inserts another tab and page to the tab view.

  The tab will be labelled \a label and \a child constitutes the new
  page.  Note the difference between the widget name (which you supply
  to widget constructors and to e.g. setTabEnabled()) and the tab
  label: The name is internal to the program and invariant, while the
  label is shown on screen and may vary according to e.g. language.

  \a label is written in the QButton style, where &P makes Qt create
  an accelerator key on Alt-P for this page.  For example:

  \code
    td->insertTab( graphicsPane, "&Graphics" );
    td->insertTab( soundPane, "&Sound" );
  \endcode

  If \a index is not specified, the tab is simply added. Otherwise
  it's inserted at the specified position.

  If the user presses Alt-S the sound page of the tab dialog is shown,
  if the user presses Alt-P the graphics page is shown.

  If you call insertTab() after show(), the screen will flicker and the
  user will be confused.
*/

void QTabDialog::insertTab( QWidget * child, const QString &label, int index )
{
    d->tw->insertTab( child, label, index );
}


/*!
  Inserts another tab and page to the tab view.

  This function is the same as insertTab() but with an additional
  iconset.
 */
void QTabDialog::insertTab( QWidget *child, const QIconSet& iconset, const QString &label, int index)
{
    d->tw->insertTab( child, iconset, label, index );
}

/*!
  This is a lower-level method for inserting tabs, similar to the other
  insertTab() method.  It is useful if you are using setTabBar() to set a
  QTabBar subclass with an overridden QTabBar::paint() routine for a
  subclass of QTab.
*/
void QTabDialog::insertTab( QWidget * child, QTab* tab, int index )
{
    d->tw->insertTab( child, tab, index );
}

/*!
  Replaces the QTabBar heading the dialog by the given tab bar.
  Note that this must be called \e before any tabs have been added,
  or the behavior is undefined.
  \sa tabBar()
*/
void QTabDialog::setTabBar( QTabBar* tb )
{
    d->tw->setTabBar( tb );
    setUpLayout();
}

/*!
  Returns the currently set QTabBar.
  \sa setTabBar()
*/
QTabBar* QTabDialog::tabBar() const
{
    return d->tw->tabBar();
}

/*!  Ensures that \a w is shown.  This is useful mainly for accelerators.

  \warning Used carelessly, this function can easily surprise or
  confuse the user.

  \sa QTabBar::setCurrentTab()
*/

void QTabDialog::showPage( QWidget * w )
{
    d->tw->showPage( w );
}


/*! \obsolete
  Returns TRUE if the page with object name \a name is enabled, and
  false if it is disabled.

  If \a name is 0 or not the name of any of the pages, isTabEnabled()
  returns FALSE.

  \sa setTabEnabled(), QWidget::isEnabled()
*/

bool QTabDialog::isTabEnabled( const char* name ) const
{
    if ( !name )
	return FALSE;
    QObjectList * l
	= ((QTabDialog *)this)->queryList( "QWidget", name, FALSE, TRUE );
    if ( l && l->first() ) {
	QWidget * w;
	while( l->current() ) {
	    while( l->current() && !l->current()->isWidgetType() )
		l->next();
	    w = (QWidget *)(l->current());
	    if ( w ) {
		return d->tw->isTabEnabled( w );
	    }
	}
    }
    return FALSE;
}


/*!\obsolete

  Finds the page with object name \a name, enables/disables it
  according to the value of \a enable, and redraws the page's tab
  appropriately.

  QTabDialog uses QWidget::setEnabled() internally, rather than keep a
  separate flag.

  Note that even a disabled tab/page may be visible.  If the page is
  visible already, QTabDialog will not hide it, and if all the pages
  are disabled, QTabDialog will show one of them.

  The object name is used (rather than the tab label) because the tab
  text may not be invariant in multi-language applications.

  \sa isTabEnabled(), QWidget::setEnabled()
*/

void QTabDialog::setTabEnabled( const char* name, bool enable )
{
    if ( !name )
	return;
    QObjectList * l
	= ((QTabDialog *)this)->queryList( "QWidget", name, FALSE, TRUE );
    if ( l && l->first() ) {
	QObjectListIt it(*l);
	QObject *o;
	while( (o = it.current()) ) {
	    ++it;
	    if( o->isWidgetType() )
		d->tw->setTabEnabled( (QWidget*)o, enable );
	}
    }
}


/* ### SHOULD THIS BE HERE?
  Adds an Apply button to the dialog.  The button's text is set to \e
  text (and defaults to "Apply").

  The Apply button should apply the current settings in the dialog box
  to the application, while keeping the dialog visible.

  When Apply is clicked, the applyButtonPressed() signal is emitted.

  If \a text is a
  \link QString::operator!() null string\endlink,
  no button is shown.

  \sa setCancelButton() setDefaultButton() applyButtonPressed()
*/


/*!
  Returns TRUE if the page \a w is enabled, and
  false if it is disabled.

  \sa setTabEnabled(), QWidget::isEnabled()
*/

bool QTabDialog::isTabEnabled( QWidget* w ) const
{
    return d->tw->isTabEnabled( w );
}

/*!
  Enables/disables page \a w according to the value of \a enable, and
  redraws the page's tab appropriately.

  QTabWidget uses QWidget::setEnabled() internally, rather than keep a
  separate flag.

  Note that even a disabled tab/page may be visible.  If the page is
  visible already, QTabWidget will not hide it, and if all the pages
  are disabled, QTabWidget will show one of them.

  \sa isTabEnabled(), QWidget::setEnabled()
*/

void QTabDialog::setTabEnabled( QWidget* w, bool enable)
{
    d->tw->setTabEnabled( w, enable );
}


/*!
  Add an Apply button to the dialog.  The button's text is set to \e
  text.

  The Apply button should apply the current settings in the dialog box
  to the application, while keeping the dialog visible.

  When Apply is clicked, the applyButtonPressed() signal is emitted.

  \sa setCancelButton() setDefaultButton() applyButtonPressed()
*/
void QTabDialog::setApplyButton( const QString &text )
{
    if ( !text && d->ab ) {
        delete d->ab;
        d->ab = 0;
        setSizes();
    } else {
        if ( !d->ab ) {
            d->ab = new QPushButton( this, "apply settings" );
            connect( d->ab, SIGNAL(clicked()),
                     this, SIGNAL(applyButtonPressed()) );
            setUpLayout();
        }
        d->ab->setText( text );
        setSizes();
        //d->ab->show();
    }
}

/*!
  Adds an Apply button to the dialog.  The button's text is set to
  a localizable "Apply".
 */
void QTabDialog::setApplyButton()
{
    setApplyButton( tr("Apply") );
}


/*!
  Adds a Help button to the dialog.  The button's text is set to \e
  text.

  When Help is clicked, the helpButtonPressed() signal is emitted.

  If \a text is a
  \link QString::operator!() null string\endlink,
  no button is shown.

  \sa setApplyButton() setCancelButton() helpButtonPressed()
*/

void QTabDialog::setHelpButton( const QString &text )
{
    if ( !text ) {
        delete d->hb;
        d->hb = 0;
        setSizes();
    } else {
        if ( !d->hb ) {
            d->hb = new QPushButton( this, "give help" );
            connect( d->hb, SIGNAL(clicked()),
                     this, SIGNAL(helpButtonPressed()) );
            setUpLayout();
        }
        d->hb->setText( text );
        setSizes();
        //d->hb->show();
    }
}


/*!
  Adds a Help button to the dialog.  The button's text is set to
  a localizable "Help".
 */
void QTabDialog::setHelpButton()
{
    setHelpButton( tr("Help") );
}

/*!
  Adds a Defaults button to the dialog.  The button's text is set to \e
  text.

  The Defaults button should set the dialog (but not the application)
  back to the application defaults.

  When Defaults is clicked, the defaultButtonPressed() signal is emitted.

  If \a text is a
  \link QString::operator!() null string\endlink,
  no button is shown.

  \sa setApplyButton() setCancelButton() defaultButtonPressed()
*/

void QTabDialog::setDefaultButton( const QString &text )
{
    if ( !text ) {
        delete d->db;
        d->db = 0;
        setSizes();
    } else {
        if ( !d->db ) {
            d->db = new QPushButton( this, "back to default" );
            connect( d->db, SIGNAL(clicked()),
                     this, SIGNAL(defaultButtonPressed()) );
            setUpLayout();
        }
        d->db->setText( text );
        setSizes();
        //d->db->show();
    }
}


/*!
  Adds a Defaults button to the dialog.  The button's text is set to
  a localizable "Defaults".
 */
void QTabDialog::setDefaultButton()
{
    setDefaultButton( tr("Defaults") );
}

/*!
  Adds a Cancel button to the dialog.  The button's text is set to \e
  text.

  The cancel button should always return the application to the state
  it was in before the tab view popped up, or if the user has clicked
  Apply, back the the state immediately after the last Apply.

  When Cancel is clicked, the cancelButtonPressed() signal is emitted.
  The dialog is closed at the same time.

  If \a text is a
  \link QString::operator!() null string\endlink,
  no button is shown.

  \sa setApplyButton() setDefaultButton() cancelButtonPressed()
*/

void QTabDialog::setCancelButton( const QString &text )
{
    if ( !text ) {
        delete d->cb;
        d->cb = 0;
        setSizes();
    } else {
        if ( !d->cb ) {
            d->cb = new QPushButton( this, "cancel dialog" );
            connect( d->cb, SIGNAL(clicked()),
                     this, SIGNAL(cancelButtonPressed()) );
            connect( d->cb, SIGNAL(clicked()),
                     this, SLOT(reject()) );
            setUpLayout();
        }
        d->cb->setText( text );
        setSizes();
        //d->cb->show();
    }
}


/*!
  Adds a Cancel button to the dialog.  The button's text is set to
  a localizable "Cancel".
 */

void QTabDialog::setCancelButton()
{
    setCancelButton( tr("Cancel") );
}


/*!  Sets up the layout manager for the tab dialog.

  \sa setSizes() setApplyButton() setCancelButton() setDefaultButton()
*/

void QTabDialog::setUpLayout()
{
    // the next four are probably the same, really?
    const int topMargin = 6;
    const int leftMargin = 6;
    const int rightMargin = 6;
    const int bottomMargin = 6;
    const int betweenButtonsMargin = 7;
    const int aboveButtonsMargin = 8;

    delete d->tll;
    d->tll = new QBoxLayout( this, QBoxLayout::Down );

    // top margin
    d->tll->addSpacing( topMargin );

    QBoxLayout * tmp = new QBoxLayout( QBoxLayout::LeftToRight );
     d->tll->addLayout( tmp, 1 );
     tmp->addSpacing( leftMargin );
     tmp->addWidget( d->tw, 1);
     tmp->addSpacing( rightMargin + 2 );

    d->tll->addSpacing( aboveButtonsMargin + 2 );
    QBoxLayout * buttonRow = new QBoxLayout( QBoxLayout::RightToLeft );
    d->tll->addLayout( buttonRow, 0 );
    d->tll->addSpacing( bottomMargin );

    buttonRow->addSpacing( rightMargin );
    if ( d->cb ) {
	buttonRow->addWidget( d->cb, 0 );
	buttonRow->addSpacing( betweenButtonsMargin );
    }

    if ( d->ab ) {
	buttonRow->addWidget( d->ab, 0 );
	buttonRow->addSpacing( betweenButtonsMargin );
    }

    if ( d->db ) {
	buttonRow->addWidget( d->db, 0 );
	buttonRow->addSpacing( betweenButtonsMargin );
    }

    if ( d->hb ) {
	buttonRow->addWidget( d->hb, 0 );
	buttonRow->addSpacing( betweenButtonsMargin );
    }

    if ( d->ok ) {
	buttonRow->addWidget( d->ok, 0 );
	buttonRow->addSpacing( betweenButtonsMargin );
    }

    // add one custom widget here
    buttonRow->addStretch( 1 );
    // add another custom widget here

    d->tll->activate();
}


/*!  Sets up the minimum and maximum sizes for each child widget.

  \sa setUpLayout() setFont()
*/

void QTabDialog::setSizes()
{
    // compute largest button size
    QSize s( 0, 0 );
    int bw = s.width();
    int bh = s.height();

    if ( d->ok ) {
	s = d->ok->sizeHint();
	if ( s.width() > bw )
	    bw = s.width();
	if ( s.height() > bh )
	    bh = s.height();
    }

    if ( d->ab ) {
	s = d->ab->sizeHint();
	if ( s.width() > bw )
	    bw = s.width();
	if ( s.height() > bh )
	    bh = s.height();
    }

    if ( d->db ) {
	s = d->db->sizeHint();
	if ( s.width() > bw )
	    bw = s.width();
	if ( s.height() > bh )
	    bh = s.height();
    }

    if ( d->hb ) {
	s = d->hb->sizeHint();
	if ( s.width() > bw )
	    bw = s.width();
	if ( s.height() > bh )
	    bh = s.height();
    }

    if ( d->cb ) {
	s = d->cb->sizeHint();
	if ( s.width() > bw )
	    bw = s.width();
	if ( s.height() > bh )
	    bh = s.height();
    }

    if ( style() == WindowsStyle && bw < 75 )
	bw = 75;

    // and set all the buttons to that size
    if ( d->ok )
	d->ok->setFixedSize( bw, bh );
    if ( d->ab )
	d->ab->setFixedSize( bw, bh );
    if ( d->db )
	d->db->setFixedSize( bw, bh );
    if ( d->hb )
	d->hb->setFixedSize( bw, bh );
    if ( d->cb )
	d->cb->setFixedSize( bw, bh );

    // fiddle the tab chain so the buttons are in their natural order
    QWidget * w = d->ok;

    if ( d->hb ) {
	if ( w )
	    setTabOrder( w, d->hb );
	w = d->hb;
    }
    if ( d->db ) {
	if ( w )
	    setTabOrder( w, d->db );
	w = d->db;
    }
    if ( d->ab ) {
	if ( w )
	    setTabOrder( w, d->ab );
	w = d->ab;
    }
    if ( d->cb ) {
	if ( w )
	    setTabOrder( w, d->cb );
	w = d->cb;
    }
    setTabOrder( w, d->tw );
}

/*!\reimp
*/
void QTabDialog::resizeEvent( QResizeEvent * e )
{
    QDialog::resizeEvent( e );
}


/*!\reimp
*/
void QTabDialog::paintEvent( QPaintEvent * )
{
}


/*!
  Set the OK button's text to \a text.

  When the OK button is clicked, the applyButtonPressed() signal is emitted,
  and the current settings in the dialog box should be applied to
  the application. Then the dialog closes.

  If \a text is a
  \link QString::operator!() null string\endlink,
  no button is shown.

  \sa setCancelButton() setDefaultButton() applyButtonPressed()
*/

void QTabDialog::setOkButton( const QString &text )
{
    if ( !text ) {
        delete d->ok;
        d->ok = 0;
        setSizes();
    } else {
        if ( !d->ok ) {
            d->ok = new QPushButton( this, "ok" );
            connect( d->ok, SIGNAL(clicked()),
                     this, SIGNAL(applyButtonPressed()) );
            setUpLayout();
        }
        d->ok->setText( text );
        setSizes();
        //d->ok->show();
    }
}
/*!
  Adds an OK to the dialog.  The button's text is set to
  a localizable "OK".
 */

void QTabDialog::setOkButton()
{
    setOkButton( tr("OK") );
}


/*!
  Old version of setOkButton(), provided for backward compatibility.
 */
void QTabDialog::setOKButton( const QString &text )
{
    // Ugle workaround for original "OK" default argument
    QString newText( text );
    if ( text.isNull() )
	newText = QString::fromLatin1( "OK" );
    setOkButton( newText );
}


/*!  Returns the text in the tab for page \a w.
*/

QString QTabDialog::tabLabel( QWidget * w )
{
    return d->tw->tabLabel( w );
}	


/*!  \reimp
*/
void QTabDialog::styleChange( QStyle& s )
{
    QDialog::styleChange( s );
    setSizes();
}


/*!  Returns a pointer to the page currently being displayed by the
tab dialog.  The tab dialog does its best to make sure that this value
is never 0, but if you try hard enough it can be.
*/

QWidget * QTabDialog::currentPage() const
{
    return d->tw->currentPage();
}

/*!
  Defines a new \a label for the tab of page \a w
 */
void QTabDialog::changeTab( QWidget *w, const QString &label)
{
    d->tw->changeTab( w, label );
}

/*!
  Defines a new \a iconset and a new \a label for the tab of page \a w
 */
void QTabDialog::changeTab( QWidget *w, const QIconSet& iconset, const QString &label)
{
    d->tw->changeTab( w, iconset, label );
}

/*! Removes page \a w from this stack of widgets.  Does not
  delete \a w.
  \sa showPage(), QTabWidget::removePage(), QWidgetStack::removeWidget()
*/
void QTabDialog::removePage( QWidget * w )
{
    d->tw->removePage( w );
}

#endif
