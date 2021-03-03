/****************************************************************************
** $Id: qt/src/widgets/qwhatsthis.cpp   2.3.2   edited 2001-01-26 $
**
** Implementation of QWhatsThis class
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

#include "qwhatsthis.h"
#ifndef QT_NO_WHATSTHIS
#include "qapplication.h"
#include "qpaintdevicemetrics.h"
#include "qpixmap.h"
#include "qpainter.h"
#include "qtimer.h"
#include "qptrdict.h"
#include "qtoolbutton.h"
#include "qshared.h"
#include "qcursor.h"
#include "qbitmap.h"
#include "qtooltip.h"
#include "qsimplerichtext.h"
#include "qstylesheet.h"

// REVISED: warwick
/*!
  \class QWhatsThis qwhatsthis.h

  \brief The QWhatsThis class provides a simple description of any
  widget, e.g. answering the question "what's this?"

  \ingroup helpsystem

  <i>What's This</i> help is part of an application's
  <a href=helpsystem.html>online help systems</a>,
  offering users a level of detail between
  tool tips and full text browsing windows.

  QWhatsThis provides a single window with a single explanatory
  text, which pops up quickly when the user asks "what's this?", and
  goes away as soon as the user does something else.

  To assign <i>What's This?</i> text to a widget, you simply
  call QWhatsThis::add() for the widget. To assign text to a
  menu item, call QMenuData::setWhatsThis(), and for a global
  accelerator key, call QAccel::setWhatsThis().

  The text can be either rich text or plain text.  If you
  specify a rich text formatted string, it will be rendered using the
  default stylesheet. This makes it also possible to embed images. See
  QStyleSheet::defaultSheet() for details.

  By default, the user will be able to view the text for a widget
  by pressing Shift-F1 while the widget has focus.
  On window systems where a context help button is provided in
  the window decorations, that button enters <i>What's This?</i> mode.
  In this mode, if the user
  clicks on a widget, help will be given for the widget.  The mode
  is left when help is given or when the user presses the Escape key.

  An alternative way to enter <i>What's This?</i> mode is
  to use the ready-made toolbar tool button from
  QWhatsThis::whatsThisButton().
  If you are using QMainWindow, you can also use
  the QMainWindow::whatsThis() slot to invoke the mode from a menu item.

  <img src="whatsthis.png" width="284" height="246">

  For more control, you can create a dedicated QWhatsThis object for a
  special widget. By subclassing and reimplementing QWhatsThis::text()
  it is possible to have different explanatory texts depending on the
  position of the mouse click.

  If your widget needs even more control, see
  QWidget::customWhatsThis().

  To remove added text, you can use QWhatsThis::remove(), but since
  the text is automatically removed when the widget is destroyed,
  this is rarely needed.

  \sa QToolTip
*/

// a special button
class QWhatsThisButton: public QToolButton
{
    Q_OBJECT

public:
    QWhatsThisButton( QWidget * parent, const char * name );
    ~QWhatsThisButton();

public slots:
    void mouseReleased();

};

class QWhatsThisPrivate: public QObject
{
    Q_OBJECT
public:

    // an item for storing texts
    struct WhatsThisItem: public QShared
    {
	WhatsThisItem(): QShared() { whatsthis = 0; }
	~WhatsThisItem();
	QString s;
	QWhatsThis* whatsthis;
    };

    // the (these days pretty small) state machine
    enum State { Inactive, Waiting };

    QWhatsThisPrivate();
    ~QWhatsThisPrivate();

    bool eventFilter( QObject *, QEvent * );

    WhatsThisItem* newItem( QWidget * widget );
    void add( QWidget * widget, QWhatsThis* special );
    void add( QWidget * widget, const QString& text );

    // say it.
    void say( QWidget *, const QString&, const QPoint&  );
    void say_helper(QWidget*,const QPoint& ppos,bool);

    // setup and teardown
    static void tearDownWhatsThis();
    static void setUpWhatsThis();

    void leaveWhatsThisMode();

