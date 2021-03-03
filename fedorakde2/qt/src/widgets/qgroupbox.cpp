/**********************************************************************
** $Id: qt/src/widgets/qgroupbox.cpp   2.3.2   edited 2001-02-09 $
**
** Implementation of QGroupBox widget class
**
** Created : 950203
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

#include "qgroupbox.h"
#ifndef QT_NO_GROUPBOX
#include "qlayout.h"
#include "qpainter.h"
#include "qbitmap.h"
#include "qaccel.h"
#include "qradiobutton.h"
#include "qfocusdata.h"
#include "qobjectlist.h"
#include "qdrawutil.h"
#include "qapplication.h"


// REVISED: arnt
/*!
  \class QGroupBox qgroupbox.h
  \brief The QGroupBox widget provides a group box frame with a title.

  \ingroup organizers

  A group box provides a frame, a title and a keyboard shortcut, and
  displays various other widgets inside itself.  The title is on top,
  the keyboard shortcut moves keyboard focus to one of the group box'
  child widgets, and the child widgets are arranged in an array inside
  the frame.

  The simplest way to use it is to create a group box with the desired
  number of columns and orientation, and then just create widgets with
  the group box as parent.

  However, it is also possible to change the orientation() and number
  of columns() after construction, or to ignore all the automatic
  layout support and manage all that yourself.

  QGroupBox also lets you set the title() (normally set in the
  constructor) and if you so please, even the title's alignment().

  <img src=qgrpbox-w.png>

  \sa QButtonGroup
*/


/*!
  Constructs a group box widget with no title.

  The \e parent and \e name arguments are passed to the QWidget constructor.

  This constructor does not do automatic layout.
*/

QGroupBox::QGroupBox( QWidget *parent, const char *name )
    : QFrame( parent, name )
{
    init();
}

/*!
  Constructs a group box with a title.

  The \e parent and \e name arguments are passed to the QWidget constructor.

  This constructor does not do automatic layout.
*/

QGroupBox::QGroupBox( const QString &title, QWidget *parent, const char *name )
    : QFrame( parent, name )
{
    init();
    setTitle( title );
}

/*!
  Constructs a group box with no title. Child widgets will be arranged
  in \a strips rows or columns (depending on \a orientation).

  The \e parent and \e name arguments are passed to the QWidget constructor.
*/

QGroupBox::QGroupBox( int strips, Orientation orientation,
		    QWidget *parent, const char *name )
    : QFrame( parent, name )
{
    init();
    setColumnLayout( strips, orientation );
}

/*!
  Constructs a group box titled \a title. Child widgets will be arranged
  in \a strips rows or columns (depending on \a orientation).

  The \e parent and \e name arguments are passed to the QWidget constructor.
*/

QGroupBox::QGroupBox( int strips, Orientation orientation,
		    const QString &title, QWidget *parent,
		    const char *name )
    : QFrame( parent, name )
{
    init();
    setTitle( title );
    setColumnLayout( strips, orientation );
}

void QGroupBox::init()
{
    int fs;
    align = AlignLeft;
    fs = QFrame::Box | QFrame::Sunken;
    setFrameStyle( fs );
#ifndef QT_NO_ACCEL
    accel = 0;
#endif
    vbox = 0;
    grid = 0;
    d = 0;	//we use d directly to store a QSpacerItem
    lenvisible = 0;
    nCols = nRows = 0;
    dir = Horizontal;
}

void QGroupBox::setTextSpacer()
{
    QSpacerItem *sp = (QSpacerItem*)d;
    if ( ! sp )
	return;
    int h = 0;
    int w = 0;
    if ( lenvisible ) {
	QFontMetrics fm = fontMetrics();
	h = fm.height();
	w = fm.width( str, lenvisible ) + 2*fm.width( "xx" );
	if ( layout() ) {
	    int m = layout()->margin();
	    // do we have a child layout?
	    for ( QLayoutIterator it = layout()->iterator(); it.current(); ++it ) {
		if ( it.current()->layout() ) {
		    m += it.current()->layout()->margin();
		    break;
		}
	    }
	    if ( m > 4 )
		h -= m - 4;
	    h = QMAX( 0, h );
	}
    }
    sp->changeSize( w, h, QSizePolicy::Minimum, QSizePolicy::Fixed );
}

