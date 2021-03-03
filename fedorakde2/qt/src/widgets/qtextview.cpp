/****************************************************************************
** $Id: qt/src/widgets/qtextview.cpp   2.3.2   edited 2001-06-13 $
**
** Implementation of the QTextView class
**
** Created : 990101
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

#include "qtextview.h"
#ifndef QT_NO_TEXTVIEW
#include "../kernel/qrichtext_p.h"

#include "qapplication.h"
#include "qlayout.h"
#include "qpainter.h"

#include "qstack.h"
#include "stdio.h"
#include "qfile.h"
#include "qtextstream.h"
#include "qlayout.h"
#include "qbitmap.h"
#include "qtimer.h"
#include "qimage.h"
#include "qmime.h"
#include "qdragobject.h"
#include "qclipboard.h"
#include "qdragobject.h"




/*!
  \class QTextView qtextview.h
  \brief A sophisticated single-page rich text viewer.
  \ingroup basic
  \ingroup helpsystem

  Unlike QSimpleRichText, which merely draws small pieces of rich
  text, a QTextView is a real widget, with scrollbars when necessary,
  for showing large text documents.

  The rendering style and available tags are defined by a
  styleSheet(). Currently, a small XML/CSS1 subset including embedded
  images and tables is supported. See QStyleSheet for
  details. Possible images within the text document are resolved by
  using a QMimeSourceFactory.  See setMimeSourceFactory() for details.

  Using QTextView is quite similar to QLabel. It's mainly a call to
  setText() to set the contents. Setting the background color is
  slightly different from other widgets, since a text view is a
  scrollable widget that naturally provides a scrolling background. You
  can specify the colorgroup of the displayed text with
  setPaperColorGroup() or directly define the paper background with
  setPaper(). QTextView supports both plain color and complex pixmap
  backgrounds.

  Note that we do not intend to add a full-featured web browser widget
  to Qt (since that would easily double Qt's size and only few
  applications would benefit from it). In particular, the rich text
  support in Qt is supposed to provide a fast, portable and sufficient
  way to add reasonable online help facilities to applications. We
  will, however, extend it to some degree in future versions of Qt.

  For even more, like hypertext capabilities, see QTextBrowser.
*/

class QTextViewData
{
public:
    QStyleSheet* sheet_;
    QRichText* doc_;
    QMimeSourceFactory* factory_;
    QString original_txt;
    QString txt;
    QString contxt;
    QColorGroup mypapcolgrp;
    QColorGroup papcolgrp;
    QColor mylinkcol;
    QColor paplinkcol;
    bool linkunderline;
    QTimer* resizeTimer;
#ifndef QT_NO_DRAGANDDROP
    QTimer* dragTimer;
#endif
    QTimer* scrollTimer;
    Qt::TextFormat textformat;
    QRichTextFormatter* fcresize;
    QPoint cursor;
    QtTriple selorigin;
    QtTriple selstart;
    QtTriple selend;
    uint selection :1;
    uint dirty :1;
    uint dragselection :1;
    uint ownpalette : 1;
};


/*!
  Constructs an empty QTextView
  with the standard \a parent and \a name optional arguments.
*/
QTextView::QTextView(QWidget *parent, const char *name)
    : QScrollView( parent, name, WRepaintNoErase )
{
    init();
}


/*!
  Constructs a QTextView displaying the contents \a text with context
  \a context, with the standard \a parent and \a name optional
  arguments.
*/
QTextView::QTextView( const QString& text, const QString& context,
		      QWidget *parent, const char *name)
    : QScrollView( parent, name, WRepaintNoErase )
{
    init();
    setText( text, context );
}


