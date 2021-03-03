/**********************************************************************
** $Id: qt/src/widgets/qlineedit.cpp   2.3.2   edited 2001-10-19 $
**
** Implementation of QLineEdit widget class
**
** Created : 941011
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

#include "qlineedit.h"
#ifndef QT_NO_LINEEDIT
#include "qpainter.h"
#include "qdrawutil.h"
#include "qfontmetrics.h"
#include "qpixmap.h"
#include "qclipboard.h"
#include "qapplication.h"
#include "qvalidator.h"
#include "qdragobject.h"
#include "qtimer.h"
#include "qpopupmenu.h"
#include "qstringlist.h"
#include "qguardedptr.h"
#include <ctype.h>


struct QLineEditUndoItem
{
    QLineEditUndoItem(){pos=0;};
    QLineEditUndoItem( const QString& s, int p )
	: str(s),pos(p){}
#if defined(Q_FULL_TEMPLATE_INSTANTIATION)
    bool operator==( const QLineEditUndoItem& ) const { return FALSE; }
#endif
    QString str;
    int pos;
};

enum {
    IdUndo,
    IdRedo,
#ifndef QT_NO_CLIPBOARD
    IdCut,
    IdCopy,
    IdPaste,
#endif
    IdClear,
    IdSelectAll
};

struct QLineEditPrivate {
    QLineEditPrivate( QLineEdit * l ):
	frame(TRUE), mode(QLineEdit::Normal),
	readonly(FALSE), validator( 0 ),
	pm(0), pmDirty( TRUE ),
	blinkTimer( l, "QLineEdit blink timer" ),
	dragTimer( l, "QLineEdit drag timer" ),
	dndTimer( l, "DnD Timer" ),
	inDoubleClick( FALSE ), offsetDirty( FALSE ),
	undo(TRUE), needundo( FALSE ), ignoreUndoWithDel( FALSE ),
	mousePressed( FALSE ), dnd_primed( FALSE ), passwordChar( '*' ) {}

    bool frame;
    QLineEdit::EchoMode mode;
    bool readonly;
    const QValidator * validator;
    QPixmap * pm;
    bool pmDirty;
    QTimer blinkTimer;
    QTimer dragTimer, dndTimer;
    QRect cursorRepaintRect;
    bool inDoubleClick;
    bool offsetDirty;
    QValueList<QLineEditUndoItem> undoList;
    QValueList<QLineEditUndoItem> redoList;
    bool undo;
    bool needundo;
    bool ignoreUndoWithDel;
    bool mousePressed;
    QPoint dnd_startpos;
    bool dnd_primed;
    QChar passwordChar;
};


// REVISED: arnt
/*!
  \class QLineEdit qlineedit.h

  \brief The QLineEdit widget is a one-line text editor.

  \ingroup basic

  A line edit allows the user to enter and edit a single line of
  plain text, with a useful collection of editing functions, including
  undo & redo, cut & paste, and drag & drop.

  By changing the echoMode() of a line edit it can also be used
  as a "write-only" field, for inputs such as passwords.

  The length of the field can be constrained to a maxLength(),
  or the value can be arbitrarily constrained by setting a validator().

  A closely related class is QMultiLineEdit which allows multi-line
  editing.

  The default QLineEdit object has its own frame as specified by the
  Windows/Motif style guides, you can turn off the frame by calling
  setFrame( FALSE ).

  The default key bindings are described in keyPressEvent().
  A right-mouse-button menu presents a number of the editing commands
  to the user.

  <img src=qlined-m.png> <img src=qlined-w.png>

  \sa QMultiLineEdit QLabel QComboBox
  <a href="guibooks.html#fowler">GUI Design Handbook: Field, Entry,</a>
  <a href="guibooks.html#fowler">GUI Design Handbook: Field, Required.</a>
*/


/*! \enum QLineEdit::EchoMode

  This enum type describes how QLineEdit displays its
  contents.  The defined values are:
  <ul>
  <li> \c Normal - display characters as they are entered.  This is
	the default.
  <li> \c NoEcho - do not display anything. This may be appropriate
	for passwords where even the length of the password should
	be kept secret.
  <li> \c Password - display asterisks instead of the characters
	actually entered.
  </ul>

  \sa setEchoMode() echoMode() QMultiLineEdit::EchoMode
*/


/*!
  \fn void QLineEdit::textChanged( const QString& )
  This signal is emitted every time the text changes.
  The argument is the new text.
*/


static const int scrollTime = 40;		// mark text scroll time


/*!
  Constructs a line edit with no text.

  The maximum text length is set to 32767 characters.

  The \e parent and \e name arguments are sent to the QWidget constructor.

  \sa setText(), setMaxLength()
*/

QLineEdit::QLineEdit( QWidget *parent, const char *name )
    : QWidget( parent, name, WRepaintNoErase )
{
    init();
}


/*!
  Constructs a line edit containing the text \a contents.

  The cursor position is set to the end of the line and the maximum text
  length to 32767 characters.

  The \e parent and \e name arguments are sent to the QWidget constructor.

  \sa text(), setMaxLength()
*/

QLineEdit::QLineEdit( const QString & contents,
		      QWidget *parent, const char *name )
    : QWidget( parent, name )
{
    init();
    setText( contents );
}


/*!
  Destructs the line edit.
*/

QLineEdit::~QLineEdit()
{
    if ( d->pm )
	delete d->pm;
    delete d;
}


/*! Contains initialization common to both constructors. */

void QLineEdit::init()
{
    d = new QLineEditPrivate( this );
    connect( &d->blinkTimer, SIGNAL(timeout()),
	     this, SLOT(blinkSlot()) );
#ifndef QT_NO_DRAGANDDROP
    connect( &d->dragTimer, SIGNAL(timeout()),
	     this, SLOT(dragScrollSlot()) );
    connect( &d->dndTimer, SIGNAL(timeout()),
	     this, SLOT(doDrag()) );
#endif
    cursorPos = 0;
    offset = 0;
    maxLen = 32767;
    cursorOn = TRUE;
    markAnchor = 0;
    markDrag = 0;
    dragScrolling = FALSE;
    scrollingLeft = FALSE;
    tbuf = QString::fromLatin1("");
    setFocusPolicy( StrongFocus );
#ifndef QT_NO_CURSOR
    setCursor( ibeamCursor );
#endif
    setBackgroundMode( PaletteBase );
    setKeyCompression( TRUE );
    alignmentFlag = Qt::AlignLeft;
    setAcceptDrops( TRUE );
    //   Specifies that this widget can use more, but is able to survive on
    //   less, horizontal space; and is fixed vertically.
    setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) );
    ed = FALSE;
}


/*!
  Sets the line edit text to \e text, clears the selection and moves
  the cursor to the end of the line.

  If necessary the text is truncated to maxLength().

  \sa text()
*/