/*!
  Sets the group box title text to \a title, and add a focus-change
  accelerator if the \a title contains & followed by an appropriate
  letter.  This produces "User information" with the U underscored and
  Alt-U moves the keyboard focus into the group:

  \code
    g->setTitle( "&User information" );
  \endcode
*/

void QGroupBox::setTitle( const QString &title )
{
    if ( str == title )				// no change
	return;
    str = title;
#ifndef QT_NO_ACCEL
    if ( accel )
	delete accel;
    accel = 0;
    int s = QAccel::shortcutKey( title );
    if ( s ) {
	accel = new QAccel( this, "automatic focus-change accelerator" );
	accel->connectItem( accel->insertItem( s, 0 ),
			    this, SLOT(fixFocus()) );
    }
#endif
    calculateFrame();
    setTextSpacer();
    if ( layout() ) {
	layout()->activate();
	QSize s( size() );
	QSize ms( minimumSizeHint() );
	resize( QMAX( s.width(), ms.width() ),
		QMAX( s.height(), ms.height() ) );
    }

    update();
    updateGeometry();
}

/*!
  \fn QString QGroupBox::title() const
  Returns the group box title text.
*/

/*!
  \fn int QGroupBox::alignment() const
  Returns the alignment of the group box title.

  The default alignment is \c AlignLeft.

  \sa setAlignment(), Qt::AlignmentFlags
*/

/*!
  Sets the alignment of the group box title.

  The title is always placed on the upper frame line, however,
  the horizontal alignment can be specified by the \e alignment parameter.

  The \e alignment is one of the following flags:
  <ul>
  <li> \c AlignLeft aligns the title text to the left.
  <li> \c AlignRight aligns the title text to the right.
  <li> \c AlignHCenter aligns the title text centered.
  </ul>

  \sa alignment(), Qt::AlignmentFlags
*/

void QGroupBox::setAlignment( int alignment )
{
    align = alignment;
    update();
}

/*! \reimp
*/
void QGroupBox::resizeEvent( QResizeEvent *e )
{
    QFrame::resizeEvent(e);
    calculateFrame();
}

/*! \reimp

  \internal
  overrides QFrame::paintEvent
*/

void QGroupBox::paintEvent( QPaintEvent *event )
{
    QPainter paint( this );

    if ( lenvisible ) {					// draw title
	QFontMetrics fm = paint.fontMetrics();
	int h = fm.height();
	int tw = fm.width( str, lenvisible ) + 2*fm.width(QChar(' '));
	int x;
	if ( align & AlignHCenter )		// center alignment
	    x = frameRect().width()/2 - tw/2;
	else if ( align & AlignRight )	// right alignment
	    x = frameRect().width() - tw - 8;
	else				// left alignment
	    x = 8;
	qDrawItem( &paint, style(), x, 0, tw, h, AlignCenter + ShowPrefix,
		   colorGroup(), isEnabled(), 0, str, lenvisible, 0 );
	QRect r( x, 0, tw, h );
	paint.setClipRegion( event->region().subtract( r ) );	// clip everything but title
    }
    drawFrame( &paint );			// draw the frame
    drawContents( &paint );			// draw the contents
}


/*! \reimp */
void QGroupBox::updateMask()
{
    QRegion reg( rect() );

    int len = str.length();
    if ( len ) {
	QFontMetrics fm = fontMetrics();
	int h = fm.height();
	int tw = 0;
	while ( len ) {
	    tw = fm.width( str, len ) + 2 * fm.width( QChar(' ') );
	    if ( tw < rect().width() )
		break;
	    len--;
	}
	int x;
	if ( align & AlignHCenter )
	    x = rect().width() / 2 - tw / 2;
	else if ( align & AlignRight )
	    x = rect().width() - tw - 8;
	else
	    x = 8;
	reg = reg.subtract( QRect( 0, 0, x, h / 2 ) );
	reg = reg.subtract( QRect( x + tw, 0, rect().width() - ( x + tw ), h / 2 ) );
    }

    setMask( reg );
}

