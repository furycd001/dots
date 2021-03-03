/****************************************************************************
** $Id: qt/src/widgets/qtabwidget.cpp   2.3.2   edited 2001-03-21 $
**
** Implementation of QTabWidget class
**
** Created : 990318
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of the widgets module of the Qt GUI Toolkit.
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

#include "qtabwidget.h"
#ifndef QT_NO_TABWIDGET
#include "qobjectlist.h"
#include "qobjectdict.h"
#include "qtabbar.h"
#include "qapplication.h"
#include "qwidgetstack.h"
#include "qbitmap.h"

// NOT REVISED
/*!
  \class QTabWidget qtabwidget.h

  \brief The QTabWidget class provides a stack of tabbed widgets.

  \ingroup organizers

  A tabbed widget is one in which several "pages" are available, and
  the user selects which page to see and use by clicking on its tab,
  or by pressing the indicated Alt-(letter) key combination.

  QTabWidget does not provide more than one row of tabs, and does not
  provide tabs along the sides or bottom of the pages.

  The normal way to use QTabWidget is to do the following in the
  constructor: <ol> <li> Create a QTabWidget. <li> Create a QWidget
  for each of the pages in the tab dialog, insert children into it,
  set up geometry management for it, and use addTab() to set up a tab
  and keyboard accelerator for it. <li> Connect to the
  signals and slots. </ol>

  If you don't call addTab(), the page you have created will not be
  visible.  Please don't confuse the object name you supply to the
  QWidget constructor and the tab label you supply to addTab():
  addTab() takes a name which indicates an accelerator and is
  meaningful and descriptive to the user, while the widget name is
  used primarily for debugging.

  A signal currentChanged() is emitted when the user selects some
  page.

  Each tab is either enabled or disabled at any given time.  If a tab
  is enabled, the tab text is drawn in black and the user can select
  that tab.  If it is disabled, the tab is drawn in a different way
  and the user can not select that tab.  Note that even though a tab
  is disabled, the page can still be visible, for example if all of
  the tabs happen to be disabled.

  While tab widgets can be a very good way to split up a complex
  dialog, it's also very easy to make a royal mess out of it. See
  QTabDialog for some design hints.

  Most of the functionality in QTabWidget is provided by a QTabBar (at
  the top, providing the tabs) and a QWidgetStack (most of the area,
  organizing the individual pages).

  <img src=qtabwidget-m.png> <img src=qtabwidget-w.png>

  \sa QTabDialog
*/


/*! \enum QTabWidget::TabPosition

  This enum type defines where QTabWidget can draw the tab row: <ul>
  <li> \c Top - above the pages
  <li> \c Bottom - below the pages
  </ul>
*/

/*! \enum QTabWidget::TabShape

  This enum type defines the shape of the tabs: <ul>
  <li> \c Rounded - rounded look (normal)
  <li> \c Triangular - triangular look (very unusual, included for completeness)
  </ul>
*/

/* undocumented now
  \obsolete

  \fn void QTabWidget::selected( const QString &tabLabel );

  This signal is emitted whenever a tab is selected (raised),
  including during the first show().

  \sa raise()
*/


/*! \fn void QTabWidget::currentChanged( QWidget* );

  This signal is emitted whenever the current page changes.

  \sa currentPage(), showPage(), tabLabel()
*/

class QTabWidgetData
{
public:
    QTabWidgetData()
	: tabs(0), stack(0), dirty( TRUE ), pos( QTabWidget::Top ), shape( QTabWidget::Rounded )
	{};
    ~QTabWidgetData(){};
    QTabBar* tabs;
    QWidgetStack* stack;
    bool dirty;
    QTabWidget::TabPosition pos;
    QTabWidget::TabShape shape;
};





/*!
  Constructs a tabbed widget with parent \a parent, name \a name
  and widget flags \a f.
*/


QTabWidget::QTabWidget( QWidget *parent, const char *name, WFlags f )
    : QWidget( parent, name, f )
{
    init();
}

/*!
  \overload
*/
QTabWidget::QTabWidget( QWidget *parent, const char *name )
    : QWidget( parent, name )
{
    init();
}

void QTabWidget::init()
{
    d = new QTabWidgetData;

    d->stack = new QWidgetStack( this, "tab pages" );
    d->stack->installEventFilter( this );
    setTabBar( new QTabBar( this, "tab control" ) );

    d->stack->setFrameStyle( QFrame::StyledPanel | QFrame::Raised );
    d->stack->setLineWidth( style().defaultFrameWidth() );

    setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding ) );
    setFocusPolicy( TabFocus );
    setFocusProxy( d->tabs );
}