void QLineEdit::setText( const QString &text )
{
    QString oldText( tbuf );
    tbuf = text;
    if ( (int)tbuf.length() > maxLen )
	tbuf.truncate( maxLen );
    offset    = 0;
    cursorPos = 0;
    markAnchor = 0;
    markDrag = 0;
    end( FALSE );
#ifndef QT_NO_VALIDATOR
    if ( validator() )
	(void)validator()->validate( tbuf, cursorPos );
#endif
    d->pmDirty = TRUE;

    update();
    if ( d->undo ) {
	d->undoList.clear();
	d->redoList.clear();
	d->needundo = TRUE;
     }
    if ( oldText != tbuf )
	emit textChanged( tbuf );
}


/*!
  Selects all text (i.e. marks it) and moves the cursor to the
  end. This is useful when a default value has been inserted,
  since if the user types before clicking on the widget the
  selected text will be erased.
*/

void QLineEdit::selectAll()
{
    setSelection( 0, tbuf.length() );
    end( TRUE );
}



/*!
  Deselects all text (i.e. removes marking) and leaves the cursor at the
  current position.
*/

void QLineEdit::deselect()
{
    setSelection( cursorPos, 0 );
}


/*!
  Returns the text in the line.
  \sa setText()
*/

QString QLineEdit::text() const
{
    return tbuf;
}



/*!  Returns the text that is displayed.  This is normally
the same as text(), but can be e.g. "*****" if EchoMode is Password or
"" if it is NoEcho.

\sa setEchoMode() text() EchoMode
*/

QString QLineEdit::displayText() const
{
    QString res;

    switch( echoMode() ) {
    case Normal:
	res = tbuf;
	break;
    case NoEcho:
	res = QString::fromLatin1("");
	break;
    case Password:
	res.fill( d->passwordChar, tbuf.length() );
	break;
    }
    return res;
}



/*!
  Returns TRUE if part of the text has been marked by the user (e.g. by
  clicking and dragging).

  \sa markedText()
*/

bool QLineEdit::hasMarkedText() const
{
    return markAnchor != markDrag;
}

/*!
  Returns the text marked by the user (e.g. by clicking and
  dragging), or a \link QString::operator!() null string\endlink
  if no text is marked.
  \sa hasMarkedText()
*/

QString QLineEdit::markedText() const
{
    return tbuf.mid( minMark(), maxMark() - minMark() );
}

/*!
  Returns the maximum permitted length of the text in the editor.
  \sa setMaxLength()
*/

int QLineEdit::maxLength() const
{
    return maxLen;
}

/*!
  Set the maximum length of the text in the editor.  If the text is
  too long, it is chopped off at the limit. Any marked text will
  be unmarked.	The cursor position is set to 0 and the first part of the
  string is shown. \sa maxLength().
*/

void QLineEdit::setMaxLength( int m )
{
    maxLen = m;
    markAnchor = 0;
    markDrag = 0;
    if ( (int)tbuf.length() > maxLen ) {
	tbuf.truncate( maxLen );
	d->pmDirty = TRUE;
    }
    setCursorPosition( 0 );
    if ( d->pmDirty )
	update();
}

/*!
  \fn void  QLineEdit::returnPressed()
  This signal is emitted when the return or enter key is pressed.
*/


/*!
  Converts a key press into a line edit action.

  If return or enter is pressed and the current text is valid (or can be
  \link QValidator::fixup() made valid\endlink by the validator),
  the signal returnPressed is emitted.

  The default key bindings are:
  <ul>
  <li><i> Left Arrow </i> Move the cursor one character leftwards.
  <li><i> Right Arrow </i> Move the cursor one character rightwards.
  <li><i> Backspace </i> Delete the character to the left of the cursor.
  <li><i> Home </i> Move the cursor to the beginning of the line.
  <li><i> End </i> Move the cursor to the end of the line.
  <li><i> Delete </i> Delete the character to the right of the cursor.
  <li><i> Shift - Left Arrow </i> Move and mark text one character leftwards.
  <li><i> Shift - Right Arrow </i> Move and mark text one character rightwards.
  <li><i> Control-A </i> Move the cursor to the beginning of the line.
  <li><i> Control-B </i> Move the cursor one character leftwards.
  <li><i> Control-C </i> Copy the marked text to the clipboard.
  <li><i> Control-D </i> Delete the character to the right of the cursor.
  <li><i> Control-E </i> Move the cursor to the end of the line.
  <li><i> Control-F </i> Move the cursor one character rightwards.
  <li><i> Control-H </i> Delete the character to the left of the cursor.
  <li><i> Control-K </i> Delete to end of line
  <li><i> Control-V </i> Paste the clipboard text into line edit.
  <li><i> Control-X </i> Move the marked text to the clipboard.
  <li><i> Control-Z </i> Undo the last operation.
  <li><i> Control-Y </i> Redo the last undone operation.
  </ul>
  In addition, the following key bindings are used on Windows:
  <ul>
  <li><i> Shift - Delete </i> Cut the marked text, copy to clipboard
  <li><i> Shift - Insert </i> Paste the clipboard text into line edit
  <li><i> Control - Insert </i> Copy the marked text to the clipboard
  </ul>

  All other keys with valid ASCII codes insert themselves into the line.
*/

