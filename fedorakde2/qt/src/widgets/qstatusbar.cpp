/****************************************************************************
** $Id: qt/src/widgets/qstatusbar.cpp   2.3.2   edited 2001-10-26 $
**
** Implementation of QStatusBar class
**
** Created : 980119
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

#include "qstatusbar.h"
#ifndef QT_NO_STATUSBAR

#include "qlist.h"
#include "qlayout.h"
#include "qpainter.h"
#include "qtimer.h"
#include "qdrawutil.h"
#include "qapplication.h"
#include "qsizegrip.h"

// REVISED: warwick
/*!
  \class QStatusBar qstatusbar.h

  \brief The QStatusBar class provides a horizontal bar suitable for
  presenting status information.

  \ingroup application
  \ingroup helpsystem

  Each status indicator falls into one of three categories:

  <ul>
  <li> \e Temporary - occupies most of the status bar briefly.  Used
    e.g. for explaining tool tip texts or menu entries.
  <li> \e Normal - occupies part of the status bar and may be hidden
    by temporary messages.  Used e.g. for displaying the page and line
    number in a word processor.
  <li> \e Permanent - is never hidden.  Used for important mode
    indications.  Some applications put a Caps Lock indicator in the
    status bar.
  </ul>

  QStatusBar lets you display all three types of indicator.

  To display a \e temporary message, call message(), perhaps by
  connecting a suitable signal to it.  To remove a temporary message,
  call clear().
  There are two variants of message(), one which displays the message
  until the next clear() or mesage(), and one which also has a time limit:

  \code
     connect( loader, SIGNAL(progressMessage(const QString&)),
              statusBar(), SLOT(message(const QString&)) );

     statusBar()->message("Loading...");  // Initial message
     loader.loadStuff();                  // Emits progress messages
     statusBar()->message("Done.", 2000); // Final message for 2 seconds
  \endcode

  \e Normal and \e permanent messages are displayed by creating a small
  widget then adding it to the status bar with addWidget().  Widgets
  like QLabel, QProgressBar, or even QToolButton are useful for adding
  to status bars.  removeWidget() is used to remove widgets.

  \code
     statusBar()->addWidget(new MyReadWriteIndication(statusBar()));
  \endcode

  By default, QStatusBar provides a QSizeGrip in the lower-right corner.
  You can disable this with setSizeGripEnabled(FALSE);

  <img src=qstatusbar-m.png> <img src=qstatusbar-w.png>

  \sa QToolBar QMainWindow QLabel
  <a href="guibooks.html#fowler">GUI Design Handbook: Status Bar.</a>
*/


class QStatusBarPrivate
{
public:
    QStatusBarPrivate() {}

    struct SBItem {
	SBItem( QWidget* widget, int stretch, bool permanent )
	    : s( stretch ), w( widget ), p( permanent ) {}
	int s;
	QWidget * w;
	bool p;
    };

    QList<SBItem> items;
    QString tempItem;

    QBoxLayout * box;
    QTimer * timer;

#ifndef QT_NO_SIZEGRIP
    QSizeGrip * resizer;
#endif    
};


/*!
  Constructs a status bar with just a size grip.
  \sa setSizeGripEnabled()
*/
QStatusBar::QStatusBar( QWidget * parent, const char *name )
    : QWidget( parent, name )
{
    d = new QStatusBarPrivate;
    d->items.setAutoDelete( TRUE );
    d->box = 0;
#ifndef QT_NO_SIZEGRIP
    d->resizer = 0;
#endif
    d->timer = 0;

    setSizeGripEnabled(TRUE); // causes reformat()
}


/*!
  Destructs the status bar and frees any allocated resources.
*/
QStatusBar::~QStatusBar()
{
    delete d;
    d = 0;
}