    // variables
    QWidget * whatsThat;
    QPtrDict<WhatsThisItem> * dict;
    QPtrDict<QWidget> * tlw;
    QPtrDict<QWhatsThisButton> * buttons;
    State state;

#ifndef QT_NO_CURSOR
    QCursor * cursor;
#endif

    QString currentText;

private slots:
    void cleanupWidget()
    {
	const QObject* o = sender();
	if ( o->isWidgetType() ) // sanity
	    QWhatsThis::remove((QWidget*)o);
    }
};


// static, but static the less-typing way
static QWhatsThisPrivate * wt = 0;


// the item
QWhatsThisPrivate::WhatsThisItem::~WhatsThisItem()
{
    if ( count )
	qFatal( "Internal error #10%d in What's This", count );
}


static const char * button_image[] = {
"16 16 3 1",
" 	c None",
"o	c #000000",
"a	c #000080",
"o        aaaaa  ",
"oo      aaa aaa ",
"ooo    aaa   aaa",
"oooo   aa     aa",
"ooooo  aa     aa",
"oooooo  a    aaa",
"ooooooo     aaa ",
"oooooooo   aaa  ",
"ooooooooo aaa   ",
"ooooo     aaa   ",
"oo ooo          ",
"o  ooo    aaa   ",
"    ooo   aaa   ",
"    ooo         ",
"     ooo        ",
"     ooo        "};

#ifndef QT_NO_CURSOR

#define cursor_bits_width 32
#define cursor_bits_height 32
static unsigned char cursor_bits_bits[] = {
  0x01, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x05, 0xf0, 0x07, 0x00,
  0x09, 0x18, 0x0e, 0x00, 0x11, 0x1c, 0x0e, 0x00, 0x21, 0x1c, 0x0e, 0x00,
  0x41, 0x1c, 0x0e, 0x00, 0x81, 0x1c, 0x0e, 0x00, 0x01, 0x01, 0x07, 0x00,
  0x01, 0x82, 0x03, 0x00, 0xc1, 0xc7, 0x01, 0x00, 0x49, 0xc0, 0x01, 0x00,
  0x95, 0xc0, 0x01, 0x00, 0x93, 0xc0, 0x01, 0x00, 0x21, 0x01, 0x00, 0x00,
  0x20, 0xc1, 0x01, 0x00, 0x40, 0xc2, 0x01, 0x00, 0x40, 0x02, 0x00, 0x00,
  0x80, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, };

#define cursor_mask_width 32
#define cursor_mask_height 32
static unsigned char cursor_mask_bits[] = {
  0x01, 0x00, 0x00, 0x00, 0x03, 0xf0, 0x07, 0x00, 0x07, 0xf8, 0x0f, 0x00,
  0x0f, 0xfc, 0x1f, 0x00, 0x1f, 0x3e, 0x1f, 0x00, 0x3f, 0x3e, 0x1f, 0x00,
  0x7f, 0x3e, 0x1f, 0x00, 0xff, 0x3e, 0x1f, 0x00, 0xff, 0x9d, 0x0f, 0x00,
  0xff, 0xc3, 0x07, 0x00, 0xff, 0xe7, 0x03, 0x00, 0x7f, 0xe0, 0x03, 0x00,
  0xf7, 0xe0, 0x03, 0x00, 0xf3, 0xe0, 0x03, 0x00, 0xe1, 0xe1, 0x03, 0x00,
  0xe0, 0xe1, 0x03, 0x00, 0xc0, 0xe3, 0x03, 0x00, 0xc0, 0xe3, 0x03, 0x00,
  0x80, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, };

#endif


// the button class
QWhatsThisButton::QWhatsThisButton( QWidget * parent, const char * name )
    : QToolButton( parent, name )
{
    QPixmap p( button_image );
    setPixmap( p );
    setToggleButton( TRUE );
    setAutoRaise( TRUE );
    setFocusPolicy( NoFocus );
    setTextLabel( tr( "What's this?" ) );
    wt->buttons->insert( (void *)this, this );
    connect( this, SIGNAL( released() ),
	     this, SLOT( mouseReleased() ) );
}


