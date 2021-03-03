/**********************************************************************
** $Id: qt/src/widgets/qlabel.cpp   2.3.2   edited 2001-10-08 $
**
** Implementation of QLabel widget class
**
** Created : 941215
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

#include "qlabel.h"
#ifndef QT_NO_LABEL
#include "qbitmap.h"
#include "qpainter.h"
#include "qdrawutil.h"
#include "qaccel.h"
#include "qmovie.h"
#include <ctype.h>
#include "qimage.h"
#include "qbitmap.h"
#include "qapplication.h"
#include "qsimplerichtext.h"
#include "qstylesheet.h"
#include "qlineedit.h"


class QLabelPrivate
{
public:
    QLabelPrivate()
	:minimumWidth(0), img(0), pix(0)
    {}
    int minimumWidth; // for richtext
    QImage* img; // for scaled contents
    QPixmap* pix; // for scaled contents
};


// BEING REVISED: aavit
/*!
  \class QLabel qlabel.h
  \brief The QLabel widget provides a static information display

  \ingroup basic

  QLabel is used for displaying information in the form of text or
  image to the user. No user interaction functionality is
  provided. The visual appearance of the label can be configured in
  various ways, and it can be used for specifying a focus accelerator
  key for another widget.

  A QLabel can contain any of the following content types:
  <ul>
  <li> A plain text: set by passing a QString to setText().
  <li> A rich text: set by passing a QString that contains a rich text to setText().
  <li> A pixmap: set by passing a QPixmap to setPixmap().
  <li> A movie: set by passing a QMovie to setMovie().
  <li> A number: set by passing an \e int or a \e double to setNum(), which converts the number to plain text.
  <li> Nothing: The same as an empty plain text. This is the default. Set by clear().
  </ul>

  When the content is changed using any of these functions, any
  previous content is cleared.

  The look of a QLabel can be tuned in several ways. All the settings
  of QFrame are available for specifying a widget frame. The
  positioning of the content within the QLabel widget area can be
  tuned with setAlignment() and setIndent().  For example, this code
  sets up a sunken panel with a two-line text in the bottom right
  corner (both lines being flush with the right side of the label):

  \code
    QLabel *label = new QLabel;
    label->setFrameStyle( QFrame::Panel | QFrame::Sunken );
    label->setText( "first line\nsecond line" );
    label->setAlignment( AlignBottom | AlignRight );
  \endcode

  A QLabel is often used as a label for another, interactive
  widget. For this use, QLabel provides a handy mechanism for adding
  an accelerator key (see QAccel) that will set the keyboard focus to
  the other widget (called the QLabel's "buddy"). Example:

  \code
     QLineEdit* phoneEdit = new QLineEdit( this, "phoneEdit" );
     QLabel* phoneLabel = new QLabel( phoneEdit, "&Phone:", this, "phoneLabel" );
  \endcode

  In this example, keyboard focus is transferred to the label's buddy
  (the QLineEdit) when the user presses <dfn>Alt-P.</dfn> You can also
  use the setBuddy() function to accomplish the same.

  <img src=qlabel-m.png> <img src=qlabel-w.png>

  \sa QLineEdit, QTextView, QPixmap, QMovie,
  <a href="guibooks.html#fowler">GUI Design Handbook: Label</a>
*/


/*!
  Constructs an empty label.

  The \a parent, \a name and \a f arguments are passed to the QFrame
  constructor.

  \sa setAlignment(), setFrameStyle(), setIndent()
*/

QLabel::QLabel( QWidget *parent, const char *name, WFlags f )
    : QFrame( parent, name, f | WMouseNoMask  )
{
    init();
}


/*!
  Constructs a label with a text. The \a text is set with setText().

  The \a parent, \a name and \a f arguments are passed to the QFrame
  constructor.

  \sa setText(), setAlignment(), setFrameStyle(), setIndent()
*/

QLabel::QLabel( const QString &text, QWidget *parent, const char *name,
		WFlags f )
	: QFrame( parent, name, f | WMouseNoMask  )
{
    init();
    setText( text );
}