/*!
  Adds \a widget to this status bar.

  \a widget is permanently visible if \a permanent is TRUE, and is
  obscured by temporary messages if \a permanent is FALSE.  The
  default is FALSE.

  \a stretch is used to compute a suitable size for \a widget as the
  status bar grows and shrinks. The default of 0 uses a minimum of space.

  If \a permanent is TRUE, \a widget is located at the far right of
  the status bar.  If \a permanent is FALSE (the default) \a widget is
  located just to the left of the first permanent widget.

  This function may cause some flicker.

  \sa removeWidget()
*/

void QStatusBar::addWidget( QWidget * widget, int stretch, bool permanent )
{
    if ( !widget ) {
#if defined(CHECK_NULL)
	qWarning( "QStatusBar::addWidget(): Cannot add null widget" );
#endif
	return;
    }

    QStatusBarPrivate::SBItem* item
	= new QStatusBarPrivate::SBItem( widget, stretch, permanent );

    d->items.last();
    while( !permanent && d->items.current() && d->items.current()->p )
	d->items.prev();

    d->items.insert( d->items.at() >= 0 ? d->items.at()+1 : 0, item );

    if ( !d->tempItem.isEmpty() && !permanent )
	widget->hide();

    reformat();
}


/*!
  Removes \a widget from the status bar.

  This function may cause some flicker.

  Note that \a widget is not deleted.

  \sa addWidget()
*/

void QStatusBar::removeWidget( QWidget* widget )
{
    if ( !widget )
	return;
    bool found = FALSE;
    QStatusBarPrivate::SBItem* item = d->items.first();
    while ( item && !found ) {
	if ( item->w == widget ) {
	    d->items.remove();
	    found = TRUE;
	}
	item = d->items.next();
    }

    if ( found )
	reformat();
#if defined(DEBUG)
    else
	qDebug( "QStatusBar::removeWidget(): Widget not found." );
#endif
}

/*!
  \fn bool QStatusBar::isSizeGripEnabled() const

  Returns whether the QSizeGrip in the bottom right of the status bar
  is enabled.

  \sa setSizeGripEnabled()
*/

bool QStatusBar::isSizeGripEnabled() const
{
#ifdef QT_NO_SIZEGRIP
    return FALSE;
#else    
    return !!d->resizer;
#endif
}

/*!
  Enables or disables the QSizeGrip in the bottom right of the status bar.
  By default, the size grip is enabled.

  \sa isSizeGripEnabled()
*/
void QStatusBar::setSizeGripEnabled(bool enabled)
{
#ifndef QT_NO_SIZEGRIP
    if ( !enabled != !d->resizer ) {
	if ( enabled ) {
	    d->resizer = new QSizeGrip( this, "QStatusBar::resizer" );
	} else {
	    delete d->resizer;
	    d->resizer = 0;
	}
	reformat();
	if ( d->resizer && isVisible() )
	    d->resizer->show();
    }
#endif
}


/*!
  Changes the status bar's appearance to account for item
  changes. Special subclasses may need this, but normally
  geometry management will take care of any necessary
  rearrangements.
*/
void QStatusBar::reformat()
{
    if ( d->box )
	delete d->box;

    QBoxLayout *vbox;
    if ( isSizeGripEnabled() ) {
	d->box = new QHBoxLayout( this );
	vbox = new QVBoxLayout( d->box );
    } else {
	vbox = d->box = new QVBoxLayout( this );
    }
    vbox->addSpacing( 3 );
    QBoxLayout* l = new QHBoxLayout( vbox );
    l->addSpacing( 3 );

    int maxH = fontMetrics().height();

    QStatusBarPrivate::SBItem* item = d->items.first();
    while ( item && !item->p ) {
	l->addWidget( item->w, item->s );
	l->addSpacing( 4 );
	int itemH = item->w->sizeHint().height();
	maxH = QMAX( maxH, itemH );
	item = d->items.next();
    }

    l->addStretch( 0 );

    while ( item ) {
	l->addWidget( item->w, item->s );
	l->addSpacing( 4 );
	int itemH = item->w->sizeHint().height();
	maxH = QMAX( maxH, itemH );
	item = d->items.next();
    }
#ifndef QT_NO_SIZEGRIP
    if ( d->resizer ) {
	maxH = QMAX( maxH, d->resizer->sizeHint().height() );
	d->box->addSpacing( 2 );
	d->box->addWidget( d->resizer, 0, AlignBottom );
    }
#endif
    l->addStrut( maxH );
    vbox->addSpacing( 2 );
    d->box->activate();
    repaint();
}




