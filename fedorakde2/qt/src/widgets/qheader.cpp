/****************************************************************************
** $Id: qt/src/widgets/qheader.cpp   2.3.2   edited 2001-07-04 $
**
** Implementation of QHeader widget class (table header)
**
** Created : 961105
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

#include "qheader.h"
#ifndef QT_NO_HEADER
#include "qpainter.h"
#include "qdrawutil.h"
#include "qbitmap.h"
#include "qbitarray.h"
#include "qvector.h"

static const int GRIPMARGIN  = 4;		//half the size of the resize area
static const int MARKSIZE = 32;
static const int QH_MARGIN = 4;


struct QHeaderData
{
    QHeaderData(int n)
    {
	count = n;
	labels.setAutoDelete( TRUE );
	iconsets.setAutoDelete( TRUE );
	sizes.resize(n);
	positions.resize(n);
	heights.resize(n);
	labels.resize(n);
	if ( int( iconsets.size() ) < n )
	    iconsets.resize( n );
	i2s.resize(n);
	s2i.resize(n);
	clicks.resize(n);
	resize.resize(n);
	int p =0;
	for ( int i = 0; i < n ; i ++ ) {
	    sizes[i] = 88;
	    // heights[i] = 10; set properly in QHeader::init()
	    i2s[i] = i;
	    s2i[i] = i;
	    positions[i] = p;
	    p += sizes[i];
	}
	clicks_default = TRUE;
	resize_default = TRUE;
	clicks.fill( clicks_default );
	resize.fill( resize_default );
	move = TRUE;
	sortColumn = -1;
	sortDirection = TRUE;
	positionsDirty = TRUE;
    }


    QArray<QCOORD>	sizes;
    QArray<QCOORD>	heights;
    QArray<QCOORD>	positions; // sorted by index
    QVector<QString>	labels;
    QVector<QIconSet>	iconsets;
    QArray<int>	        i2s;
    QArray<int>	        s2i;

    QBitArray           clicks;
    QBitArray           resize;
    uint move : 1;
    uint clicks_default : 1; // default value for new clicks bits
    uint resize_default : 1; // default value for new resize bits
    bool sortDirection;
    int sortColumn;
    int count;
    bool positionsDirty;

    void calculatePositions(){
	// positions is sorted by index, not by section
	positionsDirty = FALSE;
	int p = 0;
	for ( int i = 0; i < count; i++ ) {
	    positions[i] = p;
	    p +=sizes[i2s[i]];
	}
    }
    int sectionAt( int pos ) {
	// positions is sorted by index, not by section
	if ( !count )
	    return -1;
	int l = 0;
	int r = count - 1;
	int i = ( (l+r+1) / 2 );
	while ( r - l ) {
	    if ( positions[i] > pos )
		r = i -1;
	    else
		l = i;
	    i = ( (l+r+1) / 2 );
	}
	if ( positions[i] <= pos && pos <= positions[i] + sizes[ i2s[i] ] )
	    return i2s[i];
	return -1;
    }
};


// BEING REVISED: eiriken
/*!
  \class QHeader qheader.h
  \brief The QHeader class provides a table header.
  \ingroup advanced

  This class provides a table header as known from spreadsheet-like widgets.
  QHeader can be used vertically or horizontally (see setOrientation()).

  With addLabel() you can add sections, and with removeLabel() you can remove
  them. If you enabled clicking for one or all sections (see setClickEnabled()),
  the user can reorder the sections and click on them which may be used for
  sorting (see also setSortIndicator()). This feature is turned on by default.

  So, if the user reorders the sections by clicking and moving them with the mouse the index
  of a section may change. This means, the section you inserted at the first
  position might be displayed at a different index then. To get the index at which e.g
  the first section is displayed, use mapToIndex() with 0 as argument for our example.

  If you want e.g. to know which section is displayed at e.g. index 3 use
  mapToSection() with 3 as argument.

  So, you can always work with the section numbers as you inserted them
  without caring about the index at which they are displayed at the moment. Also
  the API of QHeader works with the section numbers.

  <img src=qheader-m.png> <img src=qheader-w.png>

  \sa QListView QTableView
 */



/*!
  Constructs a horizontal header.

  The \e parent and \e name arguments are sent to the QWidget constructor.
*/

QHeader::QHeader( QWidget *parent, const char *name )
    : QWidget( parent, name, WNorthWestGravity )
{
    orient = Horizontal;
    init( 0 );
}

/*!
  Constructs a horizontal header with \a n sections.

  The \e parent and \e name arguments are sent to the QWidget constructor.

*/

QHeader::QHeader( int n,  QWidget *parent, const char *name )
    : QWidget( parent, name, WNorthWestGravity )
{
    orient = Horizontal;
    init( n );
}

/*!
  Destructs the header.
 */

QHeader::~QHeader()
{
    delete d;
    d = 0;
}

/*! \reimp
 */

void QHeader::showEvent( QShowEvent *e )
{
    d->calculatePositions();
    QWidget::showEvent( e );
}