void QLineEdit::keyPressEvent( QKeyEvent *e )
{
    if ( e->key() == Key_Enter || e->key() == Key_Return ) {
#ifdef QT_NO_VALIDATOR
	emit returnPressed();
	e->ignore();
#else
	const QValidator * v = validator();
	if ( !v || v->validate( tbuf, cursorPos ) == QValidator::Acceptable ) {
	    emit returnPressed();
	    e->ignore();
	}
	else if ( v ) {
	    QString old( tbuf );
	    v->fixup( tbuf );
	    if ( old != tbuf ) {
		d->pmDirty = TRUE;
		if ( cursorPos > (int)tbuf.length() )
		    cursorPos = tbuf.length();
		update();
	    }
	    if ( v->validate( tbuf, cursorPos ) == QValidator::Acceptable )
		emit returnPressed();
	    e->ignore();
	}
#endif
	return;
    }
    if ( !d->readonly ) {
	QString t = e->text();
	if ( !t.isEmpty() && (!e->ascii() || e->ascii()>=32) &&
	     e->key() != Key_Delete &&
	     e->key() != Key_Backspace ) {
	    insert( t );
	    return;
	}
    }
    bool needundo = d->needundo;
    d->needundo = TRUE;
    bool ignoreUndoWithDel = d->ignoreUndoWithDel;
    d->ignoreUndoWithDel = FALSE;
    int unknown = 0;
    if ( e->state() & ControlButton ) {
	switch ( e->key() ) {
	case Key_A:
	    home( e->state() & ShiftButton );
	    break;
	case Key_B:
	    cursorLeft( e->state() & ShiftButton );
	    break;
#ifndef QT_NO_CLIPBOARD
	case Key_C:
	    copy();
	    break;
#endif
	case Key_D:
	    if ( !d->readonly ) {
		d->ignoreUndoWithDel = ignoreUndoWithDel;
		del();
	    }
	    break;
	case Key_E:
	    end( e->state() & ShiftButton );
	    break;
	case Key_F:
	    cursorRight( e->state() & ShiftButton );
	    break;
	case Key_H:
	    if ( !d->readonly ) {
		d->ignoreUndoWithDel = ignoreUndoWithDel;
		backspace();
	    }
	    break;
	case Key_K:
	    if ( !d->readonly && cursorPos < (int)tbuf.length() ) {
		QString t( tbuf );
		if ( d->undoList.isEmpty() || d->undoList.last().str != tbuf ) {
		    d->undoList += QLineEditUndoItem(tbuf, cursorPos );
		    d->redoList.clear();
		}
		t.truncate( cursorPos );
		validateAndSet( t, cursorPos, cursorPos, cursorPos );
	    }
	    break;
#ifndef QT_NO_CLIPBOARD
	case Key_V:
	    if ( !d->readonly )
		insert( QApplication::clipboard()->text() );
	    break;
	case Key_X:
	    if ( !d->readonly && hasMarkedText() && echoMode() == Normal ) {
		copy();
		del();
	    }
	    break;
#if defined (_WS_WIN_)
	case Key_Insert:
	    copy();
	    break;
#endif
#endif
	case Key_Right:
	    cursorWordForward( e->state() & ShiftButton );
	    break;
	case Key_Left:
	    cursorWordBackward( e->state() & ShiftButton );
	    break;
	case Key_Z:
	    if ( !d->readonly )
		undoInternal();
	    break;
	case Key_Y:
	    if ( !d->readonly )
		redoInternal();
	    break;
	default:
	    unknown++;
	}
    } else {
	switch ( e->key() ) {
	case Key_Left:
	    cursorLeft( e->state() & ShiftButton );
	    break;
	case Key_Right:
	    cursorRight( e->state() & ShiftButton );
	    break;
	case Key_Backspace:
	    if ( !d->readonly ) {
		d->ignoreUndoWithDel = ignoreUndoWithDel;
		backspace();
	    }
	    break;
	case Key_Home:
	    home( e->state() & ShiftButton );
	    break;
	case Key_End:
	    end( e->state() & ShiftButton );
	    break;
	case Key_Delete:
	    if ( !d->readonly ) {
#if defined (_WS_WIN_)
		if ( e->state() & ShiftButton ) {
		    cut();
		    break;
		}
#endif
		d->ignoreUndoWithDel = ignoreUndoWithDel;
		del();
	    }
	    break;
#if defined (_WS_WIN_)
	case Key_Insert:
	    if ( !d->readonly && e->state() & ShiftButton )
		paste();
	    else
		unknown++;
	    break;
#endif
	case Key_F14: // Undo key on Sun keyboards
	    if ( !d->readonly )
		undoInternal();
	    break;
#ifndef QT_NO_CLIPBOARD
	case Key_F16: // Copy key on Sun keyboards
	    copy();
	    break;
	case Key_F18: // Paste key on Sun keyboards
	    if ( !d->readonly )
		insert( QApplication::clipboard()->text() );
	    break;
	case Key_F20: // Cut key on Sun keyboards
	    if ( !d->readonly && hasMarkedText() && echoMode() == Normal ) {
		copy();
		del();
	    }
	    break;
#endif
	default:
	    unknown++;
	}
    }

    if ( unknown ) {				// unknown key
	d->needundo = needundo;
	e->ignore();
	return;
    }
}


/*!\reimp
*/

void QLineEdit::focusInEvent( QFocusEvent * e)
{
    d->pmDirty = TRUE;
    cursorOn = FALSE;
    blinkOn();
    if ( e->reason() == QFocusEvent::Tab )
	selectAll();
    d->pmDirty = TRUE;
    repaint( FALSE );
}


/*!\reimp
*/

void QLineEdit::focusOutEvent( QFocusEvent * e )
{
    if ( e->reason() != QFocusEvent::ActiveWindow
	 && e->reason() != QFocusEvent::Popup )
	deselect();
    d->dragTimer.stop();
    if ( cursorOn )
	blinkSlot();
    d->pmDirty = TRUE;
    repaint( FALSE );
}

/*!\reimp
*/
void QLineEdit::leaveEvent( QEvent * )
{
}


/*!\reimp
*/

void QLineEdit::paintEvent( QPaintEvent *e )
{
    if ( d->offsetDirty )
	updateOffset();
    if ( !d->pm || d->pmDirty ) {
	makePixmap();
	if ( d->pm->isNull() ) {
	    delete d->pm;
	    d->pm = 0;
	    return;
	}

	QPainter p( d->pm, this );

	const QColorGroup & g = colorGroup();
	QBrush bg = g.brush((isEnabled()) ? QColorGroup::Base :
			    QColorGroup::Background);
	QFontMetrics fm = fontMetrics();
	int markBegin = minMark();
	int markEnd = maxMark();

	p.fillRect( 0, 0, width(), height(), bg );

	QString display = displayText();
	QString before = display.mid( 0, markBegin );
	QString marked = display.mid( markBegin, markEnd - markBegin );
	QString after = display.mid( markEnd, display.length() );

	int y = (d->pm->height() + fm.height())/2 - fm.descent() - 1 ;

	int x = offset + 2;
	int w;

	w = fm.width( before );
	if ( x < d->pm->width() && x + w >= 0 ) {
	    p.setPen( g.text() );
	    p.drawText( x, y, before );
	}
	x += w;

	w = fm.width( marked );
	if ( x < d->pm->width() && x + w >= 0 ) {
	    p.fillRect( x, y-fm.ascent()-1, w, fm.height()+2,
			g.brush( QColorGroup::Highlight ) );
	    p.setPen( g.highlightedText() );
	    p.drawText( x, y, marked );
	}
	x += w;

	w = fm.width( after );
	if ( x < d->pm->width() && x + w >= 0 ) {
	    p.setPen( g.text() );
	    p.drawText( x, y, after );
	}
	// ... x += w;

	p.setPen( g.text() );

	d->cursorRepaintRect.setTop( y + frameW() - fm.ascent() );
	d->cursorRepaintRect.setHeight( fm.height() );
	d->pmDirty = FALSE;
    }

    QPainter p( this );

    if ( frame() ) {
	style().drawPanel( &p, 0, 0, width(), height(), colorGroup(),
			   TRUE, style().defaultFrameWidth() );
	p.drawPixmap( frameW(), frameW(), *d->pm );
    } else {
	p.drawPixmap( 0, 0, *d->pm );
    }

    if ( hasFocus() ) {
	d->cursorRepaintRect
	    = QRect( offset + frameW() +
		     fontMetrics().width( displayText().left( cursorPos ) ),
		     d->cursorRepaintRect.top(),
		     5, d->cursorRepaintRect.height() );

	int curYTop = d->cursorRepaintRect.y();
	int curYBot = d->cursorRepaintRect.bottom();
	int curXPos = d->cursorRepaintRect.x() + 2;
	if ( !d->readonly && cursorOn &&
	     d->cursorRepaintRect.intersects( e->rect() ) ) {
	    p.setPen( colorGroup().text() );
	    p.drawLine( curXPos, curYTop, curXPos, curYBot );
	    if ( style() != WindowsStyle ) {
		p.drawLine( curXPos - 2, curYTop, curXPos + 2, curYTop );
		p.drawLine( curXPos - 2, curYBot, curXPos + 2, curYBot );
	    }
	}
	// Now is the optimal time to set this - all the repaint-minimization
	// then also reduces the number of calls to setMicroFocusHint().
	setMicroFocusHint( curXPos, curYTop, 1, curYBot-curYTop+1 );
    } else {
	delete d->pm;
	d->pm = 0;
    }

}