/*!
  Adds an empty cell at the next free position. If \a size is greater
  than 0 then the empty cell has a fixed height or width.
  If the groupbox is oriented horizontally then the empty cell has a fixed
  height and if oriented vertically it has a fixed width.

  Use this method to separate the widgets in the groupbox or to skip
  the next free cell. For performance reasons call this method after
  calling setColumnLayout(), setColumns() or setOrientation(). It is in
  general a good idea to call these methods first (if needed at all) and
  insert the widgets and spaces afterwards.
*/
void QGroupBox::addSpace( int size )
{
    QApplication::sendPostedEvents( this, QEvent::ChildInserted );

    if ( nCols <= 0 || nRows <= 0 )
	return;

    if ( row >= nRows || col >= nCols )
	grid->expand( row+1, col+1 );

    if ( size > 0 ) {
	QSpacerItem *spacer
	    = new QSpacerItem( ( dir == Horizontal ) ? 0 : size,
			       ( dir == Vertical ) ? 0 : size,
			       QSizePolicy::Fixed, QSizePolicy::Fixed );
	grid->addItem( spacer, row, col );
    }

    skip();
}

/*!
  Returns the numbers of columns in the groupbox as passed to
  the constructor, setColumns() or setColumnLayout().
*/
int QGroupBox::columns() const
{
    if ( dir == Horizontal )
	return nCols;
    return nRows;
}

/*!
  Changes the numbers of columns to \a c. Usually it is no good idea
  to use the method since it is slow (it does a complete layout).
  Better set the numbers of columns directly in the constructor.

  \sa column() setColumnLayout()
*/
void QGroupBox::setColumns( int c )
{
    setColumnLayout( c, dir );
}

/*!
  \fn Orientation QGroupBox::orientation() const
  Returns the current orientation of the groupbox.

  \sa setOrientation()
*/

/*!
  Changes the orientation of the groupbox. Usually it is no good idea
  to use the method since it is slow (it does a complete layout).
  Better set the orientation directly in the constructor.

  \sa orientation()
*/
void QGroupBox::setOrientation( Qt::Orientation o )
{
    setColumnLayout( columns(), o );
}

/*!
  Changes the layout of the group box. This function is only useful in
  combination with the default constructor that does not take any
  layout information. This function will put all existing children in
  the new layout. Nevertheless is is not good programming style to
  call this function after children have been inserted.

  \sa setOrientation() setColumns()
 */
void QGroupBox::setColumnLayout(int columns, Orientation direction)
{
    if ( layout() )
      delete layout();
    vbox = 0;
    grid = 0;

    if ( columns < 0 ) // if 0, we create the vbox but not the grid. See below.
	return;

    vbox = new QVBoxLayout( this, 11, 0 );

    QSpacerItem *spacer = new QSpacerItem( 0, 0, QSizePolicy::Minimum,
					   QSizePolicy::Fixed );
    d = (QGroupBoxPrivate*) spacer;
    setTextSpacer();
    vbox->addItem( spacer );

    nCols = 0;
    nRows = 0;
    dir = direction;

    // Send all child events and ignore them. Otherwise we will end up
    // with doubled insertion. This won't do anything because nCols ==
    // nRows == 0.
    QApplication::sendPostedEvents( this, QEvent::ChildInserted );

    // if 0 or smaller , create a vbox-layout but no grid. This allows
    // the designer to handle its own grid layout in a group box.
    if ( columns <= 0 )
	return;

    dir = direction;
    if ( dir == Horizontal ) {
	nCols = columns;
	nRows = 1;
    } else {
	nCols = 1;
	nRows = columns;
    }
    grid = new QGridLayout( nRows, nCols, 5 );
    row = col = 0;
    grid->setAlignment( AlignTop );
    vbox->addLayout( grid );

    // Add all children
    if ( children() ) {
	QObjectListIt it( *children() );
	QWidget *w;
	while( (w=(QWidget *)it.current()) != 0 ) {
	    ++it;
	    if ( w->isWidgetType() )
		insertWid( w );
	}
    }
}