void QTextView::init()
{
    d = new QTextViewData;
    d->mypapcolgrp = palette().active();
    d->papcolgrp = d->mypapcolgrp;
    d->mylinkcol = blue;
    d->paplinkcol = d->mylinkcol;
    d->linkunderline = TRUE;
    d->fcresize = 0;

    setKeyCompression( TRUE );
    setVScrollBarMode( QScrollView::Auto );
    setHScrollBarMode( QScrollView::Auto );

    d->doc_ = 0;
    d->sheet_ = 0;
    d->factory_ = 0;
    d->txt = QString::fromLatin1("<p></p>");
    d->textformat = AutoText;
    d->dirty = TRUE;
    d->selection = FALSE;
    d->dragselection = FALSE;
    d->ownpalette = FALSE;

    viewport()->setBackgroundMode( PaletteBase );
    viewport()->setFocusProxy( this );
    viewport()->setFocusPolicy( WheelFocus );

    d->resizeTimer = new QTimer( this, "qt_resizetimer" );
    connect( d->resizeTimer, SIGNAL( timeout() ), this, SLOT( doResize() ));
#ifndef QT_NO_DRAGANDDROP
    d->dragTimer = new QTimer( this );
    connect( d->dragTimer, SIGNAL( timeout() ), this, SLOT( doStartDrag() ));
#endif
    d->scrollTimer = new QTimer( this );
    connect( d->scrollTimer, SIGNAL( timeout() ), this, SLOT( doAutoScroll() ));
}

/*!
  Destructs the view.
*/
QTextView::~QTextView()
{
    delete d->fcresize;
    QTextFormatCollection* formats = d->doc_?d->doc_->formats:0;
    delete d->doc_;
    delete formats; //#### fix inheritance structure in rich text
    delete d;
}

/*!
  Changes the contents of the view to the string \a text and the
  context to \a context.

  \a text may be interpreted either as plain text or as rich text,
  depending on the textFormat(). The default setting is \c AutoText,
  i.e. the text view autodetects the format from \a text.

  The optional \a context is used to resolve references within the
  text document, for example image sources. It is passed directly to
  the mimeSourceFactory() when quering data.

  \sa text(), setTextFormat()
*/
void QTextView::setText( const QString& text, const QString& context)
{
    QTextFormatCollection* formats = d->doc_?d->doc_->formats:0;
    delete d->doc_;
    delete formats; //#### fix inheritance structure in rich text
    d->doc_ = 0;
    d->selection = FALSE;

    d->original_txt = text;
    d->contxt = context;

    if ( text.isEmpty() )
	d->txt = QString::fromLatin1("<p></p>");
    else if ( d->textformat == AutoText ) {
	if ( QStyleSheet::mightBeRichText( text ) )
	    d->txt = text;
	else
	    d->txt = QStyleSheet::convertFromPlainText( text );
    }
    else if ( d->textformat == PlainText )
	d->txt = QStyleSheet::convertFromPlainText( text );
    else // rich text
	d->txt = text;


    setContentsPos( 0, 0 );
    richText().invalidateLayout();
    richText().flow()->initialize( visibleWidth() );
    updateLayout();
    viewport()->update();
}

/*!\overload

  Changes the contents of the view to the string \a text.

  \a text may be interpreted either as plain text or as rich text,
  depending on the textFormat(). The default setting is \c AutoText,
  i.e. the text view autodetects the format from \a text.

  This function calls setText( text, QString::null ), i.e. it sets a
  text without any context.

  \sa text(), setTextFormat()
 */
void QTextView::setText( const QString& text )
{
    setText( text, QString::null );
}


/*!
  Appends \a text to the current text.

  Useful for log viewers.

  \warning This function has known problems (incorrect painting and
  layouting). If this problem occures to you, use setText( text() +
  theNewText ) instead. The new richtext engine, which is part of Qt
  3.0, is able to handle append(), insert(), etc. properly.
*/
void QTextView::append( const QString& text )
{
    richText().append( text,  mimeSourceFactory(), styleSheet() );
    if ( isVisible() ) {
	int y = contentsHeight();
	int h = richText().lastChild()->bottomMargin();
	if ( d->fcresize ) {
	    d->fcresize->updateLayout();
	    doResize();
	} else
	    updateLayout();
	updateContents( contentsX(), y-h, visibleWidth(), h );
    }
    d->original_txt += text;
}