QWhatsThisButton::~QWhatsThisButton()
{
    if ( wt && wt->buttons )
	wt->buttons->take( (void *)this );
}


void QWhatsThisButton::mouseReleased()
{
    if ( wt->state == QWhatsThisPrivate::Inactive && isOn() ) {
	QWhatsThisPrivate::setUpWhatsThis();
#ifndef QT_NO_CURSOR
	QApplication::setOverrideCursor( *wt->cursor, FALSE );
#endif
	wt->state = QWhatsThisPrivate::Waiting;
	qApp->installEventFilter( wt );
    }
}


// the what's this manager class
QWhatsThisPrivate::QWhatsThisPrivate()
    : QObject( 0, "global what's this object" )
{
    qAddPostRoutine( tearDownWhatsThis );
    whatsThat = 0;
    dict = new QPtrDict<QWhatsThisPrivate::WhatsThisItem>;
    tlw = new QPtrDict<QWidget>;
    wt = this;
    buttons = new QPtrDict<QWhatsThisButton>;
    state = Inactive;
#ifndef QT_NO_CURSOR
    cursor = new QCursor( QBitmap( cursor_bits_width, cursor_bits_height,
				   cursor_bits_bits, TRUE ),
			  QBitmap( cursor_mask_width, cursor_mask_height,
				   cursor_mask_bits, TRUE ),
			  1, 1 );
#endif
}

QWhatsThisPrivate::~QWhatsThisPrivate()
{
#ifndef QT_NO_CURSOR
    if ( state == Waiting )
	QApplication::restoreOverrideCursor();
#endif

    // the two straight-and-simple dicts
    delete tlw;
    delete buttons;

    // then delete the complex one.
    QPtrDictIterator<WhatsThisItem> it( *dict );
    WhatsThisItem * i;
    QWidget * w;
    while( (i=it.current()) != 0 ) {
	w = (QWidget *)it.currentKey();
	++it;
	dict->take( w );
	i->deref();
	if ( !i->count )
	    delete i;
    }
    delete dict;
#ifndef QT_NO_CURSOR
    delete cursor;
#endif
    delete whatsThat;

    // and finally lose wt
    wt = 0;
}

bool QWhatsThisPrivate::eventFilter( QObject * o, QEvent * e )
{
    if ( !o || !e )
	return FALSE;

    if ( o == whatsThat ) {
	if (e->type() == QEvent::MouseButtonPress  ||
	    e->type() == QEvent::KeyPress ) {
	    whatsThat->hide();
	    return TRUE;
	}
#ifdef _WS_QWS_
       else if ( e->type() == QEvent::Paint ) {
	   wt->say_helper(0,QPoint(0,0),FALSE);
       }
#endif
	return FALSE;
    }

    switch( state ) {
    case Waiting:
	if ( e->type() == QEvent::MouseButtonPress && o->isWidgetType() ) {
	    QWidget * w = (QWidget *) o;
	    if ( ( (QMouseEvent*)e)->button() == RightButton )
		return FALSE; // ignore RMB
	    if ( w->customWhatsThis() )
		return FALSE;
	    QWhatsThisPrivate::WhatsThisItem * i = 0;
	    while( w && !i ) {
		i = dict->find( w );
		if ( !i )
		    w = w->parentWidget();
	    }

	    leaveWhatsThisMode();
	    if (!i )
		return TRUE;
	    QPoint pos =  ((QMouseEvent*)e)->pos();
	    if ( i->whatsthis )
		say( w, i->whatsthis->text( pos ), w->mapToGlobal(pos) );
	    else
		say( w, i->s, w->mapToGlobal(pos) );
	    return TRUE;
	} else if ( e->type() == QEvent::MouseButtonRelease ) {
	    if ( ( (QMouseEvent*)e)->button() == RightButton )
		return FALSE; // ignore RMB
	    return !o->isWidgetType() || !((QWidget*)o)->customWhatsThis();
	} else if ( e->type() == QEvent::MouseMove ) {
	    return !o->isWidgetType() || !((QWidget*)o)->customWhatsThis();
	} else if ( e->type() == QEvent::KeyPress ) {
	    QKeyEvent* kev = (QKeyEvent*)e;

	    if (kev->key() == Qt::Key_Escape) {
		leaveWhatsThisMode();
		return TRUE;
	    }
	    else if ( kev->key() == Key_Menu ||
		      ( kev->key() == Key_F10 && kev->state() == ShiftButton ) )
		return FALSE; // ignore these keys, they are used for context menus
	    else if ( kev->state() == kev->stateAfter() &&
		      kev->key() != Key_Meta )  // not a modifier key
		leaveWhatsThisMode();

	} else if ( e->type() == QEvent::MouseButtonDblClick ) {
	    return TRUE;
	}
	break;
    case Inactive:
 	if ( e->type() == QEvent::Accel &&
 	     ((QKeyEvent *)e)->key() == Key_F1 &&
 	     o->isWidgetType() &&
 	     ((QKeyEvent *)e)->state() == ShiftButton ) {
 	    QWidget * w = ((QWidget *)o)->focusWidget();
 	    QWhatsThisPrivate::WhatsThisItem *i = w ? dict->find(w) : 0;
 	    if ( i && !i->s.isNull() ) {
		if ( i->whatsthis )
		    say( w, i->whatsthis->text( QPoint(0,0) ),
			 w->mapToGlobal( w->rect().center() ) );
		else
		    say( w, i->s, w->mapToGlobal( w->rect().center() ));
		((QKeyEvent *)e)->accept();
		return TRUE;
 	    }
 	}
	break;
    }
    return FALSE;
}



