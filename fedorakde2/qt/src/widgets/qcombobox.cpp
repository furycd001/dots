/**********************************************************************
** $Id: qt/src/widgets/qcombobox.cpp   2.3.2   edited 2001-10-21 $
**
** Implementation of QComboBox widget class
**
** Created : 940426
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

#include "qcombobox.h"
#ifndef QT_NO_COMBOBOX
#include "qpopupmenu.h"
#include "qlistbox.h"
#include "qpainter.h"
#include "qdrawutil.h"
#include "qstrlist.h"
#include "qpixmap.h"
#include "qtimer.h"
#include "qapplication.h"
#include "qlineedit.h"
#include "qbitmap.h"
#include "qeffects_p.h"
#include <limits.h>

// NOT REVISED
/*!
  \class QComboBox qcombobox.h
  \brief The QComboBox widget is a combined button and popup list.

  \ingroup basic

  A combo box may be defined as a selection widget which displays the
  current selection, and which can pop up a list of possible
  selections.  Some combo boxes also allow the user to select
  arbitrary strings, using a line editor.

  Since combo boxes occupy little screen space and always display the
  current selection, they are very well suited to displaying and
  selecting modes (such as font family and size): The user can always
  see what mode he/she is in, and the majority of the screen space is
  available for real work.

  QComboBox supports three different appearances: Motif 1.x, Motif 2.0
  and Windows 95.  In Motif 1.x, a combo box was called XmOptionMenu.
  In Motif 2.0, OSF introduced an improved combo box and
  named that XmComboBox.  QComboBox provides both.

  QComboBox provides two different constructors.  The simplest one
  creates an old-style combo box in Motif style:
  \code
      QComboBox * c = new QComboBox( this, "read-only combo" );
  \endcode

  The other one creates a new-style combo box in Motif style, and can
  create both read-only and read-write combo boxes:
  \code
      QComboBox * c1 = new QComboBox( FALSE, this, "read-only combo" );
      QComboBox * c2 = new QComboBox( TRUE, this, "read-write combo" );
  \endcode

  New-style combo boxes use a list box in both Motif and Windows
  styles, and both the content size and the on-screen size of the list
  box can be limited.  Old-style combo boxes use a popup in Motif
  style, and that popup will happily grow larger than the desktop if
  you put enough data in it.

  The two constructors create identical-looking combos in Windows
  style.

  Combo boxes can contain pixmaps as well as texts; the
  insert() and changeItem() functions are suitably overloaded.  For
  read-write combo boxes, the function clearEdit()
  is provided, to clear the displayed string without changing the
  combo box' contents.

  A combo box emits two signals, activated() and highlighted(), when a
  new item has been activated (selected) or highlighted (set to
  current).  Both signals exist in two versions, one with a \c char*
  argument and one with an \c int argument.  If the user highlights or
  activates a pixmap, only the \c int signals are emitted.

  When the user enters a new string in a read-write combo, the widget
  may or may not insert it, and it can insert it in several locations.
  The default policy is is \c AtBottom, you can change it using
  setInsertionPolicy().

  It is possible to constrain the input to an editable combo box using
  QValidator; see setValidator().  By default, all input is accepted.

  If the combo box is not editable then it has a default focusPolicy()
  of \c TabFocus, i.e. it will not grab focus if clicked.  This
  differs from both Windows and Motif.  If the combo box is editable then it
  has a default focusPolicy() of \c StrongFocus, i.e. it will grab focus if
  clicked.

  <img src="qcombo1-m.png">(Motif 1, read-only)<br clear=all>
  <img src="qcombo2-m.png">(Motif 2, read-write)<br clear=all>
  <img src="qcombo3-m.png">(Motif 2, read-only)<br clear=all>
  <img src="qcombo1-w.png">(Windows style)

  \sa QLineEdit QListBox QSpinBox QRadioButton QButtonGroup
  <a href="guibooks.html#fowler">GUI Design Handbook: Combo Box,</a>
  <a href="guibooks.html#fowler">GUI Design Handbook: Drop-Down List Box.</a>
*/


/*! \enum QComboBox::Policy

  This enum type specifies what QComboBox should do with a new string
  entered by the user.  The following policies are defined: <ul>

  <li> \c NoInsertion means not to insert the string in the combo.

  <li> \c AtTop means to insert the string at the top of the combo box.

  <li> \c AtCurrent means to replace the previously selected item with
  the typed string.

  <li> \c AtBottom means to insert the string at the bottom of the
  combo box.

  <li> \c AfterCurrent means to to insert the string just after the
  previously selected item.

  <li> \c BeforeCurrent means to to insert the string just before the
  previously selected item.

  </ul>

  activated() is always emitted, of course.

  If inserting the new string would cause the combo box to breach
  its content size limit, the item at the other end of the list is
  deleted.  The definition of "other end" is implementation-dependent.
*/


/*! \fn void QComboBox::activated( int index )

  This signal is emitted when a new item has been activated (selected).
  The \e index is the position of the item in the popup list.
*/

/*! \fn void QComboBox::activated( const QString &string )

  This signal is emitted when a new item has been activated
  (selected). \a string is the activated string.

  You can also use activated(int) signal, but be aware that its
  argument is meaningful only for selected strings, not for typed
  strings.
*/

/*! \fn void QComboBox::highlighted( int index )

  This signal is emitted when a new item has been set to current.
  The \e index is the position of the item in the popup list.
*/

/*! \fn void QComboBox::highlighted( const QString &string )

  This signal is emitted when a new item has been highlighted. \a
  string is the highlighted string.

  You can also use highlighted(int) signal.
*/

/*! \fn void QComboBox::textChanged( const QString &string )

  This signal is useful for editable comboboxes. It is emitted whenever
  the contents of the text entry field changes.
*/


class QComboBoxPopup : public QPopupMenu
{
public:
    int itemHeight( int index )
    {
	return QPopupMenu::itemHeight( index );
    }

};


struct QComboData
{
    QComboData( QComboBox *cb ): usingLBox( FALSE ), pop( 0 ), lBox( 0 ), combo( cb )
    {
	duplicatesEnabled = TRUE;
	cb->setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Fixed ) );
    }
    ~QComboData()
    {
	delete pop;
    }

    bool usingListBox()  { return usingLBox; }
    QListBox * listBox() { ASSERT(usingLBox); return lBox; }
    QComboBoxPopup * popup() { ASSERT(!usingLBox); return pop; }
    void updateLinedGeometry();

    void setListBox( QListBox *l ) { lBox = l ; usingLBox = TRUE;
    				l->setMouseTracking( TRUE );}

    void setPopupMenu( QComboBoxPopup * pm ) { pop = pm; usingLBox = FALSE; }

    int		current;
    int		maxCount;
    int		sizeLimit;
    QComboBox::Policy p;
    bool	autoresize;
    bool	poppedUp;
    bool	mouseWasInsidePopup;
    bool	arrowPressed;
    bool	arrowDown;
    bool	discardNextMousePress;
    bool	shortClick;
    bool	useCompletion;
    bool	completeNow;
    int		completeAt;
    bool duplicatesEnabled;
    int fullHeight, currHeight;

    QLineEdit * ed;  // /bin/ed rules!

    QSize sizeHint;