/*!
  Destructs the tab widget.
*/
QTabWidget::~QTabWidget()
{
    delete d;
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
void QTabWidget::addTab( QWidget *child, const QString &label)
{
    QTab * t = new QTab();
    CHECK_PTR( t );
    t->label = label;
    addTab( child, t );
}


/*!
  Adds another tab and page to the tab view.

  This function is the same as addTab() but with an additional
  iconset.
 */
void QTabWidget::addTab( QWidget *child, const QIconSet& iconset, const QString &label)
{
    QTab * t = new QTab();
    CHECK_PTR( t );
    t->label = label;
    t->iconset = new QIconSet( iconset );
    addTab( child, t );
}

/*!
  This is a lower-level method for adding tabs, similar to the other
  addTab() method.  It is useful if you are using setTabBar() to set a
  QTabBar subclass with an overridden QTabBar::paint() routine for a
  subclass of QTab.
*/
void QTabWidget::addTab( QWidget *child, QTab* tab)
{
    tab->enabled = TRUE;
    int id = d->tabs->addTab( tab );
    d->stack->addWidget( child, id );
    if ( d->stack->frameStyle() != ( QFrame::StyledPanel | QFrame::Raised ) )
	d->stack->setFrameStyle( QFrame::StyledPanel | QFrame::Raised );
    setUpLayout();
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
void QTabWidget::insertTab( QWidget *child, const QString &label, int index)
{
    QTab * t = new QTab();
    CHECK_PTR( t );
    t->label = label;
    insertTab( child, t, index );
}


/*!
  Inserts another tab and page to the tab view.

  This function is the same as insertTab() but with an additional
  iconset.
 */
void QTabWidget::insertTab( QWidget *child, const QIconSet& iconset, const QString &label, int index )
{
    QTab * t = new QTab();
    CHECK_PTR( t );
    t->label = label;
    t->iconset = new QIconSet( iconset );
    insertTab( child, t, index );
}

/*!
  This is a lower-level method for inserting tabs, similar to the other
  insertTab() method.  It is useful if you are using setTabBar() to set a
  QTabBar subclass with an overridden QTabBar::paint() routine for a
  subclass of QTab.
*/
void QTabWidget::insertTab( QWidget *child, QTab* tab, int index)
{
    tab->enabled = TRUE;
    int id = d->tabs->insertTab( tab, index );
    d->stack->addWidget( child, id );
    if ( d->stack->frameStyle() != ( QFrame::StyledPanel | QFrame::Raised ) )
	d->stack->setFrameStyle( QFrame::StyledPanel | QFrame::Raised );
    setUpLayout();
}


/*!
  Defines a new label for the tab of page \a w
 */
void QTabWidget::changeTab( QWidget *w, const QString &label)
{

    //#### accelerators
    int id = d->stack->id( w );
    if ( id < 0 )
	return;
    QTab* t = d->tabs->tab( id );
    if ( !t )
	return;
    t->label = label;
    d->tabs->layoutTabs();

    int ct = d->tabs->currentTab();
    bool block = d->tabs->signalsBlocked();
    d->tabs->blockSignals( TRUE );
    d->tabs->setCurrentTab( 0 );
    d->tabs->setCurrentTab( ct );
    d->tabs->blockSignals( block );

    d->tabs->update();
    setUpLayout();
}

/*!
  Defines a new \a iconset and a new \a label for the tab of page \a w
 */
void QTabWidget::changeTab( QWidget *w, const QIconSet& iconset, const QString &label)
{
    //#### accelerators
    int id = d->stack->id( w );
    if ( id < 0 )
	return;
    QTab* t = d->tabs->tab( id );
    if ( !t )
	return;
    if ( t->iconset )
	delete t->iconset;
    t->label = label;
    t->iconset = new QIconSet( iconset );
    d->tabs->layoutTabs();

    int ct = d->tabs->currentTab();
    bool block = d->tabs->signalsBlocked();
    d->tabs->blockSignals( TRUE );
    d->tabs->setCurrentTab( 0 );
    d->tabs->setCurrentTab( ct );
    d->tabs->blockSignals( block );

    d->tabs->update();
    setUpLayout();
}

/*!
  Returns TRUE if the page \a w is enabled, and
  false if it is disabled.

  \sa setTabEnabled(), QWidget::isEnabled()
*/

bool QTabWidget::isTabEnabled( QWidget* w ) const
{
    int id = d->stack->id( w );
    if ( id >= 0 )
	return w->isEnabled();
    else
	return FALSE;
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

void QTabWidget::setTabEnabled( QWidget* w, bool enable)
{
    int id = d->stack->id( w );
    if ( id >= 0 ) {
	w->setEnabled( enable );
	d->tabs->setTabEnabled( id, enable );
    }
}

/*!  Ensures that \a w is shown.  This is useful mainly for accelerators.

  \warning Used carelessly, this function can easily surprise or
  confuse the user.

  \sa QTabBar::setCurrentTab()
*/
void QTabWidget::showPage( QWidget * w)
{
    int id = d->stack->id( w );
    if ( id >= 0 ) {
	d->stack->raiseWidget( w );
	d->tabs->setCurrentTab( id );
	// ### why overwrite the frame style?
	if ( d->stack->frameStyle() != ( QFrame::StyledPanel|QFrame::Raised ) )
	    d->stack->setFrameStyle( QFrame::StyledPanel | QFrame::Raised );
    }
}

/*! Removes page \a w from this stack of widgets.  Does not
  delete \a w.
  \sa showPage(), QWidgetStack::removeWidget()
*/
void QTabWidget::removePage( QWidget * w )
{
    int id = d->stack->id( w );
    if ( id >= 0 ) {
	d->tabs->setTabEnabled( id, FALSE );
	d->stack->removeWidget( w );
	d->tabs->removeTab( d->tabs->tab(id) );
	setUpLayout();
	
	if ( d->tabs->count() == 0 ) 
	    d->stack->setFrameStyle( QFrame::NoFrame );
    }
}

/*!  Returns the text in the tab for page \a w.
*/

QString QTabWidget::tabLabel( QWidget * w)
{
    QTab * t = d->tabs->tab( d->stack->id( w ) );
    return t ? t->label : QString::null;
}

/*!  Returns a pointer to the page currently being displayed by the
tab dialog.  The tab dialog does its best to make sure that this value
is never 0, but if you try hard enough it can be.
*/

QWidget * QTabWidget::currentPage() const
{
    return d->stack->visibleWidget();
}

/*! Returns the ID of the current page.
*/

int QTabWidget::currentPageIndex() const
{
    return d->tabs->currentTab();
}

/*! Sets the page with index \a id as current page.

    Note that \e id is not the index that is specified when you insert a tab
*/

void QTabWidget::setCurrentPage( int id )
{
    d->tabs->setCurrentTab( id );
    showTab( id );
}

/*!
  \reimp
 */
void QTabWidget::resizeEvent( QResizeEvent * )
{
    setUpLayout();
}

/*!
  Replaces the QTabBar heading the dialog by the given tab bar.
  Note that this must be called \e before any tabs have been added,
  or the behavior is undefined.
  \sa tabBar()
*/
void QTabWidget::setTabBar( QTabBar* tb)
{
    if ( tb->parentWidget() != this )
	tb->reparent( this, QPoint(0,0), TRUE );
    delete d->tabs;
    d->tabs = tb;
    connect( d->tabs, SIGNAL(selected(int)),
	     this,    SLOT(showTab(int)) );
    setUpLayout();
}


/*!
  Returns the currently set QTabBar.
  \sa setTabBar()
*/
QTabBar* QTabWidget::tabBar() const
{
    return d->tabs;
}

/*!
  Ensures that the selected tab's page is visible and appropriately sized.
*/

void QTabWidget::showTab( int i )
{
    if ( d->stack->widget( i ) ) {
	d->stack->raiseWidget( i );
	emit selected( d->tabs->tab( i )->label );
	emit currentChanged( d->stack->widget( i ) );
    }
}

/*!
  Set up the layout.
 */
void QTabWidget::setUpLayout( bool onlyCheck )
{
    if ( onlyCheck && !d->dirty )
	return; // nothing to do

    if ( !isVisible() ) {
	d->dirty = TRUE;
	return; // we'll do it later
    }
    QSize t( d->tabs->sizeHint() );
    if ( t.width() > width() )
	t.setWidth( width() );
    int lw = d->stack->lineWidth();
    if ( d->pos == Bottom ) {
	d->tabs->setGeometry( QMAX(0, lw-2), height() - t.height() - lw, t.width(), t.height() );
	d->stack->setGeometry( 0, 0, width(), height()-t.height()+QMAX(0, lw-2) );
    }
    else { // Top
	d->tabs->setGeometry( QMAX(0, lw-2), 0, t.width(), t.height() );
	d->stack->setGeometry( 0, t.height()-lw, width(), height()-t.height()+QMAX(0, lw-2));
    }

    d->dirty = FALSE;
    if ( !onlyCheck )
	update();
    if ( autoMask() )
	updateMask();
}

/*!\reimp
*/
QSize QTabWidget::sizeHint() const
{
    QSize s( d->stack->sizeHint() );
    QSize t( d->tabs->sizeHint() );
    return QSize( QMAX( s.width(), t.width()),
		  s.height() + t.height() );
}


/*!
  Returns a suitable minimum size for the tab widget.
*/
QSize QTabWidget::minimumSizeHint() const
{
    QSize s( d->stack->minimumSizeHint() );
    QSize t( d->tabs->minimumSizeHint() );
    return QSize( QMAX( s.width(), t.width()),
		  s.height() + t.height() );
}

/*! \reimp
 */
void QTabWidget::showEvent( QShowEvent * )
{
    setUpLayout( TRUE );
}


/*!
  Returns the position of the tabs.

  Possible values are QTabWidget::Top and QTabWidget::Bottom.
  \sa setTabPosition()
 */
QTabWidget::TabPosition QTabWidget::tabPosition() const
{
    return d->pos;
}

/*!
  Sets the position of the tabs to \e pos

  Possible values are QTabWidget::Top and QTabWidget::Bottom.
  \sa tabPosition()
 */
void QTabWidget::setTabPosition( TabPosition pos)
{
    if (d->pos == pos)
	return;
    d->pos = pos;
    if (d->tabs->shape() == QTabBar::TriangularAbove || d->tabs->shape() == QTabBar::TriangularBelow ) {
	if ( pos == Bottom )
	    d->tabs->setShape( QTabBar::TriangularBelow );
	else
	    d->tabs->setShape( QTabBar::TriangularAbove );
    }
    else {
	if ( pos == Bottom )
	    d->tabs->setShape( QTabBar::RoundedBelow );
	else
	    d->tabs->setShape( QTabBar::RoundedAbove );
    }
    d->tabs->layoutTabs();
    setUpLayout();
}

/*!
  Returns the shape of the tabs.
  
  \sa setTabShape()
*/

QTabWidget::TabShape QTabWidget::tabShape() const
{
    return d->shape;
}

/*!
  Sets the shape of the tabs to \a s.
*/

void QTabWidget::setTabShape( TabShape s )
{
    if ( d->shape == s )
	return;
    d->shape = s;
    if ( d->pos == Top ) {
	if ( s == Rounded )
	    d->tabs->setShape( QTabBar::RoundedAbove );
	else
	    d->tabs->setShape( QTabBar::TriangularAbove );
    } else {
	if ( s == Rounded )
	    d->tabs->setShape( QTabBar::RoundedBelow );
	else
	    d->tabs->setShape( QTabBar::TriangularBelow );
    }
    d->tabs->layoutTabs();
    setUpLayout();
}


/*!
  Returns the width of the margin. The margin is the distance between
  the innermost pixel of the frame and the outermost pixel of the
  pages.

  \sa setMargin()
*/
int QTabWidget::margin() const
{
    return d->stack->margin();
}

/*!
  Sets the width of the margin to \e w.
  \sa margin()
*/
 void QTabWidget::setMargin( int w )
{
    d->stack->setMargin( w );
    setUpLayout();
}


/*! \reimp
 */
void QTabWidget::styleChange( QStyle& old )
{
    d->stack->setLineWidth( style().defaultFrameWidth() );
    setUpLayout();
    QWidget::styleChange( old );
}


/*! \reimp
 */
void QTabWidget::updateMask()
{
    if ( !autoMask() )
	return;

    QRect r;
    QRegion reg( r );
    reg += QRegion( d->tabs->geometry() );
    reg += QRegion( d->stack->geometry() );
    setMask( reg );
}


/*!\reimp
 */
bool QTabWidget::eventFilter( QObject *o, QEvent * e)
{
    if ( o == d->stack && e->type() == QEvent::ChildRemoved
	 && ( (QChildEvent*)e )->child()->isWidgetType() ) {
	removePage( (QWidget*)  ( (QChildEvent*)e )->child() );
	return TRUE;
    }
    return FALSE;
}


/*!\reimp
 */

QSizePolicy QTabWidget::sizePolicy() const
{
    //### removeme 3.0
    return QWidget::sizePolicy();
}

#endif