void QWhatsThisPrivate::setUpWhatsThis()
{
    if ( !wt )
	wt = new QWhatsThisPrivate();
}


void QWhatsThisPrivate::tearDownWhatsThis()
{
    delete wt;
    wt = 0;
}



void QWhatsThisPrivate::leaveWhatsThisMode()
{
    if ( state == Waiting ) {
	QPtrDictIterator<QWhatsThisButton> it( *(wt->buttons) );
	QWhatsThisButton * b;
	while( (b=it.current()) != 0 ) {
	    ++it;
	    b->setOn( FALSE );
	}
#ifndef QT_NO_CURSOR
	QApplication::restoreOverrideCursor();
#endif
	state = Inactive;
	qApp->removeEventFilter( this );
    }
}



void QWhatsThisPrivate::say_helper(QWidget* widget,const QPoint& ppos,bool init)
{
    const int shadowWidth = 6;   // also used as '5' and '6' and even '8' below
    const int vMargin = 8;
    const int hMargin = 12;

    if ( currentText.isEmpty() )
	return;

    QRect r;
#ifndef QT_NO_RICHTEXT
    QSimpleRichText* doc = 0;

    if ( QStyleSheet::mightBeRichText( currentText ) ) {
	doc = new QSimpleRichText( currentText, whatsThat->font() );
	doc->adjustSize();
	r.setRect( 0, 0, doc->width(), doc->height() );
    }
    else
#endif
    {
	int sw = QApplication::desktop()->width() / 3;
	if ( sw < 200 )
	    sw = 200;
	else if ( sw > 300 )
	    sw = 300;

	r = whatsThat->fontMetrics().boundingRect( 0, 0, sw, 1000,
			    AlignLeft + AlignTop + WordBreak + ExpandTabs,
			    currentText );
    }

    int w = r.width() + 2*hMargin;
    int h = r.height() + 2*vMargin;

    if ( init ) {
	// okay, now to find a suitable location

	int x;

	// first try locating the widget immediately above/below,
	// with nice alignment if possible.
	QPoint pos;
	if ( widget )
	    pos = widget->mapToGlobal( QPoint( 0,0 ) );

	if ( widget && w > widget->width() + 16 )
		x = pos.x() + widget->width()/2 - w/2;
	else
	    x = ppos.x() - w/2;

	// squeeze it in if that would result in part of what's this
	// being only partially visible
	if ( x + w > QApplication::desktop()->width() )
	    x = (widget? (QMIN(QApplication::desktop()->width(),
			      pos.x() + widget->width())
			 ) : QApplication::desktop()->width() )
		- w;

	int sx = QApplication::desktop()->x();
	int sy = QApplication::desktop()->y();
	if ( x < sx )
	    x = sx;

	int y;
	if ( widget && h > widget->height() + 16 ) {
	    y = pos.y() + widget->height() + 2; // below, two pixels spacing
	    // what's this is above or below, wherever there's most space
	    if ( y + h + 10 > QApplication::desktop()->height() )
		y = pos.y() + 2 - shadowWidth - h; // above, overlap
	}
	y = ppos.y() + 2;

	// squeeze it in if that would result in part of what's this
	// being only partially visible
	if ( y + h > QApplication::desktop()->height() )
	    y = ( widget ? (QMIN(QApplication::desktop()->height(),
				 pos.y() + widget->height())
			    ) : QApplication:: desktop()->height() )
		- h;
	if ( y < sy )
	    y = sy;
	
	whatsThat->setGeometry( x, y, w + shadowWidth, h + shadowWidth );
	whatsThat->show();
    }

    // now for super-clever shadow stuff.  super-clever mostly in
    // how many window system problems it skirts around.

    QPainter p( whatsThat );
    p.setPen( whatsThat->colorGroup().foreground() );
    p.drawRect( 0, 0, w, h );
    p.setPen( whatsThat->colorGroup().mid() );
    p.setBrush( whatsThat->colorGroup().background() );
    p.drawRect( 1, 1, w-2, h-2 );
    p.setPen( whatsThat->colorGroup().foreground() );

#ifndef QT_NO_RICHTEXT
    if ( doc ) {
	doc->draw( &p, hMargin, vMargin, r, whatsThat->colorGroup(), 0 );
	delete doc;
    }
    else
#endif
    {
	p.drawText( hMargin, vMargin, r.width(), r.height(),
		    AlignLeft + AlignTop + WordBreak + ExpandTabs,
		    currentText );
    }
    p.setPen( whatsThat->colorGroup().shadow() );

    p.drawPoint( w + 5, 6 );
    p.drawLine( w + 3, 6,
		w + 5, 8 );
    p.drawLine( w + 1, 6,
		w + 5, 10 );
    int i;
    for( i=7; i < h; i += 2 )
	p.drawLine( w, i,
		    w + 5, i + 5 );
    for( i = w - i + h; i > 6; i -= 2 )
	p.drawLine( i, h,
		    i + 5, h + 5 );
    for( ; i > 0 ; i -= 2 )
	p.drawLine( 6, h + 6 - i,
		    i + 5, h + 5 );
}