private:
    bool	usingLBox;
    QComboBoxPopup *pop;
    QListBox   *lBox;
    QComboBox *combo;

};

void QComboData::updateLinedGeometry()
{
    if ( !ed || !combo )
	return;
    if ( current == 0 && combo->count() == 0 ) {
	ed->setGeometry( combo->style().comboButtonRect( 0, 0, combo->width(), combo->height() ) );
	return;
    }

    const QPixmap *pix = current < combo->count() ? combo->pixmap( current ) : 0;
    QRect r( combo->style().comboButtonRect( 0, 0, combo->width(), combo->height() ) );
    if ( pix && pix->width() < r.width() )
	r.setLeft( r.left() + pix->width() + 4 );
    if ( r != ed->geometry() )
	ed->setGeometry( r );
}

bool QComboBox::getMetrics( int *dist, int *buttonW, int *buttonH ) const
{
    if ( d->usingListBox() && style() == WindowsStyle ) {
	QRect r  = arrowRect();
	*buttonW = r.width();
	*buttonH = r.height();
	*dist    = 4;
    } else if ( d->usingListBox() ) {
	*dist = 6;
	*buttonW = 16;
	*buttonH = 18;
    } else {
	*dist     = 8;
	*buttonH  = 7;
	*buttonW  = 11;
    }
    return TRUE;
}


static inline bool checkInsertIndex( const char *method, const char * name,
				     int count, int *index)
{
    bool range_err = (*index > count);
#if defined(CHECK_RANGE)
    if ( range_err )
	qWarning( "QComboBox::%s: (%s) Index %d out of range",
		 method, name ? name : "<no name>", *index );
#else
    Q_UNUSED( method )
    Q_UNUSED( name )
#endif
    if ( *index < 0 )				// append
	*index = count;
    return !range_err;
}


static inline bool checkIndex( const char *method, const char * name,
			       int count, int index )
{
    bool range_err = (index >= count);
#if defined(CHECK_RANGE)
    if ( range_err )
	qWarning( "QComboBox::%s: (%s) Index %i out of range",
		 method, name ? name : "<no name>", index );
#else
    Q_UNUSED( method )
    Q_UNUSED( name )
#endif
    return !range_err;
}



/*!
  Constructs a combo box widget with a parent and a name.

  This constructor creates a popup menu if the program uses Motif look
  and feel; this is compatible with Motif 1.x.
*/

QComboBox::QComboBox( QWidget *parent, const char *name )
    : QWidget( parent, name, WResizeNoErase )
{
    d = new QComboData( this );
    if ( style() == WindowsStyle ) {
	setUpListBox();
    } else {
	d->setPopupMenu( new QComboBoxPopup );
	d->popup()->setFont( font() );
	connect( d->popup(), SIGNAL(activated(int)),
			     SLOT(internalActivate(int)) );
	connect( d->popup(), SIGNAL(highlighted(int)),
			     SLOT(internalHighlight(int)) );
    }
    d->ed                    = 0;
    d->current               = 0;
    d->maxCount              = INT_MAX;
    d->sizeLimit	     = 10;
    d->p                    =  AtBottom;
    d->autoresize            = FALSE;
    d->poppedUp              = FALSE;
    d->arrowDown             = FALSE;
    d->discardNextMousePress = FALSE;
    d->shortClick            = FALSE;
    d->useCompletion = FALSE;

    setFocusPolicy( TabFocus );
    setPalettePropagation( AllChildren );
    setFontPropagation( AllChildren );
}


/*!
  Constructs a combo box with a maximum size and either Motif 2.0 or
  Windows look and feel.

  The input field can be edited if \a rw is TRUE, otherwise the user
  may only choose one of the items in the combo box.
*/


QComboBox::QComboBox( bool rw, QWidget *parent, const char *name )
    : QWidget( parent, name, WResizeNoErase )
{
    d = new QComboData( this );
    setUpListBox();

    d->current = 0;
    d->maxCount = INT_MAX;
    setSizeLimit(10);
    d->p = AtBottom;
    d->autoresize = FALSE;
    d->poppedUp = FALSE;
    d->arrowDown = FALSE;
    d->discardNextMousePress = FALSE;
    d->shortClick = FALSE;
    d->useCompletion = FALSE;

    setFocusPolicy( StrongFocus );

    d->ed = 0;
    if ( rw )
	setUpLineEdit();
    setBackgroundMode( PaletteButton );
    setPalettePropagation( AllChildren );
    setFontPropagation( AllChildren );
}



/*!
  Destructs the combo box.
*/

QComboBox::~QComboBox()
{
    delete d;
}


/*!
  If the combobox is editable and the user enters some text in
  the lineedit of the combobox and presses return (and the insertionPolicy()
  is different from \c NoInsertion), the entered text is inserted into the
  list of this combobox. Now, if you set \a enable to TRUE here,
  this new text is always inserted, else it's only inserted if it
  doesn't already exist in the list. If you set \a enable to FALSE
  and the text exists already in the list, the item which contains
  the same text like which should be inserted, this item
  gets the new current item.

  This setting only applies when the user want's to insert a text
  with pressing the return key. It does \e not affect methods like
  insertItem() and similar.
*/

void QComboBox::setDuplicatesEnabled( bool enable )
{
   d->duplicatesEnabled = enable;
}

/*!
  Returns TRUE if the same text can be inserted multiple times
  into the list of the combobox, else FALSE.

  \sa setDuplicatesEnabled();
*/

bool QComboBox::duplicatesEnabled() const
{
    return d->duplicatesEnabled;
}


/*!
  Returns the number of items in the combo box.
*/

int QComboBox::count() const
{
    if ( d->usingListBox() )
	return d->listBox()->count();
    else
	return d->popup()->count();
}


/*!
  \overload
*/

void QComboBox::insertStrList( const QStrList &list, int index )
{
    insertStrList( &list, index );
}

/*!
  Inserts the list of strings at the index \e index in the combo box.

  This is only for compatibility, as it does not support Unicode
  strings.  See insertStringList().
*/

void QComboBox::insertStrList( const QStrList *list, int index )
{
    if ( !list ) {
#if defined(CHECK_NULL)
	ASSERT( list != 0 );
#endif
	return;
    }
    QStrListIterator it( *list );
    const char* tmp;
    if ( index < 0 )
	index = count();
    while ( (tmp=it.current()) ) {
	++it;
	if ( d->usingListBox() )
	    d->listBox()->insertItem( QString::fromLatin1(tmp), index );
	else
	    d->popup()->insertItem( QString::fromLatin1(tmp), index, index );
	if ( index++ == d->current && d->current < count() ) {
	    if ( d->ed ) {
		d->ed->setText( text( d->current ) );
		d->updateLinedGeometry();
	    } else
		update();
	    currentChanged();
	}
    }
    if ( index != count() )
	reIndex();
}