/*!
  \fn void QHeader::sizeChange( int section, int oldSize, int newSize )

  This signal is emitted when the user has changed the size of some
  of a \a section of the header from \a oldSize to \a newSize. This signal is
  typically connected to a slot that repaints the table.
*/

/*!
  \fn void QHeader::clicked( int section )

  This signal is emitted when the user clicked onto the section
  \a section.

  \sa pressed(), released()
*/

/*!
  \fn void QHeader::pressed( int section )

  This signal is emitted when the user presses section \a section down.

  \sa released()
*/

/*!
  \fn void QHeader::released( int section )

  This signal is emitted when section \a section is released.

  \sa pressed()
*/


/*!
  \fn void QHeader::indexChange( int section, int fromIndex, int toIndex )

  This signal is emitted if the user moved the section \a section, which
  was displayed at the index \a fromIndex to the new index \a toIndex.
*/

/*!
  \fn void QHeader::moved( int fromIndex, int toIndex )
  \obsolete

  Use indexChange() instead!

  This signal is emitted when the user has moved the section which
  is displayed at the index \a fromIndex to the index \a toIndex.
*/

/*!
  \fn void QHeader::sectionClicked( int index )
  \obsolete

  Use clicked() instead!

  This signal is emitted when a part of the header is clicked. \a
  index is the index at which the section is displayed.

  In a list view, this signal would typically be connected to a slot
  which sorts the specified column (or row).
*/

/*!
  \obsolete

  Use sectionSize() instead!

  Returns the size in pixels of the section which is displayed at
  the index \a i.
*/

int QHeader::cellSize( int i ) const
{
    return sectionSize( mapToSection(i) );
}

/*!
  \obsolete

  Use sectionPos() instead!

  Returns the position in pixels of the section which is displayed at the
  index \a i. The  position is measured from the start of the header.
*/

int QHeader::cellPos( int i ) const
{
    if ( i == count() && i > 0 )
	return d->positions[i-1] + d->sizes[d->i2s[i-1]]; // compatibility
    return sectionPos( mapToSection(i) );
}


/*!
  Returns the number of sections in the header.
*/

int QHeader::count() const
{
    return d->count;
}



/*!
  \fn Orientation QHeader::orientation() const

  Returns \c Horizontal if the header is horizontal, \c Vertical if
  the header is vertical.

  */

/*!
  \fn void QHeader::setTracking( bool enable )

  Sets tracking if \a enable is TRUE, otherwise turns off tracking.
  If tracking is on, the sizeChange() signal is emitted continuously
  while the mouse is moved, otherwise it is only emitted when the
  mouse button is released.

  \sa tracking()
  */

/*!
  \fn bool QHeader::tracking() const

  Returns TRUE if tracking is on, FALSE otherwise.

  \sa setTracking()
  */

/*!
  Initializes with \a n columns.
 */
void QHeader::init( int n )
{
    state = Idle;
    offs = 0;
    cachedIdx = 0; // unused
    cachedPos = 0; // unused
    d = new QHeaderData(n);
    for ( int i = 0; i < n ; i ++ ) {
	d->heights[i] = fontMetrics().lineSpacing()+6;
    }
    handleIdx = 0;

    setMouseTracking( TRUE );
    trackingIsOn = FALSE;
    setBackgroundMode( PaletteButton );
    setSizePolicy( QSizePolicy( QSizePolicy::Preferred, QSizePolicy::Fixed ) );
}

/*!
  Sets the header orientation.  The \e orientation must be
  QHeader::Vertical or QHeader::Horizontal.

  When adding labels without the size parameter, setOrientation
  should be called first, otherwise labels will be sized incorrectly.
  \sa orientation()
*/

void QHeader::setOrientation( Orientation orientation )
{
    if (orient==orientation) return;
    orient = orientation;
    if ( orient == Horizontal )
	setSizePolicy( QSizePolicy( QSizePolicy::Preferred, QSizePolicy::Fixed ) );
    else
	setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Preferred ) );
    update();
    updateGeometry();
}


/*!
  Paints a rectangle starting at \a p, with length \s.
  */
void QHeader::paintRect( int p, int s )
{
    QPainter paint( this );
    paint.setPen( QPen( black, 1, DotLine ) );
    if ( orient == Horizontal )
	paint.drawRect( p, 3, s, height() - 5 );
    else
	paint.drawRect( 3, p, height() - 5, s );
}

/*!
  Marks the division line at \a idx.
  */
void QHeader::markLine( int idx )
{
    QPainter paint( this );
    paint.setPen( QPen( black, 1, DotLine ) );
    int p = pPos( idx );
    int x = p - MARKSIZE/2;
    int y = 2;
    int x2 = p + MARKSIZE/2;
    int y2 = height() - 3;
    if ( orient == Vertical ) {
	int t = x; x = y; y = t;
	t = x2; x2 = y2; y2 = t;
    }

    paint.drawLine( x, y, x2, y );
    paint.drawLine( x, y+1, x2, y+1 );

    paint.drawLine( x, y2, x2, y2 );
    paint.drawLine( x, y2-1, x2, y2-1 );

    paint.drawLine( x, y, x, y2 );
    paint.drawLine( x+1, y, x+1, y2 );

    paint.drawLine( x2, y, x2, y2 );
    paint.drawLine( x2-1, y, x2-1, y2 );
}