/*!\reimp
*/

void QLineEdit::resizeEvent( QResizeEvent * )
{
    delete d->pm;
    d->pm = 0;
    offset = 0;
    updateOffset();
}


/*! \reimp
*/
bool QLineEdit::event( QEvent * e )
{
    if ( e->type() == QEvent::AccelOverride && !d->readonly ) {
	QKeyEvent* ke = (QKeyEvent*) e;
	if ( ke->state() == NoButton ) {
	    if ( ke->key() < Key_Escape ) {
		ke->accept();
	    } else {
		switch ( ke->key() ) {
  		case Key_Delete:
  		case Key_Home:
  		case Key_End:
  		case Key_Backspace:
  		    ke->accept();
 		default:
  		    break;
  		}
	    }
	} else if ( ke->state() & ControlButton ) {
	    switch ( ke->key() ) {
// Those are too frequently used for application functionality
/*	    case Key_A:	
	    case Key_B:
	    case Key_D:
	    case Key_E:
	    case Key_F:
	    case Key_H:
	    case Key_K:
*/
	    case Key_C:
	    case Key_V:
	    case Key_X:	    
	    case Key_Y:
	    case Key_Z:	    
	    case Key_Left:
	    case Key_Right:
#if defined (_WS_WIN_)
	    case Key_Insert:
#endif
		ke->accept();
	    default:
		break;
	    }
	}
    }
    return QWidget::event( e );
}


/*! \reimp
*/
void QLineEdit::mousePressEvent( QMouseEvent *e )
{
    d->dnd_startpos = e->pos();
    d->dnd_primed = FALSE;
#ifndef QT_NO_POPUPMENU
    if ( e->button() == RightButton ) {
	QGuardedPtr<QPopupMenu> popup = new QPopupMenu( this );
	int id[ 7 ];
	id[ IdUndo ] = popup->insertItem( tr( "Undo" ) );
	id[ IdRedo ] = popup->insertItem( tr( "Redo" ) );
	popup->insertSeparator();
#ifndef QT_NO_CLIPBOARD
	id[ IdCut ] = popup->insertItem( tr( "Cut" ) );
	id[ IdCopy ] = popup->insertItem( tr( "Copy" ) );
	id[ IdPaste ] = popup->insertItem( tr( "Paste" ) );
#endif
	id[ IdClear ] = popup->insertItem( tr( "Clear" ) );
	popup->insertSeparator();
	id[ IdSelectAll ] = popup->insertItem( tr( "Select All" ) );
	popup->setItemEnabled( id[ IdUndo ],
				  !this->d->readonly && !this->d->undoList.isEmpty() );
	popup->setItemEnabled( id[ IdRedo ],
				  !this->d->readonly && !this->d->redoList.isEmpty() );
#ifndef QT_NO_CLIPBOARD
	popup->setItemEnabled( id[ IdCut ],
				  !this->d->readonly && !this->d->readonly && hasMarkedText() );
	popup->setItemEnabled( id[ IdCopy ], hasMarkedText() );
	popup->setItemEnabled( id[ IdPaste ],
				  !this->d->readonly
				  && (bool)QApplication::clipboard()->text().length() );
#endif
	popup->setItemEnabled( id[ IdClear ],
				  !this->d->readonly && (bool)text().length() );
	int allSelected = minMark() == 0 && maxMark() == (int)text().length();
	popup->setItemEnabled( id[ IdSelectAll ],
				  (bool)text().length() && !allSelected );

	int r = popup->exec( e->globalPos() );
	delete (QPopupMenu *) popup;

	if ( r == id[ IdUndo ] )
	    undoInternal();
	else if ( r == id[ IdRedo ] )
	    redoInternal();
#ifndef QT_NO_CLIPBOARD
	else if ( r == id[ IdCut ] )
	    cut();
	else if ( r == id[ IdCopy ] )
	    copy();
	else if ( r == id[ IdPaste ] )
	    paste();
#endif
	else if ( r == id[ IdClear ] )
	    clear();
	else if ( r == id[ IdSelectAll ] )
	    selectAll();
	return;
    }
#endif //QT_NO_POPUPMENU
    d->inDoubleClick = FALSE;
    int newCP = xPosToCursorPos( e->pos().x() );
    int m1 = minMark();
    int m2 = maxMark();
#ifndef QT_NO_DRAGANDDROP
    if ( hasMarkedText() && echoMode() == Normal && !( e->state() & ShiftButton ) &&
	 e->button() == LeftButton && m1 < newCP && m2 > newCP ) {
	d->dndTimer.start( QApplication::startDragTime(), TRUE );
	d->dnd_primed = TRUE;
	return;
    }
#endif

    m1 = QMIN( m1, cursorPos );
    m2 = QMAX( m2, cursorPos );
    dragScrolling = FALSE;
    if ( e->state() & ShiftButton ) {
	newMark( newCP, FALSE );
    } else {
	markDrag = newCP;
	markAnchor = newCP;
	newMark( newCP, FALSE );
    }
    repaintArea( m1, m2 );
    d->mousePressed = TRUE;
}

#ifndef QT_NO_DRAGANDDROP

/*
  \internal
*/

void QLineEdit::doDrag()
{
    d->dnd_primed = FALSE;
    QTextDrag *tdo = new QTextDrag( markedText(), this );
    tdo->drag();
}

#endif // QT_NO_DRAGANDDROP