/*!
  Constructs a label with a text and a buddy.

  The \a text is set with setText(). The \a buddy is set with setBuddy().

  The \a parent, \a name and \a f arguments are passed to the QFrame
  constructor.

  \sa setText(), setBuddy(), setAlignment(), setFrameStyle(),
  setIndent()
*/
QLabel::QLabel( QWidget *buddy,  const QString &text,
		QWidget *parent, const char *name, WFlags f )
    : QFrame( parent, name, f | WMouseNoMask )
{
    init();
#ifndef QT_NO_ACCEL
    setBuddy( buddy );
#else
    if ( buddy )
	setAlignment( alignment() | ShowPrefix );
#endif
    setText( text );
}

/*!
  Destructs the label.
*/

QLabel::~QLabel()
{
    clearContents();
    delete d;
}


void QLabel::init()
{
    lpixmap = 0;
#ifndef QT_NO_MOVIE
    lmovie = 0;
#endif
#ifndef QT_NO_ACCEL
    lbuddy = 0;
    accel = 0;
#endif
    lpixmap = 0;
    align = AlignLeft | AlignVCenter | ExpandTabs;
    extraMargin= -1;
    autoresize = FALSE;
    scaledcontents = FALSE;
    textformat = Qt::AutoText;
#ifndef QT_NO_RICHTEXT
    doc = 0;
#endif

    setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum ) );
    d = new QLabelPrivate;
}


/*!
  \fn QString QLabel::text() const

  Returns the label text. If the content is a plain or a rich text,
  this is the string that was passed to setText(). Otherwise, it is an
  empty/null string.

  \sa setText(), setNum(), clear()
*/


/*!
  Sets the label contents to \a text, or does nothing if \a text is
  equal to the current contents of the label. Any previous content is
  cleared.

  \a text will be interpreted either as a plain text or as a rich
  text, depending on the text format setting; see setTextFormat(). The
  default setting is \c AutoText, i.e. QLabel will try to auto-detect
  the format of \a text.

  If \a text is interpreted as a plain text, and a buddy has been set,
  the buddy accelerator key is updated from the new text.

  The label resizes itself if auto-resizing is enabled.

  Note that Qlabel is well suited to display small rich text documents
  only. For large documents, use QTextView instead. It will flicker
  less on resize and can also provide a scrollbar if necessary.

  \sa text(), setTextFormat(), setBuddy(), setAlignment()
*/

void QLabel::setText( const QString &text )
{
    if ( ltext == text )
	return;
    QSize osh = sizeHint();
    clearContents();
    ltext = text;
#ifndef QT_NO_ACCEL
    int p = QAccel::shortcutKey( ltext );
    if ( p ) {
	if ( !accel )
	    accel = new QAccel( this, "accel label accel" );
	accel->connectItem( accel->insertItem( p ),
			    this, SLOT(acceleratorSlot()) );
    }
#endif
#ifndef QT_NO_RICHTEXT
    if ( textformat == RichText ||
	 ( textformat == AutoText && QStyleSheet::mightBeRichText(ltext) ) ) {
	doc = new QSimpleRichText( ltext, font() );
	doc->setWidth( 10 );
	d->minimumWidth = doc->widthUsed();
    }
#endif

    updateLabel( osh );
}


/*!
  Clears any label contents. Equivalent with setText( "" ).
*/

void QLabel::clear()
{
    setText( QString::fromLatin1("") );
}


/*!
  \fn QPixmap *QLabel::pixmap() const

  If the label contains a pixmap, returns a pointer to it. Otherwise,
  returns 0.

  \sa setPixmap()
*/


/*!
  Sets the label contents to \a pixmap. Any previous content is cleared.

  The buddy accelerator, if any, is disabled.

  The label resizes itself if auto-resizing is enabled.

  \sa pixmap(), setBuddy()
*/

void QLabel::setPixmap( const QPixmap &pixmap )
{
    QSize osh = sizeHint();
    if ( !lpixmap || lpixmap->serialNumber() != pixmap.serialNumber() ) {
	clearContents();
	lpixmap = new QPixmap( pixmap );
    }

    if ( lpixmap->depth() == 1 && !lpixmap->mask() )
	lpixmap->setMask( *((QBitmap *)lpixmap) );

    updateLabel( osh );
}