/*!
  Returns the contents of the view.

  \sa context(), setText()
*/
QString QTextView::text() const
{
    return d->original_txt;
}

/*!
  Returns the context of the view.

  \sa text(), setText()
*/
QString QTextView::context() const
{
    return d->contxt;
}


void QTextView::createRichText()
{
    if ( d->mypapcolgrp != d->papcolgrp )
	viewport()->setBackgroundColor( d->mypapcolgrp.base() );
    d->papcolgrp = d->mypapcolgrp;
    d->paplinkcol = d->mylinkcol;

    d->doc_ = new QRichText( d->txt, viewport()->font(), d->contxt,
			     8, mimeSourceFactory(), styleSheet() );
    if (d->doc_->attributes().contains("bgcolor")){
	QColor  col ( d->doc_->attributes()["bgcolor"].latin1() );
	if ( col.isValid() ) {
	    d->papcolgrp.setColor( QColorGroup::Base, col );
	    viewport()->setBackgroundColor( col );
	}
    }
    if (d->doc_->attributes().contains("link")){
	QColor  col ( d->doc_->attributes()["link"].latin1() );
	if ( col.isValid() )
	    d->paplinkcol = col;
    }
    if (d->doc_->attributes().contains("text")){
	QColor  col ( d->doc_->attributes()["text"].latin1() );
	if ( col.isValid() )
	    d->papcolgrp.setColor( QColorGroup::Text,  col );
    }
    if (d->doc_->attributes().contains("background")){
	QString imageName = d->doc_->attributes()["background"];
	QPixmap pm;
	const QMimeSource* m =
	    context().isNull()
		? mimeSourceFactory()->data( imageName )
		: mimeSourceFactory()->data( imageName, context() );
	if ( m ) {
	    if ( !QImageDrag::decode( m, pm ) ) {
		qWarning("QTextImage: cannot load %s", imageName.latin1() );
	    }
	}
	if (!pm.isNull())
	    d->papcolgrp.setBrush( QColorGroup::Base, QBrush(d->papcolgrp.base(), pm) );
    }
    d->cursor = QPoint(0,0);
}


/*!
  Returns the current style sheet of the view.

  \sa setStyleSheet()
*/
QStyleSheet* QTextView::styleSheet() const
{
    if (!d->sheet_)
	return QStyleSheet::defaultSheet();
    else
	return d->sheet_;

}

/*!
  Sets the style sheet of the view.

  \sa styleSheet()
*/
void QTextView::setStyleSheet( QStyleSheet* styleSheet )
{
    d->sheet_ = styleSheet;
    viewport()->update();
}


/*!
  Returns the current mime source factory  for the view.

  \sa setMimeSourceFactory()
*/
QMimeSourceFactory* QTextView::mimeSourceFactory() const
{
    if (!d->factory_)
	return QMimeSourceFactory::defaultFactory();
    else
	return d->factory_;

}

/*!
  Sets the mime source factory for the view. The factory is used to
  resolve named references within rich text documents. If no factory
  has been specified, the text view uses the default factory
  QMimeSourceFactory::defaultFactory().

  Ownership of \a factory is \e not transferred to make it possible
  for several text view widgets to share the same mime source.

  \sa mimeSourceFactory()
*/
void QTextView::setMimeSourceFactory( QMimeSourceFactory* factory )
{
    d->factory_ = factory;
    viewport()->update();
}


/*!
  Sets the brush to use as the background to \a pap.

  This may be a nice parchment or marble pixmap or simply another
  plain color.

  Technically, setPaper() is just a convenience function to set the
  base brush of the paperColorGroup().

  \sa paper()
*/
void QTextView::setPaper( const QBrush& pap)
{
    d->mypapcolgrp.setBrush( QColorGroup::Base, pap );
    d->papcolgrp.setBrush( QColorGroup::Base, pap );
    d->ownpalette = TRUE;
    viewport()->setBackgroundColor( pap.color() );
    viewport()->update();
}