/*!
  Inserts the list of strings at the index \e index in the combo box.
*/

void QComboBox::insertStringList( const QStringList &list, int index )
{
    QStringList::ConstIterator it = list.begin();
    if ( index < 0 )
	index = count();
    while ( it != list.end() ) {
	if ( d->usingListBox() )
	    d->listBox()->insertItem( *it, index );
	else
	    d->popup()->insertItem( *it, index, index );
	if ( index++ == d->current && d->current < count() ) {
	    if ( d->ed ) {
		d->ed->setText( text( d->current ) );
		d->updateLinedGeometry();
	    } else
		update();
	    currentChanged();
	}
	++it;
    }
    if ( index != count() )
	reIndex();
}

/*!
  Inserts the array of ASCII strings at the index \e index in the combo box.

  The \e numStrings argument is the number of strings.
  If \e numStrings is -1 (default), the \e strs array must be
  terminated with 0.

  Example:
  \code
    static const char* items[] = { "red", "green", "blue", 0 };
    combo->insertStrList( items );
  \endcode
*/

void QComboBox::insertStrList( const char **strings, int numStrings, int index)
{
    if ( !strings ) {
#if defined(CHECK_NULL)
	ASSERT( strings != 0 );
#endif
	return;
    }
    if ( index < 0 )
	index = count();
    int i = 0;
    while ( (numStrings<0 && strings[i]!=0) || i<numStrings ) {
	if ( d->usingListBox() )
	    d->listBox()->insertItem( QString::fromLatin1(strings[i]), index );
	else
	    d->popup()->insertItem( QString::fromLatin1(strings[i]), index, index );
	i++;
	if ( index++ == d->current && d->current < count()  ) {
	    if ( d->ed ) {
		d->ed->setText( text( d->current ) );
		d->updateLinedGeometry();
	    } else
		update();
	    currentChanged();
	}
    }
    if ( index != count() )
	reIndex();
}


/*!
  Inserts a text item at position \e index. The item will be appended if
  \e index is negative.
*/

void QComboBox::insertItem( const QString &t, int index )
{
    int cnt = count();
    if ( !checkInsertIndex( "insertItem", name(), cnt, &index ) )
	return;
    if ( d->usingListBox() )
	d->listBox()->insertItem( t, index );
    else
	d->popup()->insertItem( t, index, index );
    if ( index != cnt )
	reIndex();
    if ( index == d->current && d->current < count()  ) {
	if ( d->ed ) {
	    d->ed->setText( text( d->current ) );
	    d->updateLinedGeometry();
	} else
	    update();
    }
    if ( index == d->current )
	currentChanged();
}

/*!
  Inserts a pixmap item at position \e index. The item will be appended if
  \e index is negative.
*/

void QComboBox::insertItem( const QPixmap &pixmap, int index )
{
    int cnt = count();
    if ( !checkInsertIndex( "insertItem", name(), cnt, &index ) )
	return;
    if ( d->usingListBox() )
	d->listBox()->insertItem( pixmap, index );
    else
	d->popup()->insertItem( pixmap, index, index );
    if ( index != cnt )
	reIndex();
    if ( index == d->current && d->current < count()  ) {
	if ( d->ed ) {
	    d->ed->setText( text( d->current ) );
	    d->updateLinedGeometry();
	} else
	    update();
    }
    if ( index == d->current )
	currentChanged();
}

/*!
  Inserts a pixmap item with additional text \a text at position \e
  index. The item will be appended if \e index is negative.
*/

void QComboBox::insertItem( const QPixmap &pixmap, const QString& text, int index )
{
    int cnt = count();
    if ( !checkInsertIndex( "insertItem", name(), cnt, &index ) )
	return;
    if ( d->usingListBox() )
	d->listBox()->insertItem( pixmap, text, index );
    else
	d->popup()->insertItem( pixmap, text, index, index );
    if ( index != cnt )
	reIndex();
    if ( index == d->current && d->current < count()  ) {
	if ( d->ed ) {
	    d->ed->setText( this->text( d->current ) );
	    d->updateLinedGeometry();
	} else
	    update();
    }
    if ( index == d->current )
	currentChanged();
}


/*!
  Removes the item at position \e index.
*/

void QComboBox::removeItem( int index )
{
    int cnt = count();
    if ( !checkIndex( "removeItem", name(), cnt, index ) )
	return;
    if ( d->usingListBox() )
	d->listBox()->removeItem( index );
    else
	d->popup()->removeItemAt( index );
    if ( index != cnt-1 )
	reIndex();
    if ( index == d->current ) {
	if ( d->ed ) {
	    QString s = QString::fromLatin1("");
	    if (d->current < cnt - 1)
		s = text( d->current );
	    d->ed->setText( s );
	    d->updateLinedGeometry();
	}
	else {
	    if ( d->usingListBox() )
		d->current = d->listBox()->currentItem();
	    else {
		if (d->current > count()-1 && d->current > 0)
		    d->current--;
	    }
	    update();
	}
	currentChanged();
    }
    else {
        if ( !d->ed ) {
            if (d->current < cnt - 1)
                setCurrentItem( d->current );
            else
                setCurrentItem( d->current - 1 );
        }
    }

}


/*!
  Removes all combo box items.
*/

void QComboBox::clear()
{
    if ( d->usingListBox() ) {
	d->listBox()->resize( 0, 0 );
	d->listBox()->clear();
    } else {
	d->popup()->clear();
    }

    d->current = 0;
    if ( d->ed ) {
	d->ed->setText( QString::fromLatin1("") );
	d->updateLinedGeometry();
    }
    currentChanged();
}


/*!
  Returns the text item being edited, or the current text item if the combo
  box is not editable.
  \sa text()
*/

QString QComboBox::currentText() const
{
    if ( d->ed )
	return d->ed->text();
    else if ( d->current < count() )
	return text( currentItem() );
    else
	return QString::null;
}


/*!
  Returns the text item at a given index, or
  \link QString::operator!() null string\endlink
  if the item is not a string.
  \sa currentText()
*/

QString QComboBox::text( int index ) const
{
    if ( !checkIndex( "text", name(), count(), index ) )
	return QString::null;
    if ( d->usingListBox() )
	return d->listBox()->text( index );
    else
	return d->popup()->text( index );
}

/*!
  Returns the pixmap item at a given index, or 0 if the item is not a pixmap.
*/

const QPixmap *QComboBox::pixmap( int index ) const
{
    if ( !checkIndex( "pixmap", name(), count(), index ) )
	return 0;
    if ( d->usingListBox() )
	return d->listBox()->pixmap( index );
    else
	return d->popup()->pixmap( index );
}

/*!
  Replaces the item at position \e index with a text.
*/