/*!
  Sets the label contents to a plain text containing the printed value
  of \a num. Does nothing if this is equal to the current contents of
  the label. Any previous content is cleared.

  The buddy accelerator, if any, is disabled.

  The label resizes itself if auto-resizing is enabled.

  \sa setText(), QString::setNum(), setBuddy()
*/

void QLabel::setNum( int num )
{
    QString str;
    str.setNum( num );
	setText( str );
}

/*!
  Sets the label contents to a plain text containing the printed value
  of \a num.  Does nothing if this is equal to the current contents of
  the label. Any previous content is cleared.

  The buddy accelerator, if any, is disabled.

  The label resizes itself if auto-resizing is enabled.

  \sa setText(), QString::setNum(), setBuddy()
*/

void QLabel::setNum( double num )
{
    QString str;
    str.setNum( num );
	setText( str );
}

/*!
  \fn int QLabel::alignment() const

  Returns the alignment setting.

  \sa setAlignment()
*/

/*!
  Sets the alignment of the label contents.

  The \a alignment must be a bitwise OR of Qt::AlignmentFlags
  values. The \c WordBreak, \c ExpandTabs, \c SingleLine and \c
  ShowPrefix flags apply only if the label contains a plain text, and
  are otherwise ignored. The \c DontClip flag is always ignored.

  If the label has a buddy, the \c ShowPrefix flag is forced to TRUE.

  The default alignment is <code>AlignLeft | AlignVCenter |
  ExpandTabs</code> if the label doesn't have a buddy and
  <code>AlignLeft | AlignVCenter | ExpandTabs | ShowPrefix </code> if
  the label has a buddy.

  \sa Qt::AlignmentFlags, alignment(), setBuddy(), setText()
*/

void QLabel::setAlignment( int alignment )
{
    if ( alignment == align )
	return;
    QSize osh = sizeHint();
#ifndef QT_NO_ACCEL
    if ( lbuddy )
	align = alignment | ShowPrefix;
    else
#endif
	align = alignment;

    updateLabel( osh );
}


/*!
  \fn int QLabel::indent() const

  Returns the indent of the label.

  \sa setIndent()
*/

/*!
  Sets the indent of the label to \a indent pixels.

  The indent applies to the left edge if alignment() is \c AlignLeft,
  to the right edge if alignment() is \c AlignRight, to the top edge
  if alignment() is \c AlignTop, and to to the bottom edge if
  alignment() is \c AlignBottom.

  If \a indent is negative, or if no indent has been set, the label
  computes the effective indent as follows: If frameWidth() is 0, the
  effective indent becomes 0. If frameWidth() is greater than 0, the
  effective indent becomes half the width of the "x" character of the
  widget's current font().

  If \a indent is non-negative, the effective indent is \a indent
  pixels.

  \sa indent(), setAlignment(), frameWidth(), font()
*/

void QLabel::setIndent( int indent )
{
    extraMargin = indent;
    updateLabel( QSize( -1, -1 ) );
}


/*!
  \fn bool QLabel::autoResize() const

  \obsolete

  Returns TRUE if auto-resizing is enabled, or FALSE if auto-resizing
  is disabled.

  Auto-resizing is disabled by default.

  \sa setAutoResize()
*/

/*! \obsolete
  Enables auto-resizing if \a enable is TRUE, or disables it if \a
  enable is FALSE.

  When auto-resizing is enabled, the label will resize itself to fit
  the contents whenever the contents change. The top left corner is
  not moved. This is useful for QLabel widgets that are not managed by
  a QLayout (e.g. top-level widgets).

  Auto-resizing is disabled by default.

  \sa autoResize(), adjustSize(), sizeHint()
*/

void QLabel::setAutoResize( bool enable )
{
    if ( (bool)autoresize != enable ) {
	autoresize = enable;
	if ( autoresize )
	    adjustSize();			// calls resize which repaints
    }
}



/*!
  Returns the size that will be used if the width of the label is
  \a w. If \a w is -1, the sizeHint() is returned.
*/