void QWhatsThisPrivate::say( QWidget * widget, const QString &text, const QPoint& ppos)
{
    currentText = text;

    // make the widget, and set it up
    if ( !whatsThat ) {
	whatsThat = new QWidget( 0, "automatic what's this? widget",
				 WType_Popup );
	whatsThat->setBackgroundMode( QWidget::NoBackground );
	whatsThat->setPalette( QToolTip::palette(), TRUE );
	whatsThat->installEventFilter( this );
    }
    say_helper(widget,ppos,TRUE);
}

QWhatsThisPrivate::WhatsThisItem* QWhatsThisPrivate::newItem( QWidget * widget )
{
    WhatsThisItem * i = dict->find( (void *)widget );
    if ( i )
	QWhatsThis::remove( widget );
    i = new WhatsThisItem;
    dict->insert( (void *)widget, i );
    QWidget * t = widget->topLevelWidget();
    if ( !tlw->find( (void *)t ) ) {
	tlw->insert( (void *)t, t );
	t->installEventFilter( this );
    }
    connect( widget, SIGNAL(destroyed()), this, SLOT(cleanupWidget()) );
    return i;
}

void QWhatsThisPrivate::add( QWidget * widget, QWhatsThis* special )
{
    newItem( widget )->whatsthis = special;
}