/*!
  Removes the mark at the division line at \a idx.
  */
void QHeader::unMarkLine( int idx )
{
    if ( idx < 0 )
	return;
    int p = pPos( idx );
    int x = p - MARKSIZE/2;
    int y = 2;
    int x2 = p + MARKSIZE/2;
    int y2 = height() - 3;
    if ( orient == Vertical ) {
	int t = x; x = y; y = t;
	t = x2; x2 = y2; y2 = t;
    }
    repaint( x, y, x2-x+1, y2-y+1 );
}

/*!
  \obsolete

  Use sectionAt() instead!

  Returns the index at which the section is displayed, which contains
  \a pos in widget coordinates, or -1 if \a pos is outside the header
  sections.
*/

int QHeader::cellAt( int pos ) const
{
    return mapToIndex( sectionAt(pos + offset()) );
}


/*!
  Tries to find a line that is not a neighbor of  \c handleIdx.
 */
int QHeader::findLine( int c )
{
    int lastpos = d->positions[d->count-1] + d->sizes[d->i2s[d->count-1]];
    int i = 0;
    if ( c > lastpos ) {
	return d->count;
    } else {
	int section = sectionAt( c );
	if ( section < 0 )
	    return handleIdx;
	i = d->s2i[section];
    }
    if ( i == handleIdx )
	return i;
    if ( i == handleIdx - 1 &&  pPos( handleIdx ) - c > MARKSIZE/2 )
	return i;
    if ( i == handleIdx + 1 && c - pPos( i ) > MARKSIZE/2 )
	return i + 1;
    if ( c - pPos( i ) > pSize( i ) / 2 )
	return i + 1;
    else
	return i;
}

/*!
  \obsolete

  Use moveSection() instead!

  Moves the section which is currently displayed at the index \a fromIndex
  to the index \a toIdx.
*/

void QHeader::moveCell( int fromIdx, int toIdx )
{
    moveSection( mapToSection(fromIdx), toIdx );
}

/*!
  \reimp
*/
void QHeader::mousePressEvent( QMouseEvent *e )
{
    if ( e->button() != LeftButton || state != Idle )
	return;
    handleIdx = 0;
    int c = orient == Horizontal ? e->pos().x() : e->pos().y();
    c += offset();

    int section = sectionAt( c );
    if ( section < 0 )
	return;
    int index = d->s2i[section];

    if ( (index > 0 && c < d->positions[index] + GRIPMARGIN) ||
	 (c > d->positions[index] + d->sizes[section] - GRIPMARGIN) ) {
	if ( c < d->positions[index]  + GRIPMARGIN )
	    handleIdx = index-1;
	else
	    handleIdx = index;
	oldHIdxSize = d->sizes[ d->i2s[handleIdx] ];
	state = d->resize[d->i2s[handleIdx]  ] ? Sliding : Blocked;
    } else if ( index >= 0 ) {
	handleIdx = index;
	moveToIdx = -1;
	state = d->clicks[ d->i2s[handleIdx]  ] ? Pressed : Blocked;
	clickPos = c;
	repaint( sRect( handleIdx ) );
	emit pressed( section );
    }
}

/*!
  \reimp
*/
void QHeader::mouseReleaseEvent( QMouseEvent *e )
{
    if ( e->button() != LeftButton )
	return;
    State oldState = state;
    state = Idle;
    switch ( oldState ) {
    case Pressed: {
	int section = d->i2s[handleIdx];
	repaint(sRect( handleIdx ), FALSE);
	emit released( section );
	if ( sRect( handleIdx ).contains( e->pos() ) ) {
	    emit sectionClicked( handleIdx );
	    emit clicked( section );
	}
	} break;
    case Sliding: {
	int c = orient == Horizontal ? e->pos().x() : e->pos().y();
	c += offset();
	handleColumnResize( handleIdx, c, TRUE );
	} break;
    case Moving: {
#ifndef QT_NO_CURSOR
	unsetCursor();
#endif
	int section = d->i2s[handleIdx];
	if ( handleIdx != moveToIdx && moveToIdx != -1 ) {
	    moveSection( section, moveToIdx );
	    repaint(); // a bit overkill, but removes the handle as well
	    emit moved( handleIdx, moveToIdx );
	    emit indexChange( section, handleIdx, moveToIdx );
	    emit released( section );
	} else {
	    repaint(sRect( handleIdx ), FALSE );
	    if ( sRect( handleIdx).contains( e->pos() ) ) {
		emit released( section );
		emit sectionClicked( handleIdx );
		emit clicked( section );
	    }
	}
	break;
    }
    case Blocked:
	//nothing
	break;
    default:
	// empty, probably.  Idle, at any rate.
	break;
    }
}