/*!
  Sets the full colorgroup of the paper to \a colgrp. If not specified
  otherwise in the document itself, any text will use
  QColorGroup::text(). The background will be painted with
  QColorGroup::brush(QColorGroup::Base).

  \sa paperColorGroup(), setPaper()
*/
void QTextView::setPaperColorGroup( const QColorGroup& colgrp)
{
    d->mypapcolgrp = colgrp;
    d->papcolgrp = colgrp;
    d->ownpalette = TRUE;
    viewport()->setBackgroundColor( colgrp.base() );
    viewport()->update();
}

/*!
  Returns the colorgroup of the paper.

  \sa setPaperColorGroup(), setPaper()
*/
const QColorGroup& QTextView::paperColorGroup() const
{
    return d->papcolgrp;
}

/*!
  Sets the color used to display links in the document to \c col.

  \sa linkColor()
 */
void QTextView::setLinkColor( const QColor& col )
{
    d->mylinkcol = col;
    d->paplinkcol = col;
}

/*!
  Returns the current link color.

  The color may either have been set with setLinkColor() or stem from
  the document's body tag.

  \sa setLinkColor()
 */
const QColor& QTextView::linkColor() const
{
    return d->paplinkcol;
}

/*!
  Defines whether or not links should be displayed underlined.
 */
void QTextView::setLinkUnderline( bool u)
{
    d->linkunderline = u;
}

/*!
  Returns whether or not links should be displayed underlined.
 */
bool QTextView::linkUnderline() const
{
    return d->linkunderline;
}


/*!
  Returns the document title parsed from the content.
*/
QString QTextView::documentTitle() const
{
    return richText().attributes()["title"];
}

/*!
  Returns the height of the view given a width of \a w.
*/
int QTextView::heightForWidth( int w ) const
{
    QRichText doc( d->txt, viewport()->font(), d->contxt,
		   8, mimeSourceFactory(), styleSheet() );
    doc.doLayout( 0, w );
    return doc.height;
}

/*!
  Returns the document defining the view as drawable and queryable rich
  text object.  This is not currently useful for applications.
*/
QRichText& QTextView::richText() const
{
    if (!d->doc_){
	QTextView* that = (QTextView*) this;
	that->createRichText();
    }
    return *d->doc_;
}

/*!
  Returns the brush used to paint the background.
*/
const QBrush& QTextView::paper()
{
    return d->papcolgrp.brush( QColorGroup::Base );
}

/*!
  Returns the brush used to paint the background.
*/
const QBrush& QTextView::paper() const
{
    return d->papcolgrp.brush( QColorGroup::Base );
}

/*!
  \reimp
*/
void QTextView::drawContentsOffset(QPainter* p, int ox, int oy,
				 int cx, int cy, int cw, int ch)
{
    if ( !d->ownpalette && d->mypapcolgrp == d->papcolgrp ) {
	d->mypapcolgrp = colorGroup();
	d->papcolgrp = d->mypapcolgrp;
    }
    QTextOptions to(&paper(), d->paplinkcol, d->linkunderline );
    to.offsetx = ox;
    to.offsety = oy;
    if ( d->selection ) {
	to.selstart = d->selstart;
	to.selend = d->selend;
    }

    QRegion r(cx-ox, cy-oy, cw, ch);

    QRichTextFormatter tc( richText() );
    tc.gotoParagraph( p, richText().getParBefore( cy ) );
    QTextParagraph* b = tc.paragraph;

    QFontMetrics fm( p->fontMetrics() );
    while ( b && tc.y() <= cy + ch ) {

	if ( b && b->dirty ) //ensure the paragraph is laid out
	    tc.updateLayout( p, cy + ch );

	tc.gotoParagraph( p, b );

	if ( tc.y() + tc.paragraph->height > cy ) {
	    do {
		tc.makeLineLayout( p );
		QRect geom( tc.lineGeometry() );
		if ( geom.bottom() > cy && geom.top() < cy+ch )
		    tc.drawLine( p, ox, oy, cx, cy, cw, ch, r, paperColorGroup(), to );
	    }
	    while ( tc.gotoNextLine( p ) );
	}
	b = b->nextInDocument();
    }

    to.selstart = QtTriple();
    to.selstart = to.selend;
    richText().flow()->drawFloatingItems( p, ox, oy, cx, cy, cw, ch, r, paperColorGroup(), to );

    p->setClipRegion(r);

    if ( paper().pixmap() )
	p->drawTiledPixmap(0, 0, visibleWidth(), visibleHeight(),
			   *paper().pixmap(), ox, oy);
    else
	p->fillRect(0, 0, visibleWidth(), visibleHeight(), paper() );

    p->setClipping( FALSE );

#if 0
    int pagesize = richText().flow()->pagesize;
    if ( pagesize > 0 ) {
	p->setPen( DotLine );
	for (int page = cy / pagesize; page <= (cy+ch) / pagesize; ++page ) {
	    p->drawLine( cx-ox, page * pagesize - oy, cx-ox+cw, page*
			 pagesize - oy );
	}
    }
#endif

}