void QWhatsThisPrivate::add( QWidget * widget, const QString &text )
{
    newItem( widget )->s = text;
}


// and finally the What's This class itself

/*!
  Adds \a text as <i>What's This</i> help for \a widget. If the text is rich
  text formatted (ie. it contains markup), it will be rendered with
  the default stylesheet QStyleSheet::defaultSheet().

  The text is destroyed if the widget is later destroyed and so need
  not be explicitly removed.

  \sa remove()
*/
void QWhatsThis::add( QWidget * widget, const QString &text )
{
    QWhatsThisPrivate::setUpWhatsThis();
    wt->add(widget,text);
}


/*!
  Removes the <i>What's This</i> help for \a widget. This happens
  automatically if the widget is destroyed.
  \sa add()
*/
void QWhatsThis::remove( QWidget * widget )
{
    QWhatsThisPrivate::setUpWhatsThis();
    QWhatsThisPrivate::WhatsThisItem * i = wt->dict->find( (void *)widget );
    if ( !i )
	return;

    wt->dict->take( (void *)widget );

    i->deref();
    if ( !i->count )
	delete i;
}


/*!
  Returns the text for \a widget, or a null string if there
  is no <i>What's This</i> help for \a widget.

  \sa add()
*/
QString QWhatsThis::textFor( QWidget * widget, const QPoint& pos)
{
    QWhatsThisPrivate::setUpWhatsThis();
    QWhatsThisPrivate::WhatsThisItem * i = wt->dict->find( widget );
    if (!i)
	return QString::null;
    return i->whatsthis? i->whatsthis->text( pos ) : i->s;
}


/*!
  Creates a QToolButton pre-configured
  to enter <i>What's This</i> mode when clicked. You
  will often use this with a toolbar:
  \code
     (void)QWhatsThis::whatsThisButton( my_help_tool_bar );
  \endcode
*/
QToolButton * QWhatsThis::whatsThisButton( QWidget * parent )
{
    QWhatsThisPrivate::setUpWhatsThis();
    return new QWhatsThisButton( parent,
				 "automatic what's this? button" );
}


/*! \base64 whatsthis.png

iVBORw0KGgoAAAANSUhEUgAAARwAAAD2CAMAAAAzgsiCAAACmlBMVEX////FxsWlpaXCw8LA
wcC+vr67vLu5ubm3t7e0tLSysrKwsLCtra2rq6upqKmmpqako6SioaJzdXODmcWfn59ziayd
nJ2bmps5RFLAwMB0iq2FmcCIncNgYGDExMTBwcG8vLyysbGvr6+srKyqqqqnp6elpKSioqKg
n5+dnZ27u7u5uLi2trazs7Oura2rqqqoqKijoqIAAH8AAH4AAH0AAHwAAHsAAHoAAHkAAHgA
AHcAAHYAAHUAAHQAAHMAAHIAAHEAAHAAAG8AAG4AAG0AAGwAAGsAAGoAAGkAAGgAAGcAAGYA
AGUAAGQAAGMAAGIAAGEAAGAAAF8AAF4AAF0AAFwAAFsAAFoAAFkAAFgAAFcAAFYAAFUAAFQA
AFMAAFIAAFGampqHnMKryv+oyfKpx/t2j62Yl5eHm8JabIpleptwh6t8lLuHocuSrtyeu+yp
yPxWbIeVlZWXl5eGm8FabYtZbYqTkpKVlJSGmsGQj49aa4lidZVqfqFyh616kbiBmsSSkZGF
msClzvqOjY2Jo9CPj49hdZVpfqB4kLaAmMGHocyPqteLioqRrduNjIyEmb9fc5JnfJ1uhKh2
jbN+lr6Fn8mNqNSVsN+IiIiZtueKiYmDmL5keZpsgqV0i7B7k7uDnMaLpdGat+eGhYWhv/OH
hoaDl75qf6JyiK15kbiBmsOJo86Qq9mYtOSgve+Dg4OpyP+Eg4OCl71vhqp3jrV/l8CGoMuO
qdaWsuGduuylw/eBgICBgYGClr11jLJ9lb2EnsiMptOUr96buOmjwfSqyf9+fX1/fn6Blrx8
e3uBlbx5eHh7enqAlbt3dnbs7OwAAACAgIAAAIAEBASAgwS/wr/z9wTz9/PGw8aEgoT//wDw
8PD//9xFzc+JAAAK1klEQVR4nO2diaMVVR3HD5RZaWUgxQPCNE2xXR48eQ94D9/zsQiyCoIg
4CRolpZ6M2mxlFYjl9TErTLK9ijbI9sjEdGDF9mc/6VzZjvLnPm9mcu9c8557/e5PO7cWe6b
+dzv2WbmAiEIgiAIgiAIgiAIgiAIUsA4nfG298ghxo173etPe8Ppb3zTm8848y1vfdtZb58w
0WnqlaO5OXvCJKepV47m5h0oR5KjuXknypHkcDcTZCY7Tb1yeG4mdE0pSZdt6pXDyxSTk/uI
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

*/