/*!
  \reimp
*/
void QHeader::mouseMoveEvent( QMouseEvent *e )
{
    int section;
    bool hit;

    int c = orient == Horizontal ? e->pos().x() : e->pos().y();
    c += offset();

    switch( state ) {
    case Idle:
	hit = FALSE;
	if ( (section = sectionAt( c )) >= 0 ) {
	    int index = d->s2i[section];
	    if ( (index > 0 && c < d->positions[index] + GRIPMARGIN) ||
		 (c > d->positions[index] + d->sizes[section] - GRIPMARGIN) ) {
		if ( index > 0 && c < d->positions[index]  + GRIPMARGIN )
		    section = d->i2s[--index];
		if ( d->resize.testBit(section) ) {
		    hit = TRUE;
#ifndef QT_NO_CURSOR
		    if ( orient == Horizontal )
			setCursor( splitHCursor );
		    else
			setCursor( splitVCursor );
#endif
		}
	    }
	}
#ifndef QT_NO_CURSOR
	if ( !hit )
	    unsetCursor();
#endif
	break;
    case Blocked:
	break;
    case Pressed:
	if ( QABS( c - clickPos ) > 4 && d->move ) {
	    state = Moving;
	    moveToIdx = -1;
#ifndef QT_NO_CURSOR
	    if ( orient == Horizontal )
		setCursor( sizeHorCursor );
	    else
		setCursor( sizeVerCursor );
#endif
	}
	break;
    case Sliding:
	handleColumnResize( handleIdx, c, FALSE );
	break;
    case Moving: {
	int newPos = findLine( c );
	if ( newPos != moveToIdx ) {
	    if ( moveToIdx == handleIdx || moveToIdx == handleIdx + 1 )
		repaint( sRect(handleIdx) );
	    else
		unMarkLine( moveToIdx );
	    moveToIdx = newPos;
	    if ( moveToIdx == handleIdx || moveToIdx == handleIdx + 1 )
		paintRect( pPos( handleIdx ), pSize( handleIdx ) );
	    else
		markLine( moveToIdx );
	}
	break;
    }
    default:
	qWarning( "QHeader::mouseMoveEvent: (%s) unknown state", name() );
	break;
    }
}

/*!
  Handles resizing of sections. This means it redraws the relevant parts
  of the header.
*/

void QHeader::handleColumnResize( int index, int c, bool final )
{
    int section = d->i2s[index];
    int lim = d->positions[index] +  2*GRIPMARGIN;
    if ( c == lim ) return;
    if ( c < lim ) c = lim;
    int oldSize = d->sizes[section];
    int newSize = c - d->positions[index];
    d->sizes[section] = newSize;

    d->calculatePositions();

    int pos = d->positions[index]-offset();
    if ( orient == Horizontal )
	repaint( pos, 0, width() - pos, height() );
    else
	repaint( 0, pos, width(), height() - pos );

    if ( tracking() && oldSize != newSize )
	emit sizeChange( section, oldSize, newSize );
    else if ( !tracking() && final && oldHIdxSize != newSize )
	emit sizeChange( section, oldHIdxSize, newSize );
}

/*!
  Returns the rectangle covered by index \a index.
*/

QRect QHeader::sRect( int index )
{

    int section = mapToSection( index );
    if ( section < 0 )
	return rect(); // ### eeeeevil

    if ( orient == Horizontal )
	return QRect(  d->positions[index]-offset(), 0, d->sizes[section], height() );
    else
	return QRect( 0, d->positions[index]-offset(), width(), d->sizes[section] );
}


/*!
  Sets the icon on the section \a section to \a iconset and the text to \a s.
  If the section does not exist, nothing happens.
  If \a size is non-negative, the section width is set to \a size.

  Any icon set that has been defined for this section remains
  unchanged.
*/

void QHeader::setLabel( int section, const QIconSet& iconset, const QString &s, int size )
{
    if ( section < 0 || section >= count() )
	return;
    d->iconsets.insert( section, new QIconSet( iconset ) );
    setLabel( section, s, size );
}

/*!
  Sets the text on section \a section to \a s. If the section does not exist,
  nothing happens.
  If \a size is non-negative, the section width is set to \a size.

  Any icon set that has been defined for this section remains
  unchanged.
*/

void QHeader::setLabel( int section, const QString &s, int size )
{
    if ( section < 0 || section >= count() )
	return;
    d->labels.insert( section, new QString( s ) );
    if ( size >= 0 )
	d->sizes[section] = size;
    if ( isUpdatesEnabled() ) {
	d->calculatePositions();
	update();
    }
}


/*!
  Returns the text set on section \a section.
*/

QString QHeader::label( int section ) const
{
    if ( section < 0 || section >= count() )
	return QString::null;
    if ( d->labels[ section ] )
	return *( d->labels[ section ] );
    else
	return QString::null;
}

/*!
  Returns the icon set set on section \a section.
*/

QIconSet *QHeader::iconSet( int section ) const
{
    if ( section < 0 || section >= count() )
	return 0;
    return d->iconsets[ section ];
}