void QComboBox::changeItem( const QString &t, int index )
{
    if ( !checkIndex( "changeItem", name(), count(), index ) )
	return;
    if ( d->usingListBox() )
	d->listBox()->changeItem( t, index );
    else
	d->popup()->changeItem( t, index );
    if ( index == d->current ) {
	if ( d->ed ) {
	    d->ed->setText( text( d->current ) );
	    d->updateLinedGeometry();
	} else
	    update();
    }
}

/*!
  Replaces the item at position \e index with a pixmap, unless the
  combo box is writable.

  \sa insertItem()
*/

void QComboBox::changeItem( const QPixmap &im, int index )
{
    if ( !checkIndex( "changeItem", name(), count(), index ) )
	return;
    if ( d->usingListBox() )
	d->listBox()->changeItem( im, index );
    else
	d->popup()->changeItem( im, index );
    if ( index == d->current )
	update();
}

/*!
  Replaces the item at position \e index with a pixmap plus text.

  \sa insertItem()
*/

void QComboBox::changeItem( const QPixmap &im, const QString &t, int index )
{
    if ( !checkIndex( "changeItem", name(), count(), index ) )
	return;
    if ( d->usingListBox() )
	d->listBox()->changeItem( im, t, index );
    else
	d->popup()->changeItem( im, t, index );
    if ( index == d->current )
	update();
}


/*!
  Returns the index of the current combo box item.
  \sa setCurrentItem()
*/

int QComboBox::currentItem() const
{
    return d->current;
}

/*!
  Sets the current combo box item.
  This is the item to be displayed on the combo box button.
  \sa currentItem()
*/

void QComboBox::setCurrentItem( int index )
{
    if ( index == d->current && !d->ed ) {
	    return;
    }
    if ( !checkIndex( "setCurrentItem", name(), count(), index ) ) {
	return;
    }
    d->current = index;
    if ( d->ed ) {
	d->ed->setText( text( index ) );
	d->updateLinedGeometry();
    }
    if ( d->poppedUp && d->usingListBox() && d->listBox() )
	d->listBox()->setCurrentItem( index );
    else
	internalHighlight( index );

    currentChanged();
}


/*! \obsolete
  Returns TRUE if auto-resizing is enabled, or FALSE if auto-resizing is
  disabled.

  Auto-resizing is disabled by default.

  \sa setAutoResize()
*/

bool QComboBox::autoResize() const
{
    return d->autoresize;
}

/*! \obsolete
  Enables auto-resizing if \e enable is TRUE, or disables it if \e enable is
  FALSE.

  When auto-resizing is enabled, the combo box button will resize itself
  whenever the current combo box item change.

  \sa autoResize(), adjustSize()
*/

void QComboBox::setAutoResize( bool enable )
{
    if ( (bool)d->autoresize != enable ) {
	d->autoresize = enable;
	if ( enable )
	    adjustSize();
    }
}


/*!\reimp
*/
QSize QComboBox::sizeHint() const
{
    if ( isVisibleTo(0) && d->sizeHint.isValid() )
	return d->sizeHint;

    constPolish();
    int i, w, h;
    QFontMetrics fm = fontMetrics();

    int maxW = count() ? 18 : 7 * fm.width(QChar('x')) + 18;
    int maxH = QMAX( fm.lineSpacing() + 2, 12 );

    for( i = 0; i < count(); i++ ) {
	if ( d->usingListBox() ) {
	    w = d->listBox()->item( i )->width( d->listBox() );
	    h = d->listBox()->item( i )->height( d->listBox() );
	}
	else {
	    h = d->popup()->itemHeight( i );
	    w = d->popup()->sizeHint().width() - 2* d->popup()->frameWidth();
	}
	if ( w > maxW )
	    maxW = w;
	if ( h > maxH )
	    maxH = h;
    }
    if ( maxH <= 20 && style() == WindowsStyle || parentWidget() &&
	 ( parentWidget()->inherits( "QToolBar" ) ||
	   parentWidget()->inherits( "QDialog" ) && d->ed ) )
	maxH = 12;

    int sw, sh;
    if ( d->usingListBox() ) {
	sw = 4 + 4 + maxW;
	sh = 5 + 5 + maxH;
	QRect cr = style().comboButtonRect( 0, 0, sw, sh );
	sw += sw - cr.width();
    } else {
	//hardcoded values for motif 1.x style
	int extraW = 20+5;
	sw = 4 + 4 + maxW + extraW;
	sh = 5 + 5 + maxH;
    }

    d->sizeHint = QSize( sw, sh ).expandedTo( QApplication::globalStrut() );
    return d->sizeHint;
}


/*!\reimp
*/
QSizePolicy QComboBox::sizePolicy() const
{
    //### removeme 3.0
    return QWidget::sizePolicy();
}


/*!
  \internal
  Receives activated signals from an internal popup list and emits
  the activated() signal.
*/

void QComboBox::internalActivate( int index )
{
    if ( d->current != index ) {
	d->current = index;
	currentChanged();
    }
    if ( d->usingListBox() )
	popDownListBox();
    else
	d->popup()->removeEventFilter( this );
    d->poppedUp = FALSE;

    QString t( text( index ) );
    if ( d->ed ) {
	d->ed->setText( t );
	d->updateLinedGeometry();
    }
    emit activated( index );
    emit activated( t );
}

/*!
  \internal
  Receives highlighted signals from an internal popup list and emits
  the highlighted() signal.
*/

void QComboBox::internalHighlight( int index )
{
    emit highlighted( index );
    QString t = text( index );
    if ( !t.isNull() )
	emit highlighted( t );
}

/*!
  \internal
  Receives timeouts after a click. Used to decide if a Motif style
  popup should stay up or not after a click.
*/
void QComboBox::internalClickTimeout()
{
    d->shortClick = FALSE;
}


/*!
  Reimplements QWidget::setBackgroundColor().

  Sets the background color for both the combo box button and the
  combo box popup list.
*/

void QComboBox::setBackgroundColor( const QColor &color )
{
    QWidget::setBackgroundColor( color );
    if ( !d->usingListBox() )
	d->popup()->setBackgroundColor( color );
}

/*!
  Reimplements QWidget::setPalette().

  Sets the palette for both the combo box button and the
  combo box popup list.
*/

void QComboBox::setPalette( const QPalette &palette )
{
    QWidget::setPalette( palette );
    if ( d->usingListBox() )
	d->listBox()->setPalette( palette );
    else
	d->popup()->setPalette( palette );
}

/*!
  Reimplements QWidget::setFont().

  Sets the font for both the combo box button and the
  combo box popup list.
*/

void QComboBox::setFont( const QFont &font )
{
    d->sizeHint = QSize();  // Invalidate Size Hint
    QWidget::setFont( font );
    if ( d->usingListBox() )
	d->listBox()->setFont( font );
    else
	d->popup()->setFont( font );
    if (d->ed)
	d->ed->setFont( font );
    if ( d->autoresize )
	adjustSize();
}


/*!\reimp
*/

void QComboBox::resizeEvent( QResizeEvent * e )
{
    if ( d->ed ) {
	d->updateLinedGeometry();
    }
    QWidget::resizeEvent( e );
}