/*!
  \reimp
*/
void QTextView::viewportResizeEvent(QResizeEvent* )
{
}

void QTextView::doResize()
{
    if ( !d->fcresize->updateLayout( 0, d->fcresize->y() + d->fcresize->paragraph->height + 1000 ) )
	d->resizeTimer->start( 0, TRUE );
    QTextFlow* flow = richText().flow();
    resizeContents( QMAX( flow->widthUsed-1, visibleWidth() ), flow->height );
}

/*!
  \reimp
*/
void QTextView::resizeEvent( QResizeEvent* e )
{
    setUpdatesEnabled( FALSE ); // to hinder qscrollview from showing/hiding scrollbars. Safe since we call resizeContents later!
    QScrollView::resizeEvent( e );
    setUpdatesEnabled( TRUE);
    richText().flow()->initialize( visibleWidth() );
    updateLayout();
}


/*!
  \reimp
*/
void QTextView::viewportMousePressEvent( QMouseEvent* e )
{
    if ( e->button() != LeftButton )
	return;
    d->cursor = e->pos() + QPoint( contentsX(), contentsY() );
    QRichTextIterator it( richText() );
    bool within = it.goTo( d->cursor );
    bool sel = d->selection && it.position() >= d->selstart && it.position() < d->selend;
    if ( !sel || !within ) {
	clearSelection();
	d->selorigin = it.position();
	d->selstart = d->selorigin;
	d->selend = d->selstart;
	d->dragselection = TRUE;
#ifndef QT_NO_DRAGANDDROP
    } else {
	d->dragTimer->start( QApplication::startDragTime(), TRUE );
#endif
    }
}

/*!
  \reimp
*/
void QTextView::viewportMouseReleaseEvent( QMouseEvent* e )
{
    if ( e->button() == LeftButton ) {
	d->scrollTimer->stop();
#ifndef QT_NO_CLIPBOARD
	if ( d->dragselection ) {
#if defined(_WS_X11_)
	    if ( style() == MotifStyle )
		copy();
#endif
	    d->dragselection = FALSE;
	} else
#endif
	{
	    clearSelection();
	}
    }
}



/*!  Returns TRUE if there is any text selected, FALSE otherwise.

  \sa selectedText()
*/
bool QTextView::hasSelectedText() const
{
    return d->selection;
}