/*!
  Adds a new section, with icon set \a iconset and label text \a
  s. Returns the index.  If \a size is non-negative, the section width
  is set to \a size, otherwise a size currently sufficient for the
  label is used.
*/
int QHeader::addLabel( const QIconSet& iconset, const QString &s, int size )
{
    int n = count() + 1;
    d->iconsets.resize( n + 1 );
    d->iconsets.insert( n - 1, new QIconSet( iconset ) );
    return addLabel( s, size );
}

/*!
  Removes the section \a section.
*/
void QHeader::removeLabel( int section )
{
    if ( section < 0 || section > count() - 1 )
	return;

    int index = d->s2i[section];
    int n = --d->count;
    int i;
    for ( i = section; i < n; ++i ) {
	d->sizes[i] = d->sizes[i+1];
	d->heights[i] = d->heights[i+1];
	d->labels.insert( i, d->labels.take( i + 1 ) );
	d->iconsets.insert( i, d->iconsets.take( i + 1 ) );
    }

    d->sizes.resize( n );
    d->positions.resize( n );
    d->heights.resize( n );
    d->labels.resize( n );
    d->iconsets.resize( n );

    for ( i = section; i < n; ++i )
	d->s2i[i] = d->s2i[i+1];
    d->s2i.resize( n  );

    if ( isUpdatesEnabled() ) {
	for ( i = 0; i < n; ++i )
	    if ( d->s2i[i] > index )
		--d->s2i[i];
    }

    for ( i = index; i < n; ++i )
	d->i2s[i] = d->i2s[i+1];
    d->i2s.resize( n );

    if ( isUpdatesEnabled() ) {
	for ( i = 0; i < n ; ++i )
	    if ( d->i2s[i] > section )
		--d->i2s[i];
    }

    if ( isUpdatesEnabled() ) {
	d->calculatePositions();
	update();
    }
}



/*!
  Adds a new section, with label text \a s. Returns the index.
  If \a size is non-negative, the section width is set to \a size,
  otherwise a size currently sufficient for the label text is used.
*/

int QHeader::addLabel( const QString &s, int size )
{
    int lastpos = 0;
    if ( d->count )
	lastpos = d->positions[d->count-1] + d->sizes[d->i2s[d->count-1]];
    int n = ++d->count;
    d->labels.resize( n );
    if ( int( d->iconsets.size() ) < n  )
	d->iconsets.resize( n );
    d->sizes.resize( n );
    d->positions.resize( n );
    d->heights.resize( n );
    int section = n - 1;
    d->labels.insert( section, new QString( s ) );  // n-1 is last real idx
    int iw = 0;
    int ih = 0;
    if ( d->iconsets[section] != 0 ) {
	iw = d->iconsets[section]->pixmap( QIconSet::Small, QIconSet::Normal ).width() + 2;
	ih = d->iconsets[section]->pixmap( QIconSet::Small, QIconSet::Normal ).height();
    }

    QFontMetrics fm = fontMetrics();
    int height = QMAX( fm.lineSpacing() + 6, ih );
    int width = fm.boundingRect( s ).width()+ QH_MARGIN * 3 + iw;

    if ( size < 0 ) {
	if ( orient == Horizontal )
	    size = width;
	else
	    size = height;
    }

    int index = section;
    d->sizes[section] = size;
    d->positions[index] = lastpos;
    // we abuse the heights as widths for vertical layout
    d->heights[section] = orient == Horizontal ? height : width;

    d->i2s.resize( n );
    d->s2i.resize( n );
    d->s2i[section] = index;
    d->i2s[index] = section;
    d->clicks.resize( n );
    d->resize.resize( n );
    d->clicks.setBit( section, d->clicks_default );
    d->resize.setBit( section, d->resize_default );

    if ( isUpdatesEnabled() )
	update();
    return index;
}


/*!\reimp
*/
QSize QHeader::sizeHint() const
{
    constPolish();
    QFontMetrics fm = fontMetrics();
    if ( orient == Horizontal ) {
	int height = fm.lineSpacing() + 6;
	int width = 0;
	for ( int i=0 ; i<count() ; i++ ) {
	    height = QMAX( height , d->heights[i] );
	    width += d->sizes[i];
	}
	return QSize( width, height );
    }
    else {
	int width = fm.width( ' ' );
	int height = 0;
	for ( int i=0 ; i<count() ; i++ ) {
	    width = QMAX( width , d->heights[i] );
	    height += d->sizes[i];
	}
	return QSize( width, height );
    }
}

/*!\reimp
*/
QSizePolicy QHeader::sizePolicy() const
{
    //### removeme 3.0
    return QWidget::sizePolicy();
}


/*!
  Scrolls the header such that \a x becomes the leftmost (or uppermost
  for vertical headers) visible pixel.
*/

void QHeader::setOffset( int x )
{
    int oldOff = offs;
    offs = x;
    if ( orient == Horizontal )
	scroll( oldOff-offs, 0 );
    else
	scroll( 0, oldOff-offs);
}



/*!

  Returns the position of actual division line \a i in widget
  coordinates. May return a position outside the widget.

  Note that the last division line is numbered count(). (There are one more lines than
  sections).
 */