/*!\reimp
*/
void QLineEdit::mouseMoveEvent( QMouseEvent *e )
{
#ifndef QT_NO_DRAGANDDROP
    if ( d->dndTimer.isActive() ) {
	d->dndTimer.stop();
	return;
    }

    if ( d->dnd_primed ) {
	if ( ( d->dnd_startpos - e->pos() ).manhattanLength() > QApplication::startDragDistance() )
	    doDrag();
	return;
    }
#endif

    if ( !(e->state() & LeftButton) )
	return;

    int margin = frame() ? frameW()*2 : 2;

    if ( e->pos().x() < margin || e->pos().x() > width() - margin ) {
	if ( !dragScrolling ) {
	    dragScrolling = TRUE;
	    scrollingLeft = e->pos().x() < margin;
	    if ( scrollingLeft )
		newMark( xPosToCursorPos( 0 ), FALSE );
	    else
		newMark( xPosToCursorPos( width() ), FALSE );
	    d->dragTimer.start( scrollTime );
	}
    } else {
	dragScrolling = FALSE;
	int mousePos = xPosToCursorPos( e->pos().x() );
	int m1 = markDrag;
	newMark( mousePos, FALSE );
	repaintArea( m1, mousePos );
    }
}

/*!\reimp
*/
void QLineEdit::mouseReleaseEvent( QMouseEvent * e )
{
    dragScrolling = FALSE;
    d->dnd_primed = FALSE;
    if ( d->dndTimer.isActive() ) {
	d->dndTimer.stop();
	int ncp = xPosToCursorPos( e->pos().x() );
	setSelection( ncp, 0 );
	setCursorPosition( ncp );
	return;
    }
    if ( d->inDoubleClick ) {
	d->inDoubleClick = FALSE;
	return;
    }

    if ( !d->mousePressed )
	return;
    d->mousePressed = FALSE;

#ifndef QT_NO_CLIPBOARD
#if defined(_WS_X11_)
    copy();
#endif

    if ( !d->readonly && e->button() == MidButton ) {
#if defined(_WS_X11_)
	insert( QApplication::clipboard()->text() );
#else
	if ( style() == MotifStyle )
	    insert( QApplication::clipboard()->text() );
#endif
	return;
    }
#endif

    if ( e->button() != LeftButton )
	return;

    int margin = frame() ? frameW()*2 : 2;
    if ( !QRect( margin, margin,
		 width() - 2*margin,
		 height() - 2*margin ).contains( e->pos() ) )
	return;

    int mousePos = xPosToCursorPos( e->pos().x() );
    int m1 = markDrag;
    newMark( mousePos, FALSE );
    repaintArea( m1, mousePos );
}


/*!\reimp
*/
void QLineEdit::mouseDoubleClickEvent( QMouseEvent * )
{
    d->inDoubleClick = TRUE;
    dragScrolling = FALSE;
    if ( echoMode() == Password )
	selectAll();
    else
	markWord( cursorPos );
}

/*!
  Moves the cursor leftwards one or more characters.
  \sa cursorRight()
*/

void QLineEdit::cursorLeft( bool mark, int steps )
{
    cursorRight( mark, -steps );
}

/*!
  Moves the cursor rightwards one or more characters.
  \sa cursorLeft()
*/

void QLineEdit::cursorRight( bool mark, int steps )
{
    int cp = cursorPos + steps;
    cp = QMAX( cp, 0 );
    cp = QMIN( cp, (int)tbuf.length() );
    if ( cp == cursorPos ) {
	if ( !mark )
	    deselect();
    } else if ( mark ) {
	newMark( cp );
	blinkOn();
    } else {
	setCursorPosition( cp );
	setSelection( cp, 0 );
    }
}

/*!
  Deletes the character to the left of the text cursor and moves the
  cursor one position to the left. If a text has been marked by the user
  (e.g. by clicking and dragging) the cursor will be put at the beginning
  of the marked text and the marked text will be removed.  \sa del()
*/

void QLineEdit::backspace()
{
    if ( hasMarkedText() ) {
	del();
    } else if ( cursorPos > 0 ) {
	if ( d->undo && d->needundo && !d->ignoreUndoWithDel ) {
	    if ( d->undoList.isEmpty() || d->undoList.last().str != tbuf ) {
		d->undoList += QLineEditUndoItem(tbuf, cursorPos );
		d->redoList.clear();
	    }
	}
	cursorLeft( FALSE );
	del();
    }
}

/*!
  Deletes the character on the right side of the text cursor. If a text
  has been marked by the user (e.g. by clicking and dragging) the cursor
  will be put at the beginning of the marked text and the marked text will
  be removed.  \sa backspace()
*/

void QLineEdit::del()
{
    QString test( tbuf);
    d->ignoreUndoWithDel = TRUE;
    if ( d->undo && ( (d->needundo && !d->ignoreUndoWithDel) || hasMarkedText() ) ) {
	if ( d->undoList.isEmpty() || d->undoList.last().str != tbuf ) {
	    d->undoList += QLineEditUndoItem(tbuf, cursorPos );
	    d->redoList.clear();
	}
    }

    if ( hasMarkedText() ) {
	test.remove( minMark(), maxMark() - minMark() );
	validateAndSet( test, minMark(), minMark(), minMark() );
    } else if ( cursorPos != (int)tbuf.length() ) {
	test.remove( cursorPos, 1 );
	validateAndSet( test, cursorPos, 0, 0 );
    }
}

/*!
  Moves the text cursor to the left end of the line. If mark is TRUE text
  will be marked towards the first position, if not any marked text will
  be unmarked if the cursor is moved.  \sa end()
*/

void QLineEdit::home( bool mark )
{
    cursorRight( mark, -cursorPos );
}

/*!
  Moves the text cursor to the right end of the line. If mark is TRUE text
  will be marked towards the last position, if not any marked text will
  be unmarked if the cursor is moved.
  \sa home()
*/

void QLineEdit::end( bool mark )
{
    cursorRight( mark, tbuf.length()-cursorPos );
}


void QLineEdit::newMark( int pos, bool c )
{
    if ( markDrag != pos || cursorPos != pos )
	d->pmDirty = TRUE;
    markDrag = pos;
    setCursorPosition( pos );
#ifndef QT_NO_CLIPBOARD
#if defined(_WS_X11_)
    if ( c )
	copy();
#endif
#endif
}


void QLineEdit::markWord( int pos )
{
    int i = pos - 1;
    while ( i >= 0 && tbuf[i].isPrint() && !tbuf[i].isSpace() )
	i--;
    i++;
    int newAnchor = i;

    i = pos;
    while ( tbuf[i].isPrint() && !tbuf[i].isSpace() )
	i++;
    if ( style() != MotifStyle ) {
	while( tbuf[i].isSpace() )
	    i++;
	setCursorPosition( i );
    }
    int newDrag = i;
    setSelection( newAnchor, newDrag - newAnchor );

#ifndef QT_NO_CLIPBOARD
#if defined(_WS_X11_)
    copy();
#endif
#endif
}

#ifndef QT_NO_CLIPBOARD

/*! Copies the marked text to the clipboard, if there is any and
  if echoMode() is Normal.

  \sa cut() paste()
*/