QSize QLabel::sizeForWidth( int w ) const
{
    QFontMetrics fm = fontMetrics();
    QRect br;
    QPixmap *pix = pixmap();
#ifndef QT_NO_MOVIE
    QMovie *mov = movie();
#endif
    int fw = frameWidth();
    int m  = 2*indent();
    if ( m < 0 ) {
	if ( fw > 0 )
	    m = fm.width( 'x' );
	else
	    m = 0;
    }
    if ( pix ) {
	br = pix->rect();
    }
#ifndef QT_NO_MOVIE
    else if ( mov ) {
	br = mov->framePixmap().rect();
    }
#endif
#ifndef QT_NO_RICHTEXT
    else if ( doc ) {
	if ( w < 0 )
	    doc->adjustSize();
	else {
	    w -= 2*fw + m;
	    doc->setWidth( w );
	}
	br = QRect( 0, 0, doc->widthUsed(), doc->height() );
    }
    else
#endif
    {
	bool tryWidth = (w < 0) && (align & WordBreak);
	if ( tryWidth )
	    w = fm.width( 'x' ) * 80;
	else if ( w < 0 )
	    w = 2000;
	br = fm.boundingRect( 0, 0, w ,2000, alignment(), text() );
	if ( tryWidth && br.height() < 4*fm.lineSpacing() && br.width() > w/2 )
	    	br = fm.boundingRect( 0, 0, w/2, 2000, alignment(), text() );
	if ( tryWidth && br.height() < 2*fm.lineSpacing() && br.width() > w/4 )
	    br = fm.boundingRect( 0, 0, w/4, 2000, alignment(), text() );
	// adjust so "Yes" and "yes" will have the same height
	int h = fm.lineSpacing();
	if ( h <= 0 ) // for broken fonts....
	    h = 14;
	br.setHeight( ((br.height() + h-1) / h)*h - fm.leading() );
	if ( indent() > 0 ) {
	    if ( (align & AlignLeft) || (align & AlignRight ) )
		br.setWidth( br.width() + indent() );
	    else if ( (align & AlignTop) || (align & AlignBottom ) )
		br.setHeight( br.height() + indent() );
	}
    }
    int wid = br.width() + m + 2*fw;
    int hei = br.height() + m + 2*fw;

    return QSize( wid, hei );
}


/*!
  \reimp
*/

int QLabel::heightForWidth( int w ) const
{
#ifndef QT_NO_RICHTEXT
    if ( doc || align & WordBreak )
	return sizeForWidth( w ).height();
#endif
    return QWidget::heightForWidth( w );
}



/*!\reimp
*/
QSize QLabel::sizeHint() const
{
    //     Does not work well with the WordBreak flag; use
    //    heightForWidth() in stead.
    return sizeForWidth( -1 );
}

/*!
  \reimp
*/

QSize QLabel::minimumSizeHint() const
{
#ifndef QT_NO_RICHTEXT
    if ( doc )
	return QSize( d->minimumWidth, -1 );
#endif
    return QSize( -1, -1 );
}


/*!\reimp
*/
QSizePolicy QLabel::sizePolicy() const
{
    //### removeme 3.0
    return QWidget::sizePolicy();
}