int QHeader::pPos( int i ) const
{
    if ( i == count() )
	return d->positions[i-1] + d->sizes[ d->i2s[i-1] ] - offset();
    return d->positions[i] - offset();
}


/*!
  Returns the size of actual section \a i.
 */
int QHeader::pSize( int i ) const
{
    return d->sizes[ d->i2s[i] ];
}

/*!
  Returns the height of actual section \a i if orientation() is horizontal ,
  returns the width if vertical.

*/
int QHeader::pHeight( int i ) const
{
    int section = mapToSection(i);
    if ( section < 0 )
	return 0;
    return d->heights[section];
}

/*!
  Sets the height of actual section \a i to \a h if orientation() is
  horizontal. Sets the width if vertical.
*/
void QHeader::setPHeight( int i, int h )
{
    int section = mapToSection(i);
    if ( section < 0 )
	return;
    d->heights[section] = h;
}



/*!
  Returns the leftmost (or uppermost for vertical headers) visible pixel.
 */
int QHeader::offset() const
{
    return offs;
}

/*!
  \obsolete

  Use mapToSection() instead!

  Translates from actual index \a a (index at which the section is displayed)  to
  logical index of the section.  Returns -1 if \a a is outside the legal range.

  \sa mapToActual()
*/

int QHeader::mapToLogical( int a ) const
{
    return mapToSection( a );
}


/*!
  \obsolete

  Use mapToIndex() instead!

  Translates from logical index \a l to actual index (index, at which the section \a l is displayed) .
  Returns -1 if \a l is outside the legal range.

  \sa mapToLogical()
*/

int QHeader::mapToActual( int l ) const
{
    return mapToIndex( l );
}


/*!
  \obsolete

  Use resizeSection() instead!

  Sets the size of the section \a section to \a s pixels.

  \warning does not repaint or send out signals
*/

void QHeader::setCellSize( int section, int s )
{
    if ( section < 0 || section >= count() )
	return;
    d->sizes[ section ] = s;
    if ( isUpdatesEnabled() )
	d->calculatePositions();
}


/*!
  Enable user resizing of the section \a section if \a enable is TRUE,
  disable otherwise.  If \a section is negative (as it is by default), resizing is
  enabled/disabled for all current and new sections.

  If the user resizes a section (because this feature enabled it), a sizeChange()
  signal is emitted.

  \sa setMovingEnabled(), setClickEnabled()
*/

void QHeader::setResizeEnabled( bool enable, int section )
{
    if ( section < 0 ) {
	d->resize.fill( enable );
	// and future ones...
	d->resize_default = enable;
    } else if ( section < count() ) {
	d->resize[ section ] = enable;
    }
}


/*!
  Enable the user to exchange sections if \a enable is TRUE,
  disable otherwise.

  If you enable moving here, the indexChange() signal is emitted if
  the user moves a section.

  \sa setClickEnabled(), setResizeEnabled()
*/

void QHeader::setMovingEnabled( bool enable )
{
    d->move = enable;
}


/*!
  Enable clicking in section \a section if \a enable is TRUE, disable
  otherwise.  If \a section is negative (as it is by default), clicking is
  enabled/disabled for all current and new sections.

  If enabled, the clicked() signal is emitted when the user clicks.

  \sa setMovingEnabled(), setResizeEnabled()
*/

void QHeader::setClickEnabled( bool enable, int section )
{
    if ( section < 0 ) {
	d->clicks.fill( enable );
	// and future ones...
	d->clicks_default = enable;
    } else if ( section < count() ) {
	d->clicks[ section ] = enable;
    }
}


/*!
  Paints actual section \a index of the header, inside rectangle \a fr in
  widget coordinates.

  Calls paintSectionLabel().
*/