void QLineEdit::copy() const
{
    QString t = markedText();
    if ( !t.isEmpty() && echoMode() == Normal ) {
	disconnect( QApplication::clipboard(), SIGNAL(dataChanged()), this, 0);
	QApplication::clipboard()->setText( t );
	connect( QApplication::clipboard(), SIGNAL(dataChanged()),
		 this, SLOT(clipboardChanged()) );
    }
}

/*!
  Inserts the clipboard's text at the cursor position, deleting any
  previous marked text.

  If the end result is not acceptable for the current validator,
  nothing happens.

  \sa copy() cut()
*/

void QLineEdit::paste()
{
    insert( QApplication::clipboard()->text() );
}

/*!
  Copies the marked text to the clipboard and deletes it, if there is
  any.

  If the current validator disallows deleting the marked text, cut()
  will copy it but not delete it.

  \sa copy() paste()
*/

void QLineEdit::cut()
{
    QString t = markedText();
    if ( !t.isEmpty() ) {
	copy();
	del();
    }
}

#endif

/*!
  Sets the alignment of the line edit. Possible Values are Qt::AlignLeft,
  Qt::AlignRight and Qt::Align(H)Center - see Qt::AlignmentFlags.
  \sa alignment()
*/
void QLineEdit::setAlignment( int flag ){
    if ( flag == alignmentFlag )
	return;
    if ( flag == Qt::AlignRight ||
	 flag == Qt::AlignCenter ||
	 flag == Qt::AlignHCenter ||
	 flag == Qt::AlignLeft ) {
	alignmentFlag = flag;
	updateOffset();
	update();
    }
}

/*!
  Returns the alignment of the line edit. Possible Values
  are Qt::AlignLeft, Qt::AlignRight and Qt::Align(H)Center.

  \sa setAlignment(), Qt::AlignmentFlags
*/

int QLineEdit::alignment() const
{
    return alignmentFlag;
}

/*!
  This private slot is activated when this line edit owns the clipboard and
  some other widget/application takes over the clipboard. (X11 only)
*/

void QLineEdit::clipboardChanged()
{
#if defined(_WS_X11_)
    disconnect( QApplication::clipboard(), SIGNAL(dataChanged()),
		this, SLOT(clipboardChanged()) );
    deselect();
#endif
}



int QLineEdit::lastCharVisible() const
{
    int tDispWidth = width() - (frameW()*2 + 4);
    return xPosToCursorPos( tDispWidth );
}


int QLineEdit::minMark() const
{
    return markAnchor < markDrag ? markAnchor : markDrag;
}


int QLineEdit::maxMark() const
{
    return markAnchor > markDrag ? markAnchor : markDrag;
}



/*!  Sets the line edit to draw itself inside a frame if \a
  enable is TRUE, and to draw itself without any frame if \a enable is
  FALSE.

  The default is TRUE.

  \sa frame()
*/

void QLineEdit::setFrame( bool enable )
{
    if ( d->frame == enable )
	return;

    d->frame = enable;
    d->pmDirty = TRUE;
    updateOffset();
    update();
}


/*!  Returns TRUE if the line edit draws itself inside a frame, FALSE
  if it draws itself without any frame.

  The default is to use a frame.

  \sa setFrame()
*/

bool QLineEdit::frame() const
{
    return d ? d->frame : TRUE;
}

int QLineEdit::frameW() const
{
    return frame() ? style().defaultFrameWidth() : 0;
}

/*!  Sets the echo mode of the line edit widget.

  The echo modes available are:
  <ul>
  <li> \c Normal - display characters as they are entered.  This is
	the default.
  <li> \c NoEcho - do not display anything. This may be appropriate
	for passwords where even the length of the password should
	be kept secret.
  <li> \c Password - display asterisks instead of the characters
	actually entered.
  </ul>

  The widget's display, and the ability to copy or drag the
  text is affected by this setting.

  \sa echoMode() EchoMode displayText()
*/

void QLineEdit::setEchoMode( EchoMode mode )
{
    if ( d->mode == mode )
	return;

    d->mode = mode;
    d->pmDirty = TRUE;
    updateOffset();
    update();
}


/*!
  Returns the echo mode of the line edit.

  \sa setEchoMode() EchoMode
*/

QLineEdit::EchoMode QLineEdit::echoMode() const
{
    return d->mode;
}

/*!
  Enables or disables read-only mode, where the user can cut-and-paste
  or drag-and-drop the text, but cannot edit it.
  They never see a cursor in this case.

  \sa setEnabled(), isReadOnly()
*/
void QLineEdit::setReadOnly( bool enable )
{
    d->readonly = enable;
}

/*!
  Returns whether the line-edit is read-only.
  \sa setReadOnly()
*/
bool QLineEdit::isReadOnly() const
{
    return d->readonly;
}



/*!
  Returns a recommended size for the widget.

  The width returned is enough for a few characters, typically 15 to 20.
*/
QSize QLineEdit::sizeHint() const
{
    constPolish();
    QFontMetrics fm( font() );
    int h = fm.height();
    int w = fm.width( 'x' ) * 17; // "some"
    if ( frame() ) {
	h += 4 + frameW()*2;
	if ( style() == WindowsStyle && h < 26 )
	    h = 22;
	return QSize( w + 4 + frameW()*2, h ).expandedTo( QApplication::globalStrut() );
    } else {
	return QSize( w + 4, h + 4 ).expandedTo( QApplication::globalStrut() );
    }
}



/*!
  Returns a minimum size for the line edit.

  The width returned is enough for at least one character.
*/

QSize QLineEdit::minimumSizeHint() const
{
    constPolish();
    QFontMetrics fm( font() );
    int h = fm.height();
    int w = fm.maxWidth();
    if ( frame() ) {
	h += 4 + frameW()*2;
	if ( style() == WindowsStyle && h < 26 )
	    h = 22;
	return QSize( w + 4 + frameW()*2, h );
    } else {
	return QSize( w + 4, h + 4 );
    }
}



/*!\reimp
*/
QSizePolicy QLineEdit::sizePolicy() const
{
    //### removeme 3.0
    return QWidget::sizePolicy();
}


/*!
  Sets this line edit to accept input only as accepted by \a v,
  allowing arbitrary constraints on the text which the user can edit.

  If \a v == 0, remove the current input validator.  The default
  is no input validator (ie. any input is accepted up to maxLength()).

  \sa validator() QValidator
*/

void QLineEdit::setValidator( const QValidator * v )
{
    d->validator = v;
}

/*!
  Returns a pointer to the current input validator, or 0 if no
  validator has been set.

  \sa setValidator()
*/

const QValidator * QLineEdit::validator() const
{
    return d ? d->validator : 0;
}


/*!  This slot is equivalent to setValidator( 0 ). */

void QLineEdit::clearValidator()
{
    setValidator( 0 );
}

#ifndef QT_NO_DRAGANDDROP

/*! \reimp
*/
void QLineEdit::dragEnterEvent( QDragEnterEvent *e )
{
    if ( !d->readonly && QTextDrag::canDecode(e) )
	e->accept( rect() );
}