/*!  Returns a copy of the selected text in plain text format.

  \sa hasSelectedText()
*/
QString QTextView::selectedText() const
{
    if ( !d->selection )
	return QString::null;

    QRichTextIterator it( richText() );
    it.goTo( d->selstart );

    if ( d->selstart.a == d->selend.a && d->selstart.b == d->selend.b )
	return it.text().mid( d->selstart.c, d->selend.c - d->selstart.c );

    int column = 0;
    QString txt;
    QString s = it.text().mid( d->selstart.c );
    while ( it.position() < d->selend ) {
	if ( !s.isEmpty() ) {
	    if ( column + s.length() > 79 &&
		it.outmostParagraph()->style->whiteSpaceMode() == QStyleSheetItem::WhiteSpaceNormal ) {
		txt += '\n';
		column = 0;
	    }
	    if ( s[(int)s.length()-1]== '\n' )
		column = 0;
	    txt += s;
	    column += s.length();
	}
	int oldpar = it.position().a;
	if ( !it.right( FALSE ) )
	    break;
	if ( it.position().a != oldpar ) {
	    txt += '\n';
	    column = 0;
	}
	s = it.text();
	if ( it.position().a == d->selend.a && it.position().b == d->selend.b )
	    s = s.left( d->selend.c );
    }
    return txt;
}

static int logicalFontSize( QStyleSheet* style, QFont base, int pt )
{
    for (int i=0; i<10; i++) {
	QFont b = base;
	style->scaleFont(b,i);
	if ( b.pointSize() >= pt )
	    return i;
    }
    return 1; // else what?
}

static QString formatDiff(const QTextView* view, QTextCharFormat* pfmt, QTextCharFormat* nfmt)
{
    QString txt;
    QFont basefont = view->font();
    if ( pfmt != nfmt ) {
	QString t,pre,post;
	if ( pfmt->color() != nfmt->color() ) {
	    QString c;
	    t += c.sprintf("color=#%06x ", nfmt->color().rgb());
	}
	if ( pfmt->font() != nfmt->font() ) {
	    int plsz = logicalFontSize( view->styleSheet(), basefont, pfmt->font().pointSize() );
	    int nlsz = logicalFontSize( view->styleSheet(), basefont, nfmt->font().pointSize() );
	    if ( nlsz != plsz ) {
		QString f;
		t += f.sprintf("size=%d ",nlsz-plsz);
	    }
	    if ( pfmt->font().family() != nfmt->font().family() ) {
		t += "face=";
		t += nfmt->font().family();
		t += " ";
	    }
	    if ( pfmt->font().italic() != nfmt->font().italic() ) {
		bool on = nfmt->font().italic();
		if ( on )
		    post = post + "<i>";
		else
		    pre = "</i>" + pre;
	    }
	    if ( pfmt->font().weight() != nfmt->font().weight() ) {
		bool on = nfmt->font().weight() > 50;
		if ( on )
		    post = post + "<b>";
		else
		    pre = "</b>" + pre;
	    }
	}
	txt += pre;
	if ( !t.isEmpty() ) {
	    t.truncate(t.length()-1); // chop space
	    txt += "<font " + t + ">";
	}
	txt += post;
    }
    return txt;
}

/*!  Returns a copy of the selected text in rich text format (XML).

  \sa hasSelectedText()
*/
QString QTextView::selectedRichTextInternal() const
{
    if ( !d->selection )
	return QString::null;

    QRichTextIterator it( richText() );
    it.goTo( d->selstart );
    QString txt;
    QString s = it.text().mid( d->selstart.c );
    QTextCharFormat ifmt;
    QTextCharFormat* pfmt = &ifmt;
    while ( it.position() < d->selend ) {
	QTextCharFormat* nfmt = it.format();
	txt += formatDiff(this,pfmt,nfmt);
	pfmt = nfmt;
	txt += s;
	int oldpar = it.position().a;
	if ( !it.right( FALSE ) )
	    break;
	if ( it.position().a != oldpar )
	    txt += "</p><p>";
	s = it.text();
	if ( it.position().a == d->selend.a && it.position().b == d->selend.b )
	    s = s.left( d->selend.c );
    }
    txt += formatDiff(this,pfmt,&ifmt);
    return txt;
}