/*!
  Hide the normal status indicators and display \a message, until
  clear() or another message() is called.

  \sa clear()
*/
void QStatusBar::message( const QString &message )
{
    if ( d->tempItem == message )
	return;
    d->tempItem = message;
    if ( d->timer ) {
	delete d->timer;
	d->timer = 0;
    }
    hideOrShow();
}


/*!
  Hide the normal status indications and display \a message for \a
  ms milli-seconds, or until clear() or another message() is called,
  whichever is first.
*/
void QStatusBar::message( const QString &message, int ms )
{
    d->tempItem = message;

    if ( !d->timer ) {
	d->timer = new QTimer( this );
	connect( d->timer, SIGNAL(timeout()), this, SLOT(clear()) );
    }
    if ( ms > 0 )
	d->timer->start( ms );
    hideOrShow();
}


/*!
  Removes any temporary message being shown.

  \sa message()
*/

void QStatusBar::clear()
{
    if ( d->tempItem.isEmpty() )
	return;
    if ( d->timer ) {
	delete d->timer;
	d->timer = 0;
    }
    d->tempItem = QString::null;
    hideOrShow();
}


/*!
  Ensures that the right widgets are visible.  Used by message()
  and clear().
*/
void QStatusBar::hideOrShow()
{
    bool haveMessage = !d->tempItem.isEmpty();

    QStatusBarPrivate::SBItem* item = d->items.first();

    while( item && !item->p ) {
	if ( haveMessage )
	    item->w->hide();
	else
	    item->w->show();
	item = d->items.next();
    }

    repaint();
}


/*!
  Shows the temporary message, if appropriate.
*/
void QStatusBar::paintEvent( QPaintEvent * )
{
    bool haveMessage = !d->tempItem.isEmpty();

    QPainter p( this );
    QStatusBarPrivate::SBItem* item = d->items.first();
    while ( item ) {
	if ( (!haveMessage || item->p) && item->w->isVisible() )
	    qDrawShadeRect( &p, item->w->x()-1, item->w->y()-1,
			    item->w->width()+2, item->w->height()+2,
			    colorGroup(), TRUE, 1, 0, 0 );
	item = d->items.next();
    }
    if ( haveMessage ) {
	p.setPen( colorGroup().text() );
	// ### clip and add ellipsis if necessary
	p.drawText( 6, 0, width() - 12, height(), AlignVCenter + SingleLine,
		    d->tempItem );
    }
}

/*! \reimp
*/
void QStatusBar::resizeEvent( QResizeEvent * e )
{
#if 0
    QStatusBarPrivate::SBItem* item;    
    for ( item = d->items.first(); item; item = d->items.next() )
	item->w->setMinimumWidth( 30 );

    int mw = d->box->totalMinimumSize().width() - 30;
    for ( item = d->items.first(); item; item = d->items.next() )
	item->w->setMaximumWidth( width() - mw );
#endif
    QWidget::resizeEvent( e );
}

/*!
  \reimp
*/

bool QStatusBar::event( QEvent *e )
{
    if ( e->type() == QEvent::LayoutHint )
	update();
    if ( e->type() == QEvent::ChildRemoved ) {
	QStatusBarPrivate::SBItem* item = d->items.first();
	while ( item ) {
	    if ( item->w == ( (QChildEvent*)e )->child() )
		d->items.removeRef( item );
	    item = d->items.next();
	}
    }
    return QWidget::event( e );
}

#endif