void QHeader::paintSection( QPainter *p, int index, QRect fr )
{
    int section = mapToSection( index );
    if ( section < 0 )
	return;

    bool down = (index==handleIdx) && ( state == Pressed || state == Moving );
    p->setBrushOrigin( fr.topLeft() );
    if ( d->clicks[section] ) {
	style().drawBevelButton( p, fr.x(), fr.y(), fr.width(), fr.height(),
				 colorGroup(), down );
    } else {
	// ##### should be somhow styled in 3.0
	if ( orientation() == Horizontal ) {
	    p->save();

	    // ### Hack to keep styles working
	    p->setClipRect( fr );
	    style().drawBevelButton( p, fr.x() - 2, fr.y() - 2, fr.width() + 4, fr.height() + 4,
				     colorGroup(), down );
	
	    p->setPen( colorGroup().color( QColorGroup::Mid ) );
	    p->drawLine( fr.x(), fr.y() + fr.height() - 1, fr.x() + fr.width() - 1, fr.y() + fr.height() - 1 );
	    p->drawLine( fr.x() + fr.width() - 1, fr.y(), fr.x() + fr.width() - 1, fr.y() + fr.height() - 1 );
	    p->setPen( colorGroup().color( QColorGroup::Light ) );
	    if ( index > 0 )
		p->drawLine( fr.x(), fr.y(), fr.x(), fr.y() + fr.height() - 1 );
	    if ( index == count() - 1 ) {
		p->drawLine( fr.x() + fr.width() - 1, fr.y(), fr.x() + fr.width() - 1, fr.y() + fr.height() - 1 );
		p->setPen( colorGroup().color( QColorGroup::Mid ) );
		p->drawLine( fr.x() + fr.width() - 2, fr.y(), fr.x() + fr.width() - 2, fr.y() + fr.height() - 1 );
	    }
	    p->restore();
	} else {
	    p->save();

	    // ### Hack to keep styles working
	    p->setClipRect( fr );
	    style().drawBevelButton( p, fr.x() - 2, fr.y() - 2, fr.width() + 4, fr.height() + 4,
				     colorGroup(), down );
	
	    p->setPen( colorGroup().color( QColorGroup::Mid ) );
	    p->drawLine( fr.x() + width() - 1, fr.y(), fr.x() + fr.width() - 1, fr.y() + fr.height() - 1 );
	    p->drawLine( fr.x(), fr.y() + fr.height() - 1, fr.x() + fr.width() - 1, fr.y() + fr.height() - 1 );
	    p->setPen( colorGroup().color( QColorGroup::Light ) );
	    if ( index > 0 )
		p->drawLine( fr.x(), fr.y(), fr.x() + fr.width() - 1, fr.y() );
	    if ( index == count() - 1 ) {
		p->drawLine( fr.x(), fr.y() + fr.height() - 1, fr.x() + fr.width() - 1, fr.y() + fr.height() - 1 );
		p->setPen( colorGroup().color( QColorGroup::Mid ) );
		p->drawLine( fr.x(), fr.y() + fr.height() - 2, fr.x() + fr.width() - 1, fr.y() + fr.height() - 2 );
	    }
	    p->restore();
	}
    }

    paintSectionLabel( p, index, fr );
}

/*!
  Paints the label of actual section \a index of the header, inside rectangle \a fr in
  widget coordinates.

  Called by paintSection()
*/
void QHeader::paintSectionLabel( QPainter *p, int index, const QRect& fr )
{
    int section = mapToSection( index );
    if ( section < 0 )
	return;
    QString s;
    if ( d->labels[section] )
	s = *(d->labels[section]);
    else if ( orient == Horizontal )
	s = tr("Col %1").arg(section);
    else
	s = tr("Row %1").arg(section);

    int m = 0;
    if ( style() == WindowsStyle  &&
	 index==handleIdx && ( state == Pressed || state == Moving ) )
	m = 1;

    QRect r( fr.x() + QH_MARGIN+m, fr.y() + 2+m,
	     fr.width() - 6, fr.height() - 4 );

    int pw = 0;
    if ( d->iconsets[section] ) {
	QIconSet::Mode mode = isEnabled()?QIconSet::Normal:QIconSet::Disabled;
	QPixmap pixmap = d->iconsets[section]->pixmap( QIconSet::Small, mode );
	int pixw = pixmap.width();
	pw = pixw;
	int pixh = pixmap.height();
	p->drawPixmap( r.left(), r.center().y()-pixh/2, pixmap );
	r.setLeft( r.left() + pixw + 2 );
    }

    p->drawText ( r, AlignLeft|AlignVCenter|SingleLine, s );

    int arrowWidth = orient == Qt::Horizontal ? height() / 2 : width() / 2;
    int arrowHeight = fr.height() - 6;
    int tw = p->fontMetrics().width( s ) + 16;
    if ( d->sortColumn == section && pw + tw + arrowWidth + 2 < fr.width() ) {
	p->save();
	if ( d->sortDirection ) {
	    QPointArray pa( 3 );
	    int x = fr.x() + pw + tw;
	    p->setPen( colorGroup().light() );
	    p->drawLine( x + arrowWidth, 4, x + arrowWidth / 2, arrowHeight );
	    p->setPen( colorGroup().dark() );
	    pa.setPoint( 0, x + arrowWidth / 2, arrowHeight );
	    pa.setPoint( 1, x, 4 );
	    pa.setPoint( 2, x + arrowWidth, 4 );
	    p->drawPolyline( pa );
	} else {
	    QPointArray pa( 3 );
	    int x = fr.x() + pw + tw;
	    p->setPen( colorGroup().light() );
	    pa.setPoint( 0, x, arrowHeight );
	    pa.setPoint( 1, x + arrowWidth, arrowHeight );
	    pa.setPoint( 2, x + arrowWidth / 2, 4 );
	    p->drawPolyline( pa );
	    p->setPen( colorGroup().dark() );
	    p->drawLine( x, arrowHeight, x + arrowWidth / 2, 4 );
	}
	p->restore();
    }
}