/*!
  \reimp
*/
void QLabel::resizeEvent( QResizeEvent* e )
{
    QFrame::resizeEvent( e );

#ifdef QT_NO_RICHTEXT
    static const bool doc = FALSE;
#endif

    // optimize for standard labels
    if ( frameShape() == NoFrame && (align & WordBreak) == 0 && !doc &&
	 ( e->oldSize().width() >= e->size().width() && (align & AlignLeft ) == AlignLeft )
	 && ( e->oldSize().height() >= e->size().height() && (align & AlignTop ) == AlignTop ) ) {
	setWFlags( WResizeNoErase );
	return;
    }

    clearWFlags( WResizeNoErase );
    QRect cr = contentsRect();
    if ( !lpixmap ||  !cr.isValid() ||
	 // masked pixmaps can only reduce flicker when being top/left
	 // aligned and when we do not perform scaled contents
	 ( lpixmap->mask() && ( scaledcontents || ( ( align & (AlignLeft|AlignTop) ) != (AlignLeft|AlignTop) ) ) ) )
	return;

    // don't we all love QFrame? Reduce pixmap flicker
    setWFlags( WResizeNoErase );
    QRegion reg = QRect( QPoint(0, 0), e->size() );
    reg = reg.subtract( cr );
    if ( !scaledcontents ) {
	int x = cr.x();
	int y = cr.y();
	int w = lpixmap->width();
	int h = lpixmap->height();
	if ( (align & Qt::AlignVCenter) == Qt::AlignVCenter )
	    y += cr.height()/2 - h/2;
	else if ( (align & Qt::AlignBottom) == Qt::AlignBottom)
	    y += cr.height() - h;
	if ( (align & Qt::AlignRight) == Qt::AlignRight )
	    x += cr.width() - w;
	else if ( (align & Qt::AlignHCenter) == Qt::AlignHCenter )
	    x += cr.width()/2 - w/2;
	if ( x > cr.x() )
	    reg = reg.unite( QRect( cr.x(), cr.y(), x - cr.x(), cr.height() ) );
	if ( y > cr.y() )
	    reg = reg.unite( QRect( cr.x(), cr.y(), cr.width(), y - cr.y() ) );

	if ( x + w < cr.right() )
	    reg = reg.unite( QRect( x + w, cr.y(),  cr.right() - x - w, cr.height() ) );
	if ( y + h < cr.bottom() )
	    reg = reg.unite( QRect( cr.x(), y +  h, cr.width(), cr.bottom() - y - h ) );

	erase( reg );
    }
}


/*!
  Draws the label contents using the painter \a p.
*/

void QLabel::drawContents( QPainter *p )
{
    QRect cr = contentsRect();

#ifndef QT_NO_MOVIE
    QMovie *mov = movie();
#else
    const int mov = 0;
#endif

    int m = indent();
    if ( m < 0 && !mov ) {
	// This is ugly.
	if ( frameWidth() > 0 )
	    m = p->fontMetrics().width('x')/2;
	else
	    m = 0;
    }
    if ( m > 0 ) {
	if ( align & AlignLeft )
	    cr.setLeft( cr.left() + m );
	if ( align & AlignRight )
	    cr.setRight( cr.right() - m );
	if ( align & AlignTop )
	    cr.setTop( cr.top() + m );
	if ( align & AlignBottom )
	    cr.setBottom( cr.bottom() - m );
    }

#ifndef QT_NO_MOVIE
    if ( mov ) {
	// ### should add movie to qDrawItem
 	QRect r = style().itemRect( p,
				    cr.x(), cr.y(), cr.width(), cr.height(),
				    align, isEnabled(), &(mov->framePixmap()),
				    QString::null );
	// ### could resize movie frame at this point
	p->drawPixmap(r.x(), r.y(), mov->framePixmap() );
    }
    else
#endif
#ifndef QT_NO_RICHTEXT
    if ( doc ) {
	doc->setWidth(p, cr.width() );
	int rw = doc->widthUsed();
	int rh = doc->height();
	int xo = 0;
	int yo = 0;
	if ( align & AlignVCenter )
	    yo = (cr.height()-rh)/2;
	else if ( align & AlignBottom )
	    yo = cr.height()-rh;
	if ( align & AlignRight )
	    xo = cr.width()-rw;
	else if ( align & AlignHCenter )
	    xo = (cr.width()-rw)/2;
	if ( style() == WindowsStyle && !isEnabled() ) {
	    QColorGroup cg = colorGroup();
	    cg.setColor( QColorGroup::Text, cg.light() );
	    doc->draw(p, cr.x()+xo+1, cr.y()+yo+1, cr, cg, 0);
	}
	doc->draw(p, cr.x()+xo, cr.y()+yo, cr, colorGroup(), 0);
    } else
#endif
    {
	QPixmap* pix = lpixmap;
#ifndef QT_NO_IMAGE_SMOOTHSCALE
	if ( scaledcontents && lpixmap ) {
	    if ( !d->img )
		d->img = new QImage( lpixmap->convertToImage() );
	    if ( !d->pix )
		d->pix = new QPixmap;
	    if ( d->pix->size() != cr.size() )
		d->pix->convertFromImage( d->img->smoothScale( cr.width(), cr.height() ) );
	    pix = d->pix;
	}
#endif
	// ordinary text or pixmap label
	style().drawItem( p, cr.x(), cr.y(), cr.width(), cr.height(),
			  align, colorGroup(), isEnabled(),
			  pix, ltext );
    }
}