/*!\reimp
*/

void QComboBox::paintEvent( QPaintEvent * )
{
    QPainter p( this );
    const QColorGroup & g = colorGroup();

    if ( width() < 5 || height() < 5 ) {
	qDrawShadePanel( &p, rect(), g, FALSE, 2,
			 &g.brush( QColorGroup::Button ) );
	return;
    }

    if ( !d->usingListBox() ) {			// motif 1.x style
	int dist, buttonH, buttonW;
	getMetrics( &dist, &buttonW, &buttonH );
	int xPos = width() - dist - buttonW - 1;
	qDrawShadePanel( &p, rect(), g, FALSE, style().defaultFrameWidth(),
			 &g.brush( QColorGroup::Button ) );
	qDrawShadePanel( &p, xPos, (height() - buttonH)/2,
			 buttonW, buttonH, g, FALSE, style().defaultFrameWidth() );
	QRect clip( 4, 2, xPos - 2 - 4 - 5, height() - 4 );
	QString str = d->popup()->text( this->d->current );
	if ( !str.isNull() ) {
	    p.drawText( clip, AlignCenter | SingleLine, str );
	} else {
	    QPixmap *pix = d->popup()->pixmap( this->d->current );
	    if ( pix ) {
		p.setClipRect( clip );
		p.drawPixmap( 4, (height()-pix->height())/2, *pix );
		p.setClipping( FALSE );
	    }
	}

	if ( hasFocus() )
	    p.drawRect( xPos - 5, 4, width() - xPos + 1 , height() - 8 );

    } else if ( style() == MotifStyle ) {	// motif 2.0 style
	style().drawComboButton( &p, 0, 0, width(), height(),
				 g, d->arrowDown, d->ed != 0 );
       	if ( hasFocus() ) {
	    style().drawFocusRect(&p, style().
				  comboButtonFocusRect(0,0,width(),height()),
				  g, &g.button());
	}
	if ( !d->ed ) {
	    QRect clip = style().comboButtonRect( 0, 0, width(), height() );
	    p.setPen( g.foreground() );
	    p.setClipRect( clip );
	    p.setPen( g.foreground() );
	    QListBoxItem * item = d->listBox()->item( d->current );
	    if ( item ) {
		int itemh = item->height( d->listBox() );
		p.translate( clip.x(), clip.y() + (clip.height() - itemh)/2  );
		item->paint( &p );
	    }
	} else if ( d->listBox() && d->listBox()->item( d->current ) ) {
	    QRect r( style().comboButtonRect( 0, 0, width(), height() ) );
	    QListBoxItem * item = d->listBox()->item( d->current );
	    const QPixmap *pix = item->pixmap();
	    if ( pix ) {
		p.fillRect( r.x(), r.y(), pix->width() + 4, r.height(), colorGroup().brush( QColorGroup::Base ) );
		p.drawPixmap( r.x() + 2, r.y() + ( r.height() - pix->height() ) / 2, *pix );
	    }
	}
	p.setClipping( FALSE );
    } else {					// windows 95 style
	style().drawComboButton(&p, 0, 0, width(), height(), g, d->arrowDown, d->ed != 0, isEnabled() );

	QRect tmpR = style().comboButtonRect(0,0,width(),height());
	QRect textR(tmpR.x()+1, tmpR.y()+1, tmpR.width()-2, tmpR.height()-2);

	if ( hasFocus()) {
	    if (!d->ed) {
		p.fillRect( textR.x(), textR.y(),
			    textR.width(), textR.height(),
			    g.brush( QColorGroup::Highlight ) );

	    }
	    style().drawFocusRect(&p, style().comboButtonFocusRect(0,0,width(),height()), g, &g.highlight());
	}

	textR.setRect(tmpR.x()+2, tmpR.y()+1, tmpR.width()-4, tmpR.height()-2);
	p.setClipRect( textR );

	if ( hasFocus() ) {
	    p.setPen( g.highlightedText() );
	    p.setBackgroundColor( g.highlight() );
	} else {
	    p.setPen( g.text() );
	    p.setBackgroundColor( g.background() );
	}
	if ( !d->ed ) {
	    QListBoxItem * item = d->listBox()->item( d->current );
	    if ( item ) {
		int itemh = item->height( d->listBox() );
		p.translate( textR.x(), textR.y() + (textR.height() - itemh)/2  );
		item->paint( &p );
	    }
	} else if ( d->listBox() && d->listBox()->item( d->current ) ) {
	    p.setClipping( FALSE );
	    QRect r( style().comboButtonRect( 0, 0, width(), height() ) );
	    QListBoxItem * item = d->listBox()->item( d->current );
	    const QPixmap *pix = item->pixmap();
	    if ( pix ) {
		p.fillRect( r.x(), r.y(), pix->width() + 4, r.height(), colorGroup().brush( QColorGroup::Base ) );
		p.drawPixmap( r.x() + 2, r.y() + ( r.height() - pix->height() ) / 2, *pix );
	    }
	}
	p.setClipping( FALSE );
    }
}


/*!
  \internal
  Returns the button arrow rectangle for windows style combo boxes.
*/
QRect QComboBox::arrowRect() const
{
    return QRect( width() - 2 - 16, 2, 16, height() - 4 );
}


/*!\reimp
*/

void QComboBox::mousePressEvent( QMouseEvent *e )
{
    if ( e->button() != LeftButton )
	return;
    if ( d->discardNextMousePress ) {
	d->discardNextMousePress = FALSE;
	return;
    }
    if ( count() ) {
	d->arrowPressed = FALSE;
	if ( style() == WindowsStyle ) {
	    popup();
	    if ( arrowRect().contains( e->pos() ) ) {
		d->arrowPressed = TRUE;
		d->arrowDown    = TRUE;
		repaint( FALSE );
	    }
	} else if ( d->usingListBox() ) {
	    popup();
	    QTimer::singleShot( 200, this, SLOT(internalClickTimeout()));
	    d->shortClick = TRUE;
	} else {
	    popup();
	    QTimer::singleShot( 200, this, SLOT(internalClickTimeout()));
	    d->shortClick = TRUE;
	}
    }
}

/*!\reimp
*/

void QComboBox::mouseMoveEvent( QMouseEvent * )
{
}

/*!\reimp
*/

void QComboBox::mouseReleaseEvent( QMouseEvent * )
{
}

/*!\reimp
*/

void QComboBox::mouseDoubleClickEvent( QMouseEvent *e )
{
    mousePressEvent( e );
}


/*!\reimp
*/