/*!\reimp
*/
void QHeader::paintEvent( QPaintEvent *e )
{
    QPainter p( this );
    p.setPen( colorGroup().buttonText() );
    int pos = orient == Horizontal
		     ? e->rect().left()
		     : e->rect().top();
    int id = mapToIndex( sectionAt( pos + offset() ) );
    if ( id < 0 )
	if ( pos > 0 )
	    return;
	else
	    id = 0;
    for ( int i = id; i < count(); i++ ) {
	QRect r = sRect( i );
	paintSection( &p, i, r );
	if ( orient == Horizontal && r. right() >= e->rect().right() ||
	     orient == Vertical && r. bottom() >= e->rect().bottom() )
	    return;
    }

}

/*!
  As often the QHeader is used together with a list widget,
  QHeader can indicate a sort order. This is done using an arrow at
  the right edge of a section which points up or down. \a section
  specifies in which section this arrow should be drawn, and \a
  increasing, if the arrow should point to the bottom (TRUE) or the
  the top (FALSE).  If \a section is -1, no arrow is drawn.

  \sa QListView::setShowSortIndicator()
*/

void QHeader::setSortIndicator( int section, bool increasing )
{
    d->sortColumn = section;
    d->sortDirection = increasing;
    update();
    updateGeometry();
}

/*!
  Resizes the section \a section to \a s pixels.
*/

void QHeader::resizeSection( int section, int s )
{
    if ( section < 0 || section >= count() )
	return;
    d->sizes[ section ] = s;
    if ( isUpdatesEnabled() ) {
	d->calculatePositions();
	update();
    }
}

/*!
  Returns the size of the \a section in pixels.
*/

int QHeader::sectionSize( int section ) const
{
    if ( section < 0 || section >= count() )
	return 0;
    return d->sizes[section];
}

/*!
  Returns the position (in pixels) at which the \a section starts.

  \sa offset()
*/

int QHeader::sectionPos( int section ) const
{
    if ( d->positionsDirty )
	d->calculatePositions();
    if ( section < 0 || section >= count()  )
	return 0;
    return d->positions[ d->s2i[section] ];
}

/*!
  Returns the \a section which contains the position \a pos given in pixels.

  \sa offset()
*/

int QHeader::sectionAt( int pos ) const
{
    return d->sectionAt( pos );
}

/*!
  Returns the section which is displayed at the index \a index.
*/

int QHeader::mapToSection( int index ) const
{
    return ( index >= 0 && index < count() ) ? d->i2s[ index ] : -1;
}

/*!
  Returns the index at which the section \a section is
  currently displayed.
*/

int QHeader::mapToIndex( int section ) const
{
    return ( section >= 0 && section < count() ) ? d->s2i[ section ] : -1;
}

/*!
  Moves the section \a section to be displayed at the index
  \a toIndex.
*/

void QHeader::moveSection( int section, int toIndex )
{
    int fromIndex = mapToIndex( section );
    if ( fromIndex == toIndex ||
	 fromIndex < 0 || fromIndex > count() ||
	 toIndex < 0 || toIndex > count() )
	return;
    int i;
    int idx = d->i2s[fromIndex];
    if ( fromIndex < toIndex ) {
	for ( i = fromIndex; i < toIndex - 1; i++ ) {
	    int t;
	    d->i2s[i] = t = d->i2s[i+1];
	    d->s2i[t] = i;
	}
	d->i2s[toIndex-1] = idx;
	d->s2i[idx] = toIndex-1;
    } else {
	for ( i = fromIndex; i > toIndex ; i-- ) {
	    int t;
	    d->i2s[i] = t = d->i2s[i-1];
	    d->s2i[t] = i;
	}
	d->i2s[toIndex] = idx;
	d->s2i[idx] = toIndex;
    }
    d->calculatePositions();
}

/*!
  Returns whether the section \a section is clickable or not.
  If \a section is out of range (negative or larger than count() - 1),
  TRUE is returned if all sections are clickable, else FALSE.

  \sa setClickEnabled()
*/

bool QHeader::isClickEnabled( int section ) const
{
    if ( section >= 0 && section < count() ) {
	return (bool)d->clicks[ section ];
    }

    for ( int i = 0; i < count(); ++i ) {
	if ( !d->clicks[ i ] )
	    return FALSE;
    }
    return TRUE;
}

/*!
  Returns whether the section \a section is resizeable or not.
  If \a section is out of range (negative or larger than count() - 1),
  TRUE is returned if all sections are resizeable, else FALSE.

  \sa setResizeEnabled()
*/

bool QHeader::isResizeEnabled( int section ) const
{
    if ( section >= 0 && section < count() ) {
	return (bool)d->resize[ section ];
    }

    for ( int i = 0; i < count();++i ) {
	if ( !d->resize[ i ] )
	    return FALSE;
    }
    return TRUE;
}

/*!
  Returns TRUE if the sections of the header can be moved around be the user,
  otherwise FALSE.

  \sa setMovingEnabled()
*/

bool QHeader::isMovingEnabled() const
{
    return d->move;
}

/*! reimp */

void QHeader::setUpdatesEnabled( bool enable )
{
    if ( enable )
	d->calculatePositions();
    QWidget::setUpdatesEnabled( enable );
}

//#### what about lastSectionCoversAll?
#endif // QT_NO_HEADER