/*!
  \reimp
*/

void QLabel::setAutoMask(bool b)
{
    if ( b )
	setBackgroundMode( PaletteText );
    else
	setBackgroundMode( PaletteBackground );
    QFrame::setAutoMask( b );
}

/*!
  Draws the label contents mask using the painter \a p.
  Used only in transparent mode.

  \sa QWidget::setAutoMask();
*/

void QLabel::drawContentsMask( QPainter *p )
{
    QRect cr = contentsRect();
    int m = indent();
    if ( m < 0 ) {
	if ( frameWidth() > 0 )
	    m = p->fontMetrics().width('x')/2;
	else
	    m = 0;
    }
    if ( m > 0 ) {
	if ( align & AlignLeft )
	    cr.setLeft( cr.left() + m );
	if ( align & AlignRight )
	    cr.setRight( cr.right() - m );
	if ( align & AlignTop )
	    cr.setTop( cr.top() + m );
	if ( align & AlignBottom )
	    cr.setBottom( cr.bottom() - m );
    }

#ifndef QT_NO_MOVIE
    QMovie *mov = movie();
    if ( mov ) {
	// ### could add movie to qDrawItem
	QRect r = style().itemRect( p,
				    cr.x(), cr.y(), cr.width(), cr.height(),
				    align, isEnabled(), &(mov->framePixmap()),
				    QString::null );
	// ### could resize movie frame at this point
	QPixmap pm = mov->framePixmap();
	if ( pm.mask() ) {
	    p->setPen( color1);
	    p->drawPixmap(r.x(), r.y(), *pm.mask() );
	}
	else
	    p->fillRect( r, color1 );
	return;
    }
#endif
    QColorGroup g( color1, color1, color1, color1, color1, color1, color1,
		   color1, color0);

    QBitmap bm;
    QPixmap* pix = lpixmap;
#ifndef QT_NO_IMAGE_SMOOTHSCALE
    if ( scaledcontents && lpixmap ) {
	if ( !d->img )
	    d->img = new QImage( lpixmap->convertToImage() );
	if ( !d->pix )
	    d->pix = new QPixmap;
	if ( d->pix->size() != cr.size() )
	    d->pix->convertFromImage( d->img->smoothScale( cr.width(), cr.height() ) );
	pix = d->pix;
    }
#endif
    if (pix ) {
	if (pix->mask()) {
	    bm = *pix->mask();
	}
	else {
	    bm.resize( pix->size() );
	    bm.fill(color1);
	}
    }

#ifndef QT_NO_RICHTEXT
    if ( doc ) {
	doc->setWidth(p, cr.width() );
	int rw = doc->widthUsed();
	int rh = doc->height();
	int xo = 0;
	int yo = 0;
	if ( align & AlignVCenter )
	    yo = (cr.height()-rh)/2;
	else if ( align & AlignBottom )
	    yo = cr.height()-rh;
	if ( align & AlignRight )
	    xo = cr.width()-rw;
	else if ( align & AlignHCenter )
	    xo = (cr.width()-rw)/2;
	if ( style() == WindowsStyle && !isEnabled() ) {
	    doc->draw(p, cr.x()+xo+1, cr.y()+yo+1, cr, g, 0);
	}
	doc->draw(p, cr.x()+xo, cr.y()+yo, cr, g, 0);
    } else
#endif
    {
	style().drawItem( p, cr.x(), cr.y(), cr.width(), cr.height(),
			  align, g, isEnabled(), bm.isNull()?0:&bm, ltext );
    }
}


/*!
  Updates the label, not the frame.
*/