void QComboBox::keyPressEvent( QKeyEvent *e )
{
    int c;
    if ( ( e->key() == Key_F4 && e->state() == 0 ) ||
	 ( e->key() == Key_Down && (e->state() & AltButton) ) ||
	 ( !d->ed && e->key() == Key_Space ) ) {
	if ( count() ) {
	    if ( !d->usingListBox() )
		d->popup()->setActiveItem( this->d->current );
	    popup();
	}
	return;
    } else if ( d->usingListBox() && e->key() == Key_Up ) {
	c = currentItem();
	if ( c > 0 )
	    setCurrentItem( c-1 );
	else
	    setCurrentItem( count()-1 );
    } else if ( d->usingListBox() && e->key() == Key_Down ) {
	c = currentItem();
	if ( ++c < count() )
	    setCurrentItem( c );
	else
	    setCurrentItem( 0 );
    } else {
	e->ignore();
	return;
    }

    c = currentItem();
    if ( !text( c ).isNull() )
	emit activated( text( c ) );
    emit highlighted( c );
    emit activated( c );
}


/*!\reimp
*/

void QComboBox::focusInEvent( QFocusEvent * e )
{
    QWidget::focusInEvent( e );
}

/*!
  \internal
   Calculates the listbox height needed to contain all items, or as
   many as the list box is supposed to contain.
*/
static int listHeight( QListBox *l, int sl )
{
    if ( l->count() > 0 )
	return QMIN( l->count(), (uint)sl) * l->item( 0 )->height(l);
    else
	return l->sizeHint().height();
}


/*!
  Popups the combo box popup list.

  If the list is empty, no selections appear.
*/

void QComboBox::popup()
{
    if ( !count() )
	return;

    if ( d->usingListBox() ) {
			// Send all listbox events to eventFilter():
	d->listBox()->installEventFilter( this );
	d->listBox()->viewport()->installEventFilter( this );
	d->mouseWasInsidePopup = FALSE;
	d->listBox()->resize( width(),
			    listHeight( d->listBox(), d->sizeLimit ) +
			    d->listBox()->frameWidth() * 2 );
	QWidget *desktop = QApplication::desktop();
	int sw = desktop->width();			// screen width
	int sh = desktop->height();			// screen height
	QPoint pos = mapToGlobal( QPoint(0,height()) );

	// ### Similar code is in QPopupMenu
	int x = pos.x();
	int y = pos.y();
	int w = d->listBox()->width();
	int h = d->listBox()->height();

	// the complete widget must be visible
	if ( x + w > sw )
	    x = sw - w;
	else if ( x < 0 )
	    x = 0;
	if (y + h > sh && y - h - height() >= 0 )
	    y = y - h - height();

	d->listBox()->move( x,y );
	d->listBox()->raise();
	bool block = d->listBox()->signalsBlocked();
	d->listBox()->blockSignals( TRUE );
	d->listBox()->setCurrentItem( d->listBox()->item( d->current ) );
	d->listBox()->blockSignals( block );
	d->listBox()->setAutoScrollBar( TRUE );

#ifndef QT_NO_EFFECTS
	if ( QApplication::isEffectEnabled( UI_AnimateCombo ) ) {
	    if ( d->listBox()->y() < mapToGlobal(QPoint(0,0)).y() )
		qScrollEffect( d->listBox(), QEffects::UpScroll );
	    else
		qScrollEffect( d->listBox() );
	} else
#endif
	    d->listBox()->show();
    } else {
	d->popup()->installEventFilter( this );
	d->popup()->popup( mapToGlobal( QPoint(0,0) ), this->d->current );
    }
    d->poppedUp = TRUE;
}


/*!
  \reimp
*/
void QComboBox::updateMask()
{
    QBitmap bm( size() );
    bm.fill( color0 );

    {
	QPainter p( &bm, this );
	p.setPen( color1 );
	p.setBrush( color1 );
	style().drawComboButtonMask(&p, 0, 0, width(), height() );
    }
    setMask( bm );
}

/*!
  \internal
  Pops down (removes) the combo box popup list box.
*/
void QComboBox::popDownListBox()
{
    ASSERT( d->usingListBox() );
    d->listBox()->removeEventFilter( this );
    d->listBox()->viewport()->removeEventFilter( this );
    d->listBox()->hide();
    d->listBox()->setCurrentItem( d->current );
    if ( d->arrowDown ) {
	d->arrowDown = FALSE;
	repaint( FALSE );
    }
    d->poppedUp = FALSE;
}


/*!
  \internal
  Re-indexes the identifiers in the popup list.
*/

void QComboBox::reIndex()
{
    if ( !d->usingListBox() ) {
	int cnt = count();
	while ( cnt-- )
	    d->popup()->setId( cnt, cnt );
    }
}

/*!
  \internal
  Repaints the combo box.
*/

void QComboBox::currentChanged()
{
    if ( d->autoresize )
	adjustSize();
    update();
}

/*! \reimp

  \internal

  The event filter steals events from the popup or listbox when they
  are popped up. It makes the popup stay up after a short click in
  motif style. In windows style it toggles the arrow button of the
  combo box field, and activates an item and takes down the listbox
  when the mouse button is released.
*/