/*!\reimp
*/
void QLineEdit::dropEvent( QDropEvent *e )
{
    QString str;
    QCString plain = "plain";

    // try text/plain
    bool decoded = QTextDrag::decode(e, str, plain);
    // otherwise we'll accept any kind of text (like text/uri-list)
    if (! decoded) decoded = QTextDrag::decode(e, str);

    if ( !d->readonly && decoded) {
	if ( e->source() == this && hasMarkedText() )
	    del();
	if ( !hasMarkedText() )
	    setCursorPosition( xPosToCursorPos(e->pos().x()) );
	insert( str );
	e->accept();
    } else {
	e->ignore();
    }
}

#endif // QT_NO_DRAGANDDROP

/*!  This private slot handles cursor blinking. */

void QLineEdit::blinkSlot()
{
    if ( hasFocus() || cursorOn ) {
	cursorOn = !cursorOn;
	if ( d->pm && !d->pmDirty && d->cursorRepaintRect.isValid() )
	    repaint( d->cursorRepaintRect, FALSE );
	else
	    repaint( FALSE );
    }
    if ( hasFocus() )
	d->blinkTimer.start( QApplication::cursorFlashTime()/2, TRUE );
    else
	d->blinkTimer.stop();
}


/*!  This private slot handles drag-scrolling. */

void QLineEdit::dragScrollSlot()
{
    if ( !hasFocus() || !dragScrolling )
	d->dragTimer.stop();
    else if ( scrollingLeft )
	cursorLeft( TRUE );
    else
	cursorRight( TRUE );
}


/*!  Validates and perhaps sets this line edit to contain \a newText
  with the cursor at position newPos, with marked text from \a
  newMarkAnchor to \a newMarkDrag.  Returns TRUE if it changes the line
  edit and FALSE if it doesn't.

  Linebreaks in \a newText are converted to spaces, and it is
  truncated to maxLength() before testing its validity.

  Repaints and emits textChanged() if appropriate.
*/

bool QLineEdit::validateAndSet( const QString &newText, int newPos,
				int newMarkAnchor, int newMarkDrag )
{
    QString t( newText );
    for ( uint i=0; i<t.length(); i++ ) {
	if ( t[(int)i] < ' ' )  // unprintable/linefeed becomes space
	    t[(int)i] = ' ';
    }
    t.truncate( maxLength() );
#ifndef QT_NO_VALIDATOR
    const QValidator * v = validator();

    if ( v && v->validate( t, newPos ) == QValidator::Invalid &&
	 v->validate( tbuf, cursorPos ) != QValidator::Invalid ) {
	return FALSE;
    }
#endif
    bool tc = ( t != tbuf );

    // okay, it succeeded
    if ( newMarkDrag != markDrag ||
	 newMarkAnchor != markAnchor ||
	 newPos != cursorPos ||
	 tc ) {
	int minP = QMIN( cursorPos, minMark() );
	int maxP = QMAX( cursorPos, maxMark() );

	cursorPos = newPos;
	markAnchor = newMarkAnchor;
	markDrag = newMarkDrag;

	minP = QMIN( minP, QMIN( cursorPos, minMark() ) );
	int i = 0;
	while( i < minP && t[i] == tbuf[i] )
	    i++;
	minP = i;

	maxP = QMAX( maxP, QMAX( cursorPos, maxMark() ) );
	if ( fontMetrics().width( t ) < fontMetrics().width( tbuf ) )
	    maxP = t.length();
	tbuf = t;

	if ( cursorPos < (int)text().length() && maxP < (int)text().length() )
	    maxP = text().length();

	repaintArea( minP, maxP );
    }
    if ( tc ) {
	ed = TRUE;
	emit textChanged( tbuf );
    }
    return TRUE;
}


/*!  Removes any selected text, inserts \a newText,
  validates the result and if it is valid, sets it as the new contents
  of the line edit.

*/

void QLineEdit::insert( const QString &newText )
{
    QString t( newText );
    if ( t.isEmpty() && !hasMarkedText() )
	return;

    for ( int i=0; i<(int)t.length(); i++ )
	if ( t[i] < ' ' )  // unprintable/linefeed becomes space
	    t[i] = ' ';

    QString test( tbuf );
    int cp = cursorPos;
    if ( d->undo && ( d->needundo || hasMarkedText() ) ) {
	if ( d->undoList.isEmpty() || d->undoList.last().str != tbuf ) {
	    d->undoList += QLineEditUndoItem(tbuf, cursorPos );
	    d->redoList.clear();
	    d->needundo = FALSE;
	}
    }
    if ( hasMarkedText() ) {
	test.remove( minMark(), maxMark() - minMark() );
	cp = minMark();
    }
    test.insert( cp, t );
    int ncp = QMIN( cp+t.length(), (uint)maxLength() );
    blinkOn();
    validateAndSet( test, ncp, ncp, ncp );
}


/*!  Repaints all characters from \a from to \a to.  If cursorPos is
  between from and to, ensures that cursorPos is visible.  */

void QLineEdit::repaintArea( int from, int to )
{
    QString buf = displayText();

    int a, b;
    if ( from < to ) {
	a = from;
	b = to;
    } else {
	a = to;
	b = from;
    }

    d->pmDirty = TRUE;
    int old = offset;
    if ( d->offsetDirty || cursorPos >= a && cursorPos <= b )
	updateOffset();
    if ( !d->pmDirty ) {
	return;
    } else if ( old != offset ) {
	repaint( FALSE );
	return;
    }

    QFontMetrics fm = fontMetrics();
    int x = fm.width( buf.left( a ) ) + offset - 2 + frameW();
    QRect r( x, 0, fm.width( buf.mid( a, b-a ) ) + 5, height() );
    r = r.intersect( rect() );
    if ( !r.isValid() )
	return;
    if ( b >= (int)buf.length() )
	r.setRight( width() );
    repaint( r, FALSE );
}


/*!  \reimp */

void QLineEdit::setEnabled( bool e )
{
    d->pmDirty = TRUE;
    QWidget::setEnabled( e );
}


/*! \reimp */

void QLineEdit::setFont( const QFont & f )
{
    d->pmDirty     = TRUE;
    d->offsetDirty = TRUE;
    QWidget::setFont( f );
}


/*!  Syntactic sugar for setText( "" ), provided to match no-argument
  signals.
*/

void QLineEdit::clear()
{
    setText( QString::fromLatin1("") );
}


/*!  Sets the marked area of this line edit to start at \a start and
  be \a length characters long. */

void QLineEdit::setSelection( int start, int length )
{
    int b, e;
    b = QMIN( markAnchor, markDrag );
    e = QMAX( markAnchor, markDrag );
    b = QMIN( b, start );
    e = QMAX( e, start + length );
    markAnchor = start;
    markDrag = start + length;
    repaintArea( b, e );
}