void QLabel::updateLabel( QSize oldSizeHint )
{
    QSizePolicy policy = sizePolicy();
    if (
#ifndef QT_NO_RICHTEXT
	doc ||
#endif
	(align & WordBreak) ) {
	if ( policy == QSizePolicy( QSizePolicy::Minimum,
				    QSizePolicy::Minimum ) )
	    policy = QSizePolicy( QSizePolicy::Preferred,
				  QSizePolicy::Preferred, TRUE );
	else
	    policy.setHeightForWidth( TRUE );
    } else {
	policy.setHeightForWidth( FALSE );
    }
    setSizePolicy( policy );
    QRect cr = contentsRect();
    if ( sizeHint() != oldSizeHint )
	updateGeometry();
    if ( autoresize ) {
	adjustSize();
        update(cr.x(), cr.y(), cr.width(), cr.height());
    } else {
        update(cr.x(), cr.y(), cr.width(), cr.height());
	if ( autoMask() )
	    updateMask();
    }
}


/*!
  \internal

  Internal slot, used to set focus for accelerator labels.
*/
#ifndef QT_NO_ACCEL
void QLabel::acceleratorSlot()
{
    if ( !lbuddy )
	return;
    QWidget * w = lbuddy;
    while ( w->focusProxy() )
	w = w->focusProxy();
    if ( !w->hasFocus() &&
	 w->isEnabled() &&
	 w->isVisible() &&
	 w->focusPolicy() != NoFocus ) {
	w->setFocus();
	if ( w->inherits( "QLineEdit" ) )
	    ( (QLineEdit*)w )->selectAll();
    }
}
#endif

/*!
  \internal

  Internal slot, used to clean up if the buddy widget dies.
*/
#ifndef QT_NO_ACCEL
void QLabel::buddyDied() // I can't remember if I cried.
{
    lbuddy = 0;
}

/*!
  Sets the buddy of this label to \a buddy.

  When the user presses the accelerator key indicated by this label,
  the keyboard focus is transferred to the label's buddy widget.

  The buddy mechanism is only available for QLabels that contain a
  plain text in which one letter is prefixed with '&'. It is this
  letter that is set as the accelerator key. The letter is displayed
  underlined, and the '&' is not displayed (i.e. the \c ShowPrefix
  alignment flag is turned on; see setAlignment()).

  In a dialog, you might create two data entry widgets and a label for
  each, and set up the geometry layout so each label is just to the
  left of its data entry widget (its "buddy"), somewhat like this:

  \code
    QLineEdit *nameEd  = new QLineEdit( this );
    QLabel    *nameLb  = new QLabel( "&Name:", this );
    nameLb->setBuddy( nameEd );
    QLineEdit *phoneEd = new QLineEdit( this );
    QLabel    *phoneLb = new QLabel( "&Phone:", this );
    phoneLb->setBuddy( phoneEd );
    // ( layout setup not shown )
  \endcode

  With the code above, the focus jumps to the Name field when the user
  presses Alt-N, and to the Phone field when the user presses Alt-P.

  To unset a previously set buddy, call this function with \a buddy
  set to 0.

  \sa buddy(), setText(), QAccel, setAlignment()
*/

void QLabel::setBuddy( QWidget *buddy )
{
    if ( buddy )
	setAlignment( alignment() | ShowPrefix );
    else
	setAlignment( alignment() & ~ShowPrefix );

    if ( lbuddy )
	disconnect( lbuddy, SIGNAL(destroyed()), this, SLOT(buddyDied()) );

    lbuddy = buddy;

    if ( !lbuddy )
	return;

    int p = QAccel::shortcutKey( ltext );
    if ( p ) {
	if ( !accel )
	    accel = new QAccel( this, "accel label accel" );
	accel->connectItem( accel->insertItem( p ),
			    this, SLOT(acceleratorSlot()) );
    }

    connect( lbuddy, SIGNAL(destroyed()), this, SLOT(buddyDied()) );
}


/*!
  Returns the buddy of this label, or 0 if no buddy is currently set.

  \sa setBuddy()
*/

QWidget * QLabel::buddy() const
{
    return lbuddy;
}
#endif //QT_NO_ACCEL