bool QComboBox::eventFilter( QObject *object, QEvent *event )
{
    if ( !event )
	return TRUE;
    else if ( object == d->ed ) {
	if ( event->type() == QEvent::KeyPress ) {
	    keyPressEvent( (QKeyEvent *)event );
	    if ( ((QKeyEvent *)event)->isAccepted() ) {
		d->completeNow = FALSE;
		return TRUE;
	    } else if ( ((QKeyEvent *)event)->key() != Key_End ) {
		d->completeNow = TRUE;
		d->completeAt = d->ed->cursorPosition();
	    }
	} else if ( event->type() == QEvent::KeyRelease ) {
	    d->completeNow = FALSE;
	    keyReleaseEvent( (QKeyEvent *)event );
	    return ((QKeyEvent *)event)->isAccepted();
	} else if ( (event->type() == QEvent::FocusIn ||
		     event->type() == QEvent::FocusOut ) ) {
	    d->completeNow = FALSE;
	    // to get the focus indication right
	    update();
	} else if ( d->useCompletion && d->completeNow ) {
	    if ( !d->ed->text().isNull() &&
		 d->ed->cursorPosition() > d->completeAt &&
		 d->ed->cursorPosition() == (int)d->ed->text().length() ) {
		d->completeNow = FALSE;
		QString ct( d->ed->text() );
		QString it;
		int i =0;
		int foundAt = -1;
		int foundLength = 100000; // lots
		while( i<count() ) {
		    it = text( i );
		    if ( it.length() >= ct.length() ) {
			it.truncate( ct.length() );
			int itlen = text( i ).length();
			if ( ( it.lower() == ct.lower() )
			    && itlen < foundLength ) {
			    foundAt = i;
			    foundLength = text( i ).length();
			    break;
			}
		    }
		    i++;
		}
		if ( foundAt > -1 ) {
		    it = text( foundAt );
            if ( !d->ed->hasMarkedText() ) {
                d->ed->validateAndSet( it, ct.length(),
					   ct.length(), it.length() );
            }
		}
	    }
	}
    } else if ( d->usingListBox() && ( object == d->listBox() ||
				       object == d->listBox()->viewport() )) {
	QMouseEvent *e = (QMouseEvent*)event;
	switch( event->type() ) {
	case QEvent::MouseMove:
	    if ( !d->mouseWasInsidePopup  ) {
		QPoint pos = e->pos();
		if ( d->listBox()->rect().contains( pos ) )
		    d->mouseWasInsidePopup = TRUE;
		// Check if arrow button should toggle
		// this applies only to windows style
		if ( d->arrowPressed ) {
		    QPoint comboPos;
		    comboPos = mapFromGlobal( d->listBox()->mapToGlobal(pos) );
		    if ( arrowRect().contains( comboPos ) ) {
			if ( !d->arrowDown  ) {
			    d->arrowDown = TRUE;
			    repaint( FALSE );
			}
		    } else {
			if ( d->arrowDown  ) {
			    d->arrowDown = FALSE;
			    repaint( FALSE );
			}
		    }
		}
	    } else if ((e->state() & ( RightButton | LeftButton | MidButton ) )
		       == 0 && style() == WindowsStyle ){
		QWidget *mouseW = QApplication::widgetAt( e->globalPos(), TRUE );
		if ( mouseW == d->listBox()->viewport() ) { //###
		    QMouseEvent m( QEvent::MouseMove, e->pos(), e->globalPos(),
				   0, LeftButton );
		    QApplication::sendEvent( object, &m ); //### Evil
		    return TRUE;
		}
	    }

	    break;
	case QEvent::MouseButtonRelease:
	    if ( d->listBox()->rect().contains( e->pos() ) ) {
		QMouseEvent tmp( QEvent::MouseButtonDblClick,
				 e->pos(), e->button(), e->state() ) ;
		// will hide popup
		QApplication::sendEvent( object, &tmp );
		return TRUE;
	    } else {
		if ( d->mouseWasInsidePopup ) {
		    popDownListBox();
		} else {
		    d->arrowPressed = FALSE;
		    if ( d->arrowDown  ) {
			d->arrowDown = FALSE;
			repaint( FALSE );
		    }
		}
	    }
	    break;
	case QEvent::MouseButtonDblClick:
	case QEvent::MouseButtonPress:
	    if ( !d->listBox()->rect().contains( e->pos() ) ) {
		QPoint globalPos = d->listBox()->mapToGlobal(e->pos());
		if ( QApplication::widgetAt( globalPos, TRUE ) == this ) {
		    d->discardNextMousePress = TRUE;
		    // avoid popping up again
		}
		popDownListBox();
		return TRUE;
	    }
	    break;
	case QEvent::KeyPress:
	    switch( ((QKeyEvent *)event)->key() ) {
	    case Key_Up:
	    case Key_Down:
		if ( !(((QKeyEvent *)event)->state() & AltButton) )
		    break;
	    case Key_F4:
	    case Key_Escape:
		if ( d->poppedUp ) {
		    popDownListBox();
		    return TRUE;
		}
		break;
	    case Key_Enter:
	    case Key_Return:
		// work around QDialog's enter handling
		return FALSE;
	    default:
		break;
	    }
	default:
	    break;
	}
    } else if ( !d->usingListBox() && object == d->popup() ) {
	QMouseEvent *e = (QMouseEvent*)event;
	switch ( event->type() ) {
	case QEvent::MouseButtonRelease:
	    if ( d->shortClick ) {
		QMouseEvent tmp( QEvent::MouseMove,
				 e->pos(), e->button(), e->state() ) ;
		// highlight item, but don't pop down:
		QApplication::sendEvent( object, &tmp );
		return TRUE;
	    }
	    break;
	case QEvent::MouseButtonDblClick:
	case QEvent::MouseButtonPress:
	    if ( !d->popup()->rect().contains( e->pos() ) ) {
		// remove filter, event will take down popup:
		d->popup()->removeEventFilter( this );
		// ### uglehack!
		// call internalHighlight so the highlighed signal
		// will be emitted at least as often as necessary.
		// it may be called more often than necessary
		internalHighlight( d->current );
	    }
	    break;
	default:
	    break;
	}
    }
    return QWidget::eventFilter( object, event );
}


/*!
  Returns the current maximum on-screen size of the combo box.  The
  default is ten lines.

  \sa setSizeLimit() count() maxCount()
*/

int QComboBox::sizeLimit() const
{
    return d ? d->sizeLimit : INT_MAX;
}


/*!

  Sets the maximum on-screen size of the combo box to \a lines.  This
  is disregarded in Motif 1.x style.  The default limit is ten lines.

  If the number of items in the combo box is/grows larger than
  \c lines, a list box is added.

  \sa sizeLimit() count() setMaxCount()
*/

void QComboBox::setSizeLimit( int lines )
{
    d->sizeLimit = lines;
}



/*!
  Returns the current maximum size of the combo box.  By default,
  there is no limit, so this function returns INT_MAX.

  \sa setMaxCount() count()
*/

int QComboBox::maxCount() const
{
    return d ? d->maxCount : INT_MAX;
}


/*!
  Sets the maximum number of items the combo box can hold to \a count.

  If \a count is smaller than the current number of items, the list is
  truncated at the end.  There is no limit by default.

  \sa maxCount() count()
*/

void QComboBox::setMaxCount( int count )
{
    int l = this->count();
    while( --l > count )
	removeItem( l );
    d->maxCount = count;
}

/*!
  Returns the current insertion policy of the combo box.

  \sa setInsertionPolicy()
*/

QComboBox::Policy QComboBox::insertionPolicy() const
{
    return d->p;
}


/*!
  Sets the insertion policy of the combo box to \a policy.

  The insertion policy governs where items typed in by the user are
  inserted in the list.  The possible values are <ul> <li> \c
  NoInsertion: Strings typed by the user aren't inserted anywhere <li>
  \c AtTop: Strings typed by the user are inserted above the top item
  in the list <li> AtCurrent: Strings typed by the user replace the
  last selected item <li> AtBottom: Strings typed by the user are
  inserted at the bottom of the list. </ul>

  The default insertion policy is \c AtBottom.

  \sa insertionPolicy()
*/

void QComboBox::setInsertionPolicy( Policy policy )
{
    d->p = policy;
}



/*!
  Internal slot to keep the line editor up to date.
*/

void QComboBox::returnPressed()
{
    QString s( d->ed->text() );

    if ( s.isEmpty() ) {
	d->ed->setText( text( currentItem() ) );
	d->ed->selectAll();
	return;
    }

    int c = 0;
    bool doInsert = TRUE;
    if ( !d->duplicatesEnabled ) {
	for ( int i = 0; i < count(); ++i ) {
	    if ( s == text( i ) ) {
		doInsert = FALSE;
		c = i;
		break;
	    }
	}
    }

    if ( doInsert ) {
	if ( insertionPolicy() != NoInsertion ) {
	    int cnt = count();
	    while ( cnt >= d->maxCount ) {
		removeItem( --cnt );
	    }
	}
	
	switch ( insertionPolicy() ) {
	case AtCurrent:
	    if ( s != text( currentItem() ) )
		changeItem( s, currentItem() );
	    emit activated( currentItem() );
	    emit activated( s );
	    return;
	case NoInsertion:
	    emit activated( s );
	    return;
	case AtTop:
	    c = 0;
	    break;
	case AtBottom:
	    c = count();
	    break;
	case BeforeCurrent:
	    c = currentItem();
	    break;
	case AfterCurrent:
	    c = currentItem() + 1;
	    break;
	}
	insertItem( s, c );
    }

    setCurrentItem( c );
    emit activated( c );
    emit activated( s );
}