/*!  Sets the cursor position for this line edit to \a newPos and
  repaints accordingly.  \sa cursorPosition() */

void QLineEdit::setCursorPosition( int newPos )
{
    if ( newPos == cursorPos )
	return;
    newPos = QMIN( newPos, (int)tbuf.length() );
    newPos = QMAX( newPos, 0 );
    int b, e;
    b = QMIN( newPos, cursorPos );
    e = QMAX( newPos, cursorPos );
    cursorPos = newPos;
    blinkOn();
    repaintArea( b, e );
}


/*!  Returns the current cursor position for this line edit.  \sa
  setCursorPosition() */

int QLineEdit::cursorPosition() const
{
    return cursorPos;
}


/*! \reimp */

void QLineEdit::setPalette( const QPalette & p )
{
    d->pmDirty = TRUE;
    QWidget::setPalette( p );
}


/*!  Sets the edited flag of this line edit to \a on.  The edited flag
is never read by QLineEdit, and is changed to TRUE whenever the user
changes its contents.

This is useful e.g. for things that need to provide a default value,
but cannot find the default at once.  Just open the line edit without
the best default and when the default is known, check the edited()
return value and set the line edit's contents if the user has not
started editing the line edit.

\sa edited()
*/

void QLineEdit::setEdited( bool on )
{
    ed = on;
}


/*!  Returns the edited flag of the line edit.  If this returns FALSE,
the line edit's contents have not been changed since the construction
of the QLineEdit (or the last call to either setText() or setEdited( FALSE ),
if any).  If it returns true, the contents have been edited, or
setEdited( TRUE ) has been called.

\sa setEdited()
*/

bool QLineEdit::edited() const
{
    return ed;
}

/*!
  Moves the cursor one word to the right.  If \a mark is TRUE, the text
  is marked.
  \sa cursorWordBackward()
*/
void QLineEdit::cursorWordForward( bool mark )
{
    int i = cursorPos;
    while ( i < (int) tbuf.length() && !tbuf[i].isSpace() )
	++i;
    while ( i < (int) tbuf.length() && tbuf[i].isSpace() )
	++i;
    cursorRight( mark, i - cursorPos );
}


/*!
  Moves the cursor one word to the left.  If \a mark is TRUE, the text
  is marked.
  \sa cursorWordForward()
*/
void QLineEdit::cursorWordBackward( bool mark )
{
    int i = cursorPos;
    while ( i > 0 && tbuf[i-1].isSpace() )
	--i;
    while ( i > 0 && !tbuf[i-1].isSpace() )
	--i;
    cursorLeft( mark, cursorPos - i );
}


void QLineEdit::updateOffset()
{ // must not call repaint() - paintEvent() calls this
    if ( !isVisible() ) {
	d->offsetDirty = TRUE;
	return;
    }
    d->offsetDirty = FALSE;
    makePixmap();
    QFontMetrics fm = fontMetrics();
    int textWidth = fm.width( displayText() )+4;
    int w = d->pm->width();
    int old = offset;

    if ( textWidth > w ) {
	// may need to scroll.
	QString dt = displayText();
	dt += QString::fromLatin1( "  " );
	dt = dt.left( cursorPos + 2 );
	if ( cursorPos < 3 )
	    offset = 0;
	else if ( fm.width( dt.left( cursorPos - 2 ) ) + offset < 0 )
	    offset = -fm.width( dt.left( cursorPos - 2 ) );
	else if ( fm.width( dt ) + offset > w )
	    offset = w - fm.width( dt );
    } else {
	if ( textWidth < 5 ) {
	    // nothing is to be drawn.  okay.
	    textWidth = QMIN( 5, w );
	}
	if ( alignmentFlag == Qt::AlignRight ) {
	    // right-aligned text, space for all of it
	    offset = w - textWidth;
	} else if ( alignmentFlag == Qt::AlignCenter || alignmentFlag == Qt::AlignHCenter ) {
	    // center-aligned text, space for all of it
	    offset = (w - textWidth)/2;
	} else {
	    // default: left-aligned, space for all of it
	    offset = 0;
	}
    }

    if ( old == offset && !d->pmDirty )
	return;

    d->pmDirty = TRUE;
}


/*! Returns the index of the character to whose left edge \a goalx is
  closest.
*/

int QLineEdit::xPosToCursorPos( int goalx ) const
{
    int x1, x2;
    x1 = offset;
    int i = 0;
    QFontMetrics fm = fontMetrics();
    QString s = displayText();
    goalx -= (frameW() + 2);

    while( i < (int) s.length() ) {
	x2 = x1 + fm.width( s[i] );
	if ( QABS( x1 - goalx ) < QABS( x2 - goalx ) )
	    return i;
	i++;
	x1 = x2;
    }
    return i;
}


/*!  Starts the thing blinking, or makes sure it's displayed at once. */

void QLineEdit::blinkOn()
{
    if ( !hasFocus() )
	return;

    d->blinkTimer.start( cursorOn?QApplication::cursorFlashTime() / 2 : 0, TRUE );
    blinkSlot();
}


void QLineEdit::makePixmap() const
{
    if ( d->pm )
	return;

    QSize s( width() - frameW()*2, height() - frameW()*2 );
    if ( s.width() < 0 )
	s.setWidth( 0 );
    if ( s.height() < 0 )
	s.setHeight( 0 );
    d->pm = new QPixmap( s );
    d->pmDirty = TRUE;
}


void QLineEdit::undoInternal()
{
    if ( d->undoList.isEmpty() )
	return;
    d->undo = FALSE;

    d->redoList += QLineEditUndoItem(tbuf, cursorPos );
    setText( d->undoList.last().str );
    setCursorPosition( d->undoList.last().pos );
    markAnchor = cursorPos;
    d->undoList.remove( d->undoList.fromLast() );
    if ( d->undoList.count() > 10 )
	d->undoList.remove( d->undoList.begin() );
    d->undo = TRUE;
    d->needundo = TRUE;
}

void QLineEdit::redoInternal()
{
    if ( d->redoList.isEmpty() )
	return;
    d->undo = FALSE;
    d->undoList += QLineEditUndoItem(tbuf, cursorPos );
    setText( d->redoList.last().str );
    setCursorPosition( d->redoList.last().pos );
    markAnchor = cursorPos;
    d->redoList.remove( d->redoList.fromLast() );
    d->undo = TRUE;
    d->needundo = TRUE;
}
#endif


#if defined(Q_INCOMPATIBLE_3_0_ADDONS)
bool QLineEdit::getSelection( int *start, int *end )
{
    if( !hasMarkedText() )
	return false;

    *start = minMark();
    *end = maxMark();
    return true;
}

void QLineEdit::setPasswordChar( QChar c )
{
    d->passwordChar = c;
}

QChar QLineEdit::passwordChar() const
{
    return d->passwordChar;
}

#endif