/*!
  Constructs a dynamic <i>What's This</i> object for \a widget.

  When the widget is queried by the user, the text() function of
  this QWhatsThis will be called to provide the appropriate text,
  rather than using text assigned by add().
*/
QWhatsThis::QWhatsThis( QWidget * widget)
{
    QWhatsThisPrivate::setUpWhatsThis();
    wt->add(widget,this);
}


/*!
  Destructs the object and frees any allocated resources.
*/
QWhatsThis::~QWhatsThis()
{
}


/*!
  This virtual functions returns the text for position \e p in the
  widget that this <i>What's This</i> object documents.  If there is no
  <i>What's This</i> text for a position, QString::null is returned.

  The default implementation returns QString::null.
*/
QString QWhatsThis::text( const QPoint & )
{
    return QString::null;
}


/*!
  Enters <i>What's This</i>? mode and returns immediately.

  Qt will install a special cursor and take over mouse input
  until the user clicks somewhere, then show any help available and
  switch out of <i>What's This</i> mode.  Finally, Qt
  removes the special cursor and help window then restores ordinary event
  processing, at which point the left mouse button is not pressed.

  The user can also use the Escape key to leave <i>What's This</i>? mode.

\sa inWhatsThisMode(), leaveWhatsThisMode()
*/

void QWhatsThis::enterWhatsThisMode()
{
    QWhatsThisPrivate::setUpWhatsThis();
    if ( wt->state == QWhatsThisPrivate::Inactive ) {
#ifndef QT_NO_CURSOR
	QApplication::setOverrideCursor( *wt->cursor, FALSE );
#endif
	wt->state = QWhatsThisPrivate::Waiting;
	qApp->installEventFilter( wt );
    }
}


/*!
  Returns whether the application is in <i>What's This</i> mode.

  \sa enterWhatsThisMode(), leaveWhatsThisMode()
 */
bool QWhatsThis::inWhatsThisMode()
{
    if (!wt)
	return FALSE;
    return wt->state == QWhatsThisPrivate::Waiting;
}


/*!
  Leaves <i>What's This</i>? question mode

  This function is used internally by widgets that support
  QWidget::customWhatsThis(), applications do not usually call
  it.  An example for such a kind of widget is QPopupMenu: Menus still
  work normally in <i>What's This</i> mode, but provide help texts for single
  menu items instead.

  If \e text is not a null string, then a <i>What's This</i> help window is
  displayed at the global screen position \e pos.

\sa inWhatsThisMode(), enterWhatsThisMode()
*/
void QWhatsThis::leaveWhatsThisMode( const QString& text, const QPoint& pos )
{
    if ( !inWhatsThisMode() )
	return;

    wt->leaveWhatsThisMode();
    if ( !text.isNull() )
	wt->say( 0, text, pos );
}

#include "qwhatsthis.moc"
#endif