#ifndef QT_NO_CLIPBOARD
/*!
  Copies the marked text to the clipboard.
*/
void QTextView::copy()
{
#if defined(_WS_X11_)
    disconnect( QApplication::clipboard(), SIGNAL(dataChanged()), this, 0);
#endif
    QString t = selectedText();
    QRegExp nbsp(QChar(0x00a0U));
    t.replace( nbsp, " " );
#if defined(_OS_WIN32_)
    // Need to convert NL to CRLF
    QRegExp nl("\\n");
    t.replace( nl, "\r\n" );
#endif
    QApplication::clipboard()->setText( t );
#if defined(_WS_X11_)
    connect( QApplication::clipboard(), SIGNAL(dataChanged()),
	     this, SLOT(clipboardChanged()) );
#endif
}
#endif

/*!
  Selects all text.
*/
void QTextView::selectAll()
{
    QRichTextIterator it( richText() );
    d->selstart = it.position();
    while ( it.right( FALSE ) ) { }
    d->selend = it.position();
    viewport()->update();
    d->selection = TRUE;
#if defined(_WS_X11_)
    copy();
#endif
}

/*!
  \reimp
*/
void QTextView::viewportMouseMoveEvent( QMouseEvent* e)
{
    if (e->state() & LeftButton ) {
	if (d->dragselection ) {
	    doSelection( e->pos() );
	    ensureVisible( d->cursor.x(), d->cursor.y() );
#ifndef QT_NO_DRAGANDDROP
	} else if ( d->dragTimer->isActive() ) {
	    d->dragTimer->stop();
	    doStartDrag();
#endif
	}
    }
}

/*!
  Provides scrolling and paging.
*/
void QTextView::keyPressEvent( QKeyEvent * e)
{
    int unknown = 0;
    switch (e->key()) {
    case Key_Right:
	scrollBy( 10, 0 );
	break;
    case Key_Left:
	scrollBy( -10, 0 );
	break;
    case Key_Up:
	scrollBy( 0, -10 );
	break;
    case Key_Down:
	scrollBy( 0, 10 );
	break;
    case Key_Home:
	setContentsPos(0,0);
	break;
    case Key_End:
	setContentsPos(0,contentsHeight()-visibleHeight());
	break;
    case Key_PageUp:
	scrollBy( 0, -visibleHeight() );
	break;
    case Key_PageDown:
	scrollBy( 0, visibleHeight() );
	break;
#ifndef QT_NO_CLIPBOARD
    case Key_F16: // Copy key on Sun keyboards
	copy();
	break;
#if defined (_WS_WIN_)
    case Key_Insert:
#endif
    case Key_C:
	if ( e->state() & ControlButton )
	    copy();
	break;
#endif
    default:
	unknown++;
    }
    if ( unknown )				// unknown key
	e->ignore();
}

/*!
  \reimp
*/
void QTextView::paletteChange( const QPalette & p )
{
    QScrollView::paletteChange( p );
    if ( !d->ownpalette ) {
	d->mypapcolgrp = palette().active();
	d->papcolgrp = d->mypapcolgrp;
    }
}


/*!
  Returns the current text format.

  \sa setTextFormat()
 */
Qt::TextFormat QTextView::textFormat() const
{
    return d->textformat;
}

/*!
  Sets the text format to \a format. Possible choices are
  <ul>
  <li> \c PlainText - all characters are displayed verbatim,
  including all blanks and linebreaks.
  <li> \c RichText - rich text rendering. The available
  styles are defined in the default stylesheet
  QStyleSheet::defaultSheet().
  <li> \c AutoText - this is also the default. The label
  autodetects which rendering style suits best, \c PlainText
  or \c RichText. Technically, this is done by using the
  QStyleSheet::mightBeRichText() heuristic.
  </ul>
 */
void QTextView::setTextFormat( Qt::TextFormat format )
{
    d->textformat = format;
    setText( d->original_txt, d->contxt ); // trigger update
}


/*!\internal
 */