#ifndef QT_NO_MOVIE
void QLabel::movieUpdated(const QRect& rect)
{
    QMovie *mov = movie();
    if ( mov && !mov->isNull() ) {
	QRect r = contentsRect();
	r = style().itemRect( 0, r.x(), r.y(), r.width(), r.height(),
			      align, isEnabled(), &(mov->framePixmap()),
			      QString::null );
	r.moveBy(rect.x(), rect.y());
	r.setWidth(QMIN(r.width(), rect.width()));
	r.setHeight(QMIN(r.height(), rect.height()));
	repaint( r, mov->framePixmap().mask() != 0 );
	if ( autoMask() )
	    updateMask();
    }
}

void QLabel::movieResized( const QSize& size )
{
    if ( autoresize )
	adjustSize();
    movieUpdated( QRect( QPoint(0,0), size ) );
    updateGeometry();
}

/*!
  Sets the label contents to \a movie. Any previous content is cleared.

  The buddy accelerator, if any, is disabled.

  The label resizes itself if auto-resizing is enabled.

  \sa movie(), setBuddy()
*/

void QLabel::setMovie( const QMovie& movie )
{
    QSize osh = sizeHint();
    clearContents();

    lmovie = new QMovie( movie );
	lmovie->connectResize(this, SLOT(movieResized(const QSize&)));
	lmovie->connectUpdate(this, SLOT(movieUpdated(const QRect&)));

    if ( !lmovie->running() )	// Assume that if the movie is running,
	updateLabel( osh );	// resize/update signals will come soon enough
}

#endif // QT_NO_MOVIE

/*!
  \internal

  Clears any contents, without updating/repainting the label.
*/

void QLabel::clearContents()
{
#ifndef QT_NO_RICHTEXT
    delete doc;
    doc = 0;
#endif

    delete lpixmap;
    lpixmap = 0;
    delete d->img;
    d->img = 0;
    delete d->pix;
    d->pix = 0;

    ltext = QString::null;
#ifndef QT_NO_ACCEL
    if ( accel )
	accel->clear();
#endif
#ifndef QT_NO_MOVIE
    if ( lmovie ) {
	lmovie->disconnectResize(this, SLOT(movieResized(const QSize&)));
	lmovie->disconnectUpdate(this, SLOT(movieUpdated(const QRect&)));
	delete lmovie;
	lmovie = 0;
    }
#endif
}


#ifndef QT_NO_MOVIE

/*!
  If the label contains a movie, returns a pointer to it. Otherwise,
  returns 0.

  \sa setMovie()
*/

QMovie* QLabel::movie() const
{
    return lmovie;
}

#endif  // QT_NO_MOVIE

/*!
  Returns the current text format.

  \sa setTextFormat()
*/

Qt::TextFormat QLabel::textFormat() const
{
    return textformat;
}

/*!
  Sets the text format to \a format. See the Qt::TextFormat enum for
  an explanation of the possible options.

  The default format is \c AutoText.

  \sa textFormat(), setText()
*/

void QLabel::setTextFormat( Qt::TextFormat format )
{
    if ( format != textformat ) {
    textformat = format;
    if ( !ltext.isEmpty() )
	updateLabel( QSize( -1, -1 ) );
    }
}

/*!
  \reimp
*/

void QLabel::fontChange( const QFont & )
{
    if ( !ltext.isEmpty() )
	updateLabel( QSize( -1, -1 ) );
}

#ifndef QT_NO_IMAGE_SMOOTHSCALE
/*!
  Returns whether the label will scale its contents to fill all
  available space.

  \sa setScaledContents()
 */
bool QLabel::hasScaledContents() const
{
    return scaledcontents;
}

/*!
  When called with \a enable == TRUE, and the label shows a pixmap,
  it will scale the pixmap to fill available space.

  \sa hasScaledContents()
 */
void QLabel::setScaledContents( bool enable )
{
    if ( (bool)scaledcontents == enable )
	return;
    scaledcontents = enable;
    if ( !enable ) {
	delete d->img;
	d->img = 0;
	delete d->pix;
	d->pix = 0;
    }
    if ( autoMask() )
	updateMask();
    update();
}
#endif // QT_NO_IMAGE_SMOOTHSCALE
#endif // QT_NO_LABEL