/*! \reimp
*/

void QComboBox::setEnabled( bool enable )
{
    QWidget::setEnabled( enable );
}



/*!  Sets this combo box to be editable only as allowed by \a v.

  This function does nothing if the combo is not editable.

  \sa validator() clearValidator() QValidator
*/

void QComboBox::setValidator( const QValidator * v )
{
    if ( d && d->ed )
	d->ed->setValidator( v );
}


/*!  Returns the validator which constrains editing for this combo
  box if there is any, or else 0.

  \sa setValidator() clearValidator() QValidator
*/

const QValidator * QComboBox::validator() const
{
    return d && d->ed ? d->ed->validator() : 0;
}


/*!  This slot is equivalent to setValidator( 0 ). */

void QComboBox::clearValidator()
{
    if ( d && d->ed )
	d->ed->setValidator( 0 );
}


/*!  Sets the combo box to use \a newListBox instead of the current
  list box or popup.  As a side effect, clears the combo box of its
  current contents.

  \warning QComboBox assumes that newListBox->text(n) returns
  non-null for 0 \<= n \< newListbox->count().  This assumption is
  necessary because of the line edit in QComboBox.
*/

void QComboBox::setListBox( QListBox * newListBox )
{
    clear();

    if ( d->usingListBox() )
	delete d->listBox();
    else
	delete d->popup();

    newListBox->reparent( 0, WType_Popup, QPoint(0,0), FALSE );
    d->setListBox( newListBox );
    d->listBox()->setFont( font() );
    d->listBox()->setAutoScrollBar( FALSE );
    d->listBox()->setBottomScrollBar( FALSE );
    d->listBox()->setAutoBottomScrollBar( FALSE );
    d->listBox()->setFrameStyle( QFrame::Box | QFrame::Plain );
    d->listBox()->setLineWidth( 1 );
    d->listBox()->resize( 100, 10 );

    connect( d->listBox(), SIGNAL(selected(int)),
	     SLOT(internalActivate(int)) );
    connect( d->listBox(), SIGNAL(highlighted(int)),
	     SLOT(internalHighlight(int)));
}


/*!  Returns the current list box, or 0 if there is no list box
  currently.  (QComboBox can use QPopupMenu instead of QListBox.)
  Provided to match setListBox().

  \sa setListBox()
*/

QListBox * QComboBox::listBox() const
{
    return d && d->usingListBox() ? d->listBox() : 0;
}

/*!
  Returns the line editor, or 0 if there is no line editor currently.

  Only editable listboxes have a line editor.
 */
QLineEdit* QComboBox::lineEdit() const
{
    return d->ed;
}



/*!  Clears the line edit without changing the combo's contents.  Does
  nothing if the combo isn't editable.

  This is particularly handy when using a combo box as a line edit
  with history.  For example you can connect the combo's activated()
  signal to clearEdit() in order to present the user with a new, empty
  line as soon as return is pressed.

  \sa setEditText()
*/

void QComboBox::clearEdit()
{
    if ( d && d->ed )
	d->ed->clear();
}


/*!  Sets the text in the embedded line edit to \a newText without
  changing the combo's contents.  Does nothing if the combo isn't
  editable.

  This is useful e.g. for providing a good starting point for the
  user's editing and entering the change in the combo only when the
  user presses enter.

  \sa clearEdit() insertItem()
*/

void QComboBox::setEditText( const QString &newText )
{
    if ( d && d->ed ) {
	d->updateLinedGeometry();
	d->ed->setText( newText );
    }
}


/*!  Sets this combo box to offer auto-completion while the user is
  editing if \a enable is TRUE, or not to offer auto-completion of \a
  enable is FALSE (the default).

  The combo box uses the list of items as candidates for completion.

  Note: This will only work on editable combo boxes, so make the combo
  box editable before you call this function or it will not work.

  \sa autoCompletion() setEditText()
*/

void QComboBox::setAutoCompletion( bool enable )
{
    d->useCompletion = enable;
    d->completeNow = FALSE;
}


/*!  Returns TRUE if this combo box is in auto-completion mode.

  \sa setAutoCompletion()
*/

bool QComboBox::autoCompletion() const
{
    return d->useCompletion;
}

/*!\reimp
 */
void QComboBox::styleChange( QStyle& s )
{
    d->sizeHint = QSize();  // Invalidate Size Hint
    if ( d->ed )
	d->updateLinedGeometry();
    QWidget::styleChange( s );
}

/*!
  Returns whether the combobox is editable or not.

  \sa setEditable()
 */
bool QComboBox::editable() const
{
    return d->ed != 0;
}


/*!
  Make the input field editable, if \a y is TRUE. Otherwise the user
  may only choose one of the items in the combo box.

  \sa editable()
 */
void QComboBox::setEditable( bool y )
{
    if ( y == editable() )
	return;
    if ( y ) {
	if ( !d->usingListBox() )
	    setUpListBox();
	setUpLineEdit();
	d->ed->show();
	setFocusPolicy( StrongFocus );
    } else {
	delete d->ed;
	d->ed = 0;
	setFocusPolicy( TabFocus );
    }
    updateGeometry();
    update();
}


void QComboBox::setUpListBox()
{
    d->setListBox( new QListBox( this, "in-combo", WType_Popup ) );
    d->listBox()->setFont( font() );
    d->listBox()->setAutoScrollBar( FALSE );
    d->listBox()->setBottomScrollBar( FALSE );
    d->listBox()->setAutoBottomScrollBar( FALSE );
    d->listBox()->setFrameStyle( QFrame::Box | QFrame::Plain );
    d->listBox()->setLineWidth( 1 );
    d->listBox()->resize( 100, 10 );

    connect( d->listBox(), SIGNAL(selected(int)),
	     SLOT(internalActivate(int)) );
    connect( d->listBox(), SIGNAL(highlighted(int)),
	     SLOT(internalHighlight(int)));
}


void QComboBox::setUpLineEdit()
{
    d->ed = new QLineEdit( this, "combo edit" );
    connect (d->ed, SIGNAL( textChanged( const QString& ) ),
	     this, SIGNAL( textChanged( const QString& ) ) );
    d->ed->setFrame( FALSE );
    d->updateLinedGeometry();
    d->ed->installEventFilter( this );
    setFocusProxy( d->ed );

    connect( d->ed, SIGNAL(returnPressed()), SLOT(returnPressed()) );
}


#endif // QT_NO_COMBOBOX