/*! \reimp  */
bool QGroupBox::event( QEvent * e )
{
    if ( e->type() == QEvent::LayoutHint && layout() )
	setTextSpacer();
    return QFrame::event( e );
}

/*!\reimp */
void QGroupBox::childEvent( QChildEvent *c )
{
    // Similar to QGrid::childEvent()
    if ( !grid || !c->inserted() || !c->child()->isWidgetType() )
	return;
    QWidget *child = (QWidget*)c->child();
    if ( !child->isTopLevel() ) //ignore dialogs etc.
	insertWid( child );
}

void QGroupBox::insertWid( QWidget* w )
{
    if ( row >= nRows || col >= nCols )
	grid->expand( row+1, col+1 );
    grid->addWidget( w, row, col );
    skip();
    QApplication::postEvent( this, new QEvent( QEvent::LayoutHint ) );
}


void QGroupBox::skip()
{
    // Same as QGrid::skip()
    if ( dir == Horizontal ) {
	if ( col+1 < nCols ) {
	    col++;
	} else {
	    col = 0;
	    row++;
	}
    } else { //Vertical
	if ( row+1 < nRows ) {
	    row++;
	} else {
	    row = 0;
	    col++;
	}
    }
}


/*!  This private slot finds a nice widget in this group box that can
accept focus, and gives it focus.
*/

void QGroupBox::fixFocus()
{
    QFocusData * fd = focusData();
    QWidget * orig = fd->focusWidget();
    QWidget * best = 0;
    QWidget * candidate = 0;
    QWidget * w = orig;
    do {
	QWidget * p = w;
	while( p && p != this && !p->isTopLevel() )
	    p = p->parentWidget();
	if ( p == this && ( w->focusPolicy() & TabFocus ) == TabFocus ) {
	    if ( w->hasFocus() ||
		 ( !best &&
		   w->inherits( "QRadioButton" ) &&
		   ((QRadioButton*)w)->isChecked() ) )
		// we prefer a checked radio button or a widget that
		// already has focus, if there is one
		best = w;
	    else if ( !candidate )
		// but we'll accept anything that takes focus
		candidate = w;
	}
	w = fd->next();
    } while( w != orig );
    if ( best )
	best->setFocus();
    else if ( candidate )
	candidate->setFocus();
}


/*!
  Sets the right framerect depending on the title. Also calculates the
  visible part of the title.
 */
void QGroupBox::calculateFrame()
{
    lenvisible = str.length();

    if ( lenvisible ) { // do we have a label?
	QFontMetrics fm = fontMetrics();
	int h = fm.height();
	while ( lenvisible ) {
	    int tw = fm.width( str, lenvisible ) + 2*fm.width(QChar(' '));
	    if ( tw < width() )
		break;
	    lenvisible--;
	}
	if ( lenvisible ) { // but do we also have a visible label?
	    QRect r = rect();
	    r.setTop( h/2 );				// frame rect should be
	    setFrameRect( r );			//   smaller than client rect
	    return;
	}
    }

    // no visible label
    setFrameRect( QRect(0,0,0,0) );		//  then use client rect
}



/*! \reimp
 */
void QGroupBox::focusInEvent( QFocusEvent * )
{ // note no call to super
    fixFocus();
}


/*!\reimp
 */
void QGroupBox::fontChange( const QFont & oldFont )
{
    calculateFrame();
    setTextSpacer();
    QWidget::fontChange( oldFont );
}

/*!
  \reimp
*/

QSize QGroupBox::sizeHint() const
{
    QFontMetrics fm( font() );
    int tw = fm.width( title() ) + 2 * fm.width( "xx" );

    QSize s;
    if ( layout() ) {
	s = QFrame::sizeHint();
	return s.expandedTo( QSize( tw, 0 ) );
    } else {
	QRect r = childrenRect();
	QSize s( 100, 50 );
	s = s.expandedTo( QSize( tw, 0 ) );
	if ( r.isNull() )
	    return s;

	return s.expandedTo( QSize( r.width() + 2 * r.x(), r.height()+ 2 * r.y() ) );
    }
}

#endif