void QTextView::updateLayout()
{
    if ( !isVisible() ) {
	d->dirty = TRUE;
	return;
    }

    QSize cs( viewportSize( contentsWidth(), contentsHeight() ) );
    int ymax = contentsY() + cs.height() + 1;

    delete d->fcresize;
    d->fcresize = new QRichTextFormatter( richText() );
    d->fcresize->initParagraph( 0, &richText() );
    d->fcresize->updateLayout( 0, ymax );

    QTextFlow* flow = richText().flow();
    QSize vs( viewportSize( flow->widthUsed, flow->height ) );

    if ( vs.width() != visibleWidth() ) {
	flow->initialize( vs.width() );
	richText().invalidateLayout();
	d->fcresize->gotoParagraph( 0, &richText() );
	d->fcresize->updateLayout( 0, ymax );
    }

    resizeContents( QMAX( flow->widthUsed-1, vs.width() ), flow->height );
    d->resizeTimer->start( 0, TRUE );
    d->dirty = FALSE;
}


/*!\reimp
 */
void QTextView::showEvent( QShowEvent* )
{
    if ( d->dirty )
	updateLayout();
}

void QTextView::clearSelection()
{
#ifndef QT_NO_DRAGANDDROP
    d->dragTimer->stop();
#endif
    if ( !d->selection )
	return; // nothing to do
    d->selection = FALSE;
    QRichTextIterator it( richText() );
    it.goTo( d->selend );
    int y = it.lineGeometry().bottom();
    it.goTo( d->selstart );
    if ( y - it.lineGeometry().top()  >= visibleHeight() )
	viewport()->update();
    else {
	QRect r = it.lineGeometry();
	while ( it.position() < d->selend && it.right() ) {
	    r = r.unite( it.lineGeometry() );
	}
	updateContents( r );
    }
}

#ifndef QT_NO_DRAGANDDROP
void QTextView::doStartDrag()
{
    QTextDrag* drag = new QTextDrag( selectedText(), this ) ;
    drag->drag();
}
#endif

void QTextView::doAutoScroll()
{
    QPoint pos = viewport()->mapFromGlobal( QCursor::pos() );
    if ( pos.y() < 0 )
	scrollBy( 0, -32 );
    else if (pos.y() > visibleHeight() )
	scrollBy( 0, 32 );
    doSelection( pos );
}

void QTextView::doSelection( const QPoint& pos )
{
    QPoint to( pos + QPoint( contentsX(), contentsY()  ) );
    if ( to != d->cursor ) {
	QRichTextIterator it( richText() );
	it.goTo( to );
	d->selection = TRUE;
	if ( (it.position() != d->selstart) && (it.position()  != d->selend) ) {
	    if ( it.position() < d->selorigin ) {
		d->selstart = it.position();
		d->selend = d->selorigin;
	    } else {
		d->selstart = d->selorigin;
		d->selend = it.position();
	    }
	    QRichTextIterator it2( richText() );
	    it2.goTo( d->cursor );
	    QRect r = it2.lineGeometry();
	    r = r.unite( it.lineGeometry() );
	    while ( it.position() < it2.position() && it.right( FALSE ) )
		r = r.unite( it.lineGeometry() );
	    while ( it2.position() < it.position() && it2.right( FALSE ) )
		r = r.unite( it2.lineGeometry() );
	    d->cursor = to;
	    repaintContents( r, FALSE );
	}
    }

    if ( pos.y() < 0 || pos.y() > visibleHeight() )
	d->scrollTimer->start( 100, FALSE );
    else
	d->scrollTimer->stop();
}

void QTextView::clipboardChanged()
{
#if defined(_WS_X11_)
    disconnect( QApplication::clipboard(), SIGNAL(dataChanged()),
		this, SLOT(clipboardChanged()) );
    clearSelection();
#endif
}

/*!\reimp
 */
void QTextView::focusInEvent( QFocusEvent * )
{
    setMicroFocusHint(width()/2, 0, 1, height(), FALSE);
}

/*!\reimp
 */
void QTextView::focusOutEvent( QFocusEvent * )
{
}
#endif  // QT_NO_TEXTVIEW
