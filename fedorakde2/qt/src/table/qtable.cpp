/****************************************************************************
**
** Implementation of QTable widget class
**
** Created : 000607
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of the table module of the Qt GUI Toolkit.
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
** Licensees holding valid Qt Enterprise Edition licenses may use this
** file in accordance with the Qt Commercial License Agreement provided
** with the Software.
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

#include "qtable.h"

#ifndef QT_NO_TABLE

#include <qpainter.h>
#include <qkeycode.h>
#include <qlineedit.h>
#include <qapplication.h>
#include <qtimer.h>
#include <qobjectlist.h>
#include <stdlib.h>
#include <limits.h>

struct QTablePrivate
{
};

struct QTableHeaderPrivate
{
};


/*! \class QTableSelection qtable.h

  \brief The QTableSelection provides access to the selected area in a
  QTable.

  \module table

  The selection is a rectangular set of cells.  One of the rectangle's
  cells is called the anchor cell; that cell is the first selected.
  The init() function sets the anchor and sets the selection rectangle
  to just that cell; the expandTo() function expands the selection
  rectangle.

  There are various access functions to get the area: anchorRow() and
  anchorCol() return the achor's position, and leftCol(), rightCol(),
  topRow() and bottomRow() return the rectangle's four edges.  All
  four are in the selection.

  A newly created QTableSelection is inactive -- isActive() returns
  FALSE.  You must use init() and expandTo() to activate it.

  \sa QTable QTable::addSelection() QTable::selection().
*/

/*!  Creates an inactive selection. Use init() and expandTo() to
  activate it.
*/

QTableSelection::QTableSelection()
    : active( FALSE ), inited( FALSE ), tRow( -1 ), lCol( -1 ),
      bRow( -1 ), rCol( -1 ), aRow( -1 ), aCol( -1 )
{
}

/*!  Sets the selection anchor to row \a row and column \a col and
  sets the selection to just that cell.

  \sa expandTo() isActive()
*/

void QTableSelection::init( int row, int col )
{
    aCol = lCol = rCol = col;
    aRow = tRow = bRow = row;
    active = FALSE;
    inited = TRUE;
}

/*!  Expands the selection to \a row, \a col. The new selection
  rectangle is the bounding rectangle of \a row, \a col and the old
  selection rectangle. After calling that function, the selections is
  active.

  If you didn't call init() yet, this function does nothing.

  \sa init() isActive()
*/

void QTableSelection::expandTo( int row, int col )
{
    if ( !inited )
	return;
    active = TRUE;

    if ( row < aRow ) {
	tRow = row;
	bRow = aRow;
    } else {
	tRow = aRow;
	bRow = row;
    }

    if ( col < anchorCol() ) {
	lCol = col;
	rCol = aCol;
    } else {
	lCol = aCol;
	rCol = col;
    }
}

/*! Returns TRUE if \a s includes the same cells as this selection, or
  else FALSE.
*/

bool QTableSelection::operator==( const QTableSelection &s ) const
{
    return ( s.active == active &&
	     s.tRow == tRow && s.bRow == bRow &&
	     s.lCol == lCol && s.rCol == rCol );
}

/*! \fn int QTableSelection::topRow() const
  Returns the top row of the selection.
*/

/*! \fn int QTableSelection::bottomRow() const
  Returns the bottom row of the selection.
*/

/*! \fn int QTableSelection::leftCol() const
  Returns the left column of the selection.
*/

/*! \fn int QTableSelection::rightCol() const
  Returns the right column of the selection.
*/

/*! \fn int QTableSelection::anchorRow() const
  Returns the anchor row of the selection.
*/

/*! \fn int QTableSelection::anchorCol() const
  Returns the anchor column of the selection.
*/

/*! \fn bool QTableSelection::isActive() const
  Returns whether the selection is active or not. A selection is
  active after init() and expandTo() has beem called.
*/

// ### this class doc needs total rewrite. it looks okay, but the
// class contains about twenty members and this doesn't mention enough
// of them to be considered exhaustive.

/*!
  \class QTableItem qtable.h

  \brief The QTableItem class provides content for one cell in a QTable.

  \module table

  A QTableItem contains the data of a table cell, specifies its edit
  type and the editor used to change its content. Furthermore, it
  defines its size, the alignment of the data to display, whether the
  data can be replaced, and provides the API needed for sorting table
  items.

  Items may contain text and pixmaps and offer a QLineEdit for
  editing.  By reimplementing paint(), key(), createEditor() and
  setContentFromEditor() you can change these default settings.

  To get rid of an item, simply delete it. By doing so, all required
  actions for removing it from the table will be taken.
*/

/*! \fn QTable *QTableItem::table() const

  Returns the QTable the item belongs to.
*/

/*! \enum QTableItem::EditType

  This enum type defines whether and when the user may edit a table
  cell.  The currently defined states are:

  <ul>
  <li>\c Always - the cell always is and always looks editable.

  <li>\c WhenCurrent - the cell is editable, and looks editable
  whenever it has keyboard focus (see QTable::setCurrentCell()).

  <li>\c OnTyping - the cell is editable, but looks editable only when
  the user types in it or double-clicks in it. (This is like \c
  WhenCurrent in function but can look a bit cleaner.)

  <li>\c Never - the cell isn't editable.

  </ul>
*/

/*!  Creates a table item for the table \a table that contains the text
     \a text.
*/

QTableItem::QTableItem( QTable *table, EditType et, const QString &text )
    : txt( text ), pix(), t( table ), edType( et ), wordwrap( FALSE ),
      tcha( TRUE ), rw( -1 ), cl( -1 ), rowspan( 1 ), colspan( 1 )
{
}

/*!  Creates an item for the table \a table with the text \a text and the
  pixmap \a p.
*/

QTableItem::QTableItem( QTable *table, EditType et,
			const QString &text, const QPixmap &p )
    : txt( text ), pix( p ), t( table ), edType( et ), wordwrap( FALSE ),
      tcha( TRUE ), rw( -1 ), cl( -1 ), rowspan( 1 ), colspan( 1 )
{
}

/*!  Destructor.
*/

QTableItem::~QTableItem()
{
    table()->takeItem( this );
}

/*!  Returns the item's pixmap.
*/

QPixmap QTableItem::pixmap() const
{
    return pix;
}


/*!  Provides the text of the item.
*/

QString QTableItem::text() const
{
    if ( edType == Always ) //### why only always?
	((QTableItem*)this)->setContentFromEditor(table()->cellWidget(rw,cl));
    return txt;
}

/*!  Sets the item pixmap to \a pix. QTableItem::setPixmap(), however,
     does not repaint the cell.
*/

void QTableItem::setPixmap( const QPixmap &p )
{
    pix = p;
}

/*!  Changes the text of the item to \a str. Note that the cell is not
 repainted.
*/

void QTableItem::setText( const QString &str )
{
    txt = str;
}

/*! In order to paint the contents of an item call \a QTableItem::paint().
  \internal
*/

void QTableItem::paint( QPainter *p, const QColorGroup &cg,
			const QRect &cr, bool selected )
{
    p->fillRect( 0, 0, cr.width(), cr.height(),
		 selected ? cg.brush( QColorGroup::Highlight )
		          : cg.brush( QColorGroup::Base ) );

    int w = cr.width();
    int h = cr.height();

    int x = 0;
    if ( !pix.isNull() ) {
	p->drawPixmap( 0, ( cr.height() - pix.height() ) / 2, pix );
	x = pix.width() + 2;
    }

    if ( selected )
	p->setPen( cg.highlightedText() );
    else
	p->setPen( cg.text() );
    p->drawText( x + 2, 0, w - x - 4, h,
		 wordwrap ? (alignment() | WordBreak) : alignment(), txt );
}

/*!  This virtual function creates the editor with which the user can
  edit the cell.  The default implementation creates a QLineEdit.

  If the function returns 0, that the cell can not be edited.

  The returned widget should preferably not be visible, and it should
  preferably have QTable::viewport() as parent.

  If you reimplement this function, you probably also need to
  reimplement setContentFromEditor().

  \sa QTable::createEditor() setContentFromEditor() QTable::viewport()
*/

QWidget *QTableItem::createEditor() const
{
    QLineEdit *e = new QLineEdit( table()->viewport() );
    e->setFrame( FALSE );
    e->setText( text() );
    return e;
}

/*!  Whenever the content of a cell has been edited by the editor \a
  w, QTable calls this virtual function to copy the new values into
  the QTableItem.

  You probably \e must reimplement this function if you reimplement
  createEditor() and return something that is not a QLineEdit.

  \sa QTable::setContentFromEditor()
*/

void QTableItem::setContentFromEditor( QWidget *w )
{
    if ( w && w->inherits( "QLineEdit" ) )
	setText( ( (QLineEdit*)w )->text() );
}

/*!  The alignment function returns how the contents of the cell are
  drawn. The default implementation aligns numbers to the right and
  other text to the left.
*/

int QTableItem::alignment() const
{
    bool num;
    bool ok1 = FALSE, ok2 = FALSE;
    (void)txt.toInt( &ok1 );
    if ( !ok1 )
	(void)txt.toDouble( &ok2 ); // ### should be .-aligned
    num = ok1 || ok2;

    return ( num ? AlignRight : AlignLeft ) | AlignVCenter;
}

/*! If \a b is TRUE, the cell's text is wrapped into multiple lines,
  otherwise it will be written on one line.
*/

void QTableItem::setWordWrap( bool b )
{
    wordwrap = b;
}

/*! If word wrap has been turned on for the cell in question,
   wordWrap() is TRUE, otherwise it returns FALSE.
*/

bool QTableItem::wordWrap() const
{
    return wordwrap;
}

/*! \internal */

void QTableItem::updateEditor( int oldRow, int oldCol )
{
    if ( edType != Always )
	return;
    if ( oldRow != -1 && oldCol != -1 )
	table()->clearCellWidget( oldRow, oldCol );
    if ( rw != -1 && cl != -1 )
	table()->setCellWidget( rw, cl, createEditor() );
}

/*!  Returns the edit type of an item.

  \sa EditType
*/

QTableItem::EditType QTableItem::editType() const
{
    return edType;
}

/*! If it shouldn't be possible to replace the contents of the relevant cell
  with those of another QTableItem, set \a b to FALSE.
*/

void QTableItem::setReplaceable( bool b )
{
    tcha = b;
//ed: what the heck does tcha stand for?
}

/*! This function returns whether the relevant QTableItem can be replaced
 or not. Only items that cover no more than one cell might be replaced.

 \sa setReplaceable()
*/

bool QTableItem::isReplaceable() const
{
    if ( rowspan > 1 || colspan > 1 )
	return FALSE;
    return tcha;
}

/*! This virtual function returns the key that should be used for
  sorting. The default implementation returns the text() of the
  relevant item.
*/

QString QTableItem::key() const
{
    return text();
}

/*!  This virtual function returns the size a cell needs to show its
  entire content.

  Many custom table items will need to reimplement this function.
*/

QSize QTableItem::sizeHint() const
{
    if ( edType == Always && table()->cellWidget( rw, cl ) )
	return table()->cellWidget( rw, cl )->sizeHint();

    QSize s;
    if ( !pix.isNull() ) {
	s = pix.size();
	s.setWidth( s.width() + 2 );
    }

    if ( !wordwrap )
	return QSize( s.width() + table()->fontMetrics().width( text() ) + 10, QMAX( s.height(), table()->fontMetrics().height() ) );
    QRect r = table()->fontMetrics().boundingRect( 0, 0, table()->columnWidth( col() ), 0, wordwrap ? (alignment() | WordBreak) : alignment(), txt );
    return QSize( s.width() + r.width(), QMAX( s.height(), r.height() ) );
}

/*!  Creates a multi-cell QTableItem covering \a rs rows and \a cs
  columns.  The top left corner of the item is at the item's former
  position.

  \warning This function only works, if the item has already been
  inserted into the table using e.g. QTable::setItem().
*/

void QTableItem::setSpan( int rs, int cs )
{
    if ( rw == -1 || cl == -1 )
	return;
	
    int rrow = rw;
    int rcol = cl;
    if ( rowspan > 1 || colspan > 1 ) {
	table()->takeItem( this );
	table()->setItem( rrow, rcol, this );
    }

    rowspan = rs;
    colspan = cs;

    for ( int r = 0; r < rowspan; ++r ) {
	for ( int c = 0; c < colspan; ++c ) {
	    if ( r == 0 && c == 0 )
		continue;
	    table()->setItem( r + rw, c + cl, this );
	    rw = rrow;
	    cl = rcol;
	}
    }

    table()->updateCell( rw, cl );
}

/*! Returns the row span of an item, normally 1.

  \sa setSpan()
*/

int QTableItem::rowSpan() const
{
    return rowspan;
}

/*! Returns teh column span of the item, normally 1.

  \sa setSpan()
*/

int QTableItem::colSpan() const
{
    return colspan;
}

/*! Sets the item's row to be \a row.  Normally you will not need to
  call this function.

  If the cell spans multiple rows, this function sets the top row and
  retains the height.

*/

void QTableItem::setRow( int r )
{
    rw = r;
}

/*! Sets the item's column to be \a c.  Normally you will not need to
  call this function.

  If the cell spans multiple columns, this function sets the leftmost
  column and retains the width.
*/

void QTableItem::setCol( int c )
{
    cl = c;
}

/*! Returns the row where the item is located.  If the cell spans
  multiple rows, this function returns the top row.
*/

int QTableItem::row() const
{
    return rw;
}

/*! Returns the column where the item is located.  If the cell spans
  multiple columns, this function returns the leftmost column.
*/

int QTableItem::col() const
{
    return cl;
}


// ### this needs a bit of general stuff at the top

/*!
  \class QTable qtable.h
  \module table

  \brief A flexible and editable table widget.

  QTable has been designed to use no more memory than strictly
  needed. Thus, for an empty cell, no memory at all is allocated.  In
  order to add data, create a QTableItem and fill it using
  setItem(). With QTableItem::setText() and QTableItem::setPixmap(),
  convenient functions for setting table text and pixmaps are
  provided.  To clear a cell use clearCell().

  QTable supports various methods for selecting cells, both with
  keyboard and mouse, thus for example range selection or column and
  row selection via appropriate header cells. You can add and remove
  selections using addSelection() and removeSelection(), resp., and
  gather information about current selections by means of
  numSelections(), selection(), and currentChanged().

  QTable also offers an API for sorting columns. See setSorting(),
  sortColumn() and QTableItem::key() for details.

  Cell editing can be done in two different ways: Either you offer an
  edit widget the user can use to enter data that should replace the
  current content, or you provide him or her with an editor to change
  the data stored in the cell. If you won't allow the content of a
  cell to be replaced, however make it possible to edit the current
  data, simply set QTableItem::isReplaceable() to FALSE.

  When a user starts typing text in-place editing (replacing) for the
  current cell is invoked. Additionally, in-place editing (editing)
  starts as soon as he or she double-clicks a cell.  Sometimes,
  however, it is required that a cell always shows an editor, that the
  editor shows off as soon as the relevant cell receives the focus, or
  that the item shouldn't be edited at all. This edit type has to be
  specified in the constructor of a QTableItem.

  In-place editing is invoked by beginEdit(). This function creates
  the editor widget for the required cell (see createEditor() for
  detailed information) and shows it at the appropriate location.

  As soon as the user finishes editing endEdit() is called. Have a
  look at the endEdit() documentation for further information e.g. on
  how content is transferred from the editor to the item or how the
  editor is destroyed.

  In-place editing is implemented in an abstract way to make sure
  custom edit widgets for certain cells or cell types can be written
  easily. To obtain this it is possible to place widgets in cells. See
  setCellWidget(), clearCellWidget() and cellWidget() for further
  details.

  In order to prevent a cell not containing a QTableItem from being
  edited, you have to reimplement createEditor(). This function should
  return 0 for cells that must be not edited at all.

  It is possible to use QTable without QTableItems. However, as the
  default implementation of QTable's in-place editing uses
  QTableItems, you will have to reimplement createEditor() and
  setCellContentFromEditor() to get in-place editing without
  QTableItems. The documentation of these two functions explains the
  details you need to know for that matter.

  In order to draw custom content in a cell you have to implement your
  own subclass of QTableItem and reimplement the QTableItem::paint()
  method.

  If your application stores its data already in a way that allocating
  a QTableItem for each data containing cell seems inappropriate, you
  can reimplement QTable::paintCell() and draw the contents directly.
  You should also reimplement QTable::paintCell() if you wish to change
  the alignment of your items in the QTable.

  Unless you reimplement them this approach will however prevent you
  from using functions like setText() etc.  Remember that in this
  case, repainting the cells using updateCell() after each change made
  is necessary. To make sure you don't waste memory, read the
  documentation of resizeData().
*/

/*! \fn void QTable::currentChanged( int row, int col )

  This signal is emitted if the current cell has been changed to \a
  row, \a col.
*/

/*! \fn void QTable::valueChanged( int row, int col )

  This signal is emitted if the user edited the cell row, \a col.
*/

/*! \fn int QTable::currentRow() const

  Returns the current row.
*/

/*! \fn int QTable::currentColumn() const

  Returns the current column.
*/

/*! \enum QTable::EditMode
  <ul>
  <li>\c NotEditing - the cell is not being now
  <li>\c Editing - the cell is being edited, from its old contents
  <li>\c Replacing - the cell is being edited, and was cleared when
         editing started
  </ul>

*/
/*! \enum QTable::SelectionMode
  <ul>
  <li>\c NoSelection - No cell can be selected by the user.
  <li>\c Single - The user may select one range of cells only.
  <li>\c Multi - Multi-range selections are possible.
  </ul>
*/

/*! \fn void QTable::clicked( int row, int col, int button, const QPoint &mousePos )
  This signal is emitted as soon as a user clicks on \a
  row and \a col using mousebutton \a button.
  The actual mouse position is passed as \a mousePos.
*/

/*! \fn void QTable::doubleClicked( int row, int col, int button, const QPoint &mousePos )
  A double-click with \a button emits this signal, where \a
  row and \a col denote the position of the cell.
  The actual mouse position is passed as \a mousePos.
*/

/*! \fn void QTable::pressed( int row, int col, int button, const QPoint &mousePos )
  This signal is emitted whenever the mousebutton \a button is pressed above
  the cell located in  \a row and \a col.
  The actual mouse position is passed as \a mousePos.
*/

/*! \fn void QTable::selectionChanged()
  Whenever a selection changes, this signal is emitted.
*/

/*!
  Constructs a table of 10 * 10 cells.

  Performance is boosted by modifying the widget flags so that only part
  of the QTableItem children is redrawn.  This may be unsuitable for custom
  QTableItem classes, in which case \c WNorthWestGravity and \c WRepaintNoErase
  should be cleared.

  \sa QWidget::clearWFlags() Qt::WidgetFlags
*/

QTable::QTable( QWidget *parent, const char *name )
    : QScrollView( parent, name, WRepaintNoErase | WNorthWestGravity ),
      currentSel( 0 ), sGrid( TRUE ), mRows( FALSE ), mCols( FALSE ),
      lastSortCol( -1 ), asc( TRUE ), doSort( TRUE )
{
    init( 0, 0 );
}

/*!
  Constructs a table with a range of \a numRows * \a numCols cells.

  Performance is boosted by modifying the widget flags so that only part of
  the QTableItem children is redrawn.  This may be unsuitable for custom
  QTableItem classes, in which case the widget flags should be reset using
  QWidget::setWFlags().
*/

QTable::QTable( int numRows, int numCols, QWidget *parent, const char *name )
    : QScrollView( parent, name, WRepaintNoErase | WNorthWestGravity ),
      currentSel( 0 ), sGrid( TRUE ), mRows( FALSE ), mCols( FALSE ),
      lastSortCol( -1 ), asc( TRUE ), doSort( TRUE )
{
    init( numRows, numCols );
}

/*! \internal
*/

void QTable::init( int rows, int cols )
{
    d = 0;
    setSorting( FALSE );

    mousePressed = FALSE;

    selMode = Multi;

    contents.setAutoDelete( TRUE );
    widgets.setAutoDelete( TRUE );

    // Enable clipper and set background mode
    enableClipper( TRUE );
    viewport()->setFocusProxy( this );
    viewport()->setFocusPolicy( WheelFocus );
    viewport()->setBackgroundMode( PaletteBase );
    setResizePolicy( Manual );
    selections.setAutoDelete( TRUE );

    // Create headers
    leftHeader = new QTableHeader( rows, this, this );
    leftHeader->setOrientation( Vertical );
    leftHeader->setTracking( TRUE );
    leftHeader->setMovingEnabled( TRUE );
    topHeader = new QTableHeader( cols, this, this );
    topHeader->setOrientation( Horizontal );
    topHeader->setTracking( TRUE );
    topHeader->setMovingEnabled( TRUE );
    setMargins( 30, fontMetrics().height() + 4, 0, 0 );

    topHeader->setUpdatesEnabled( FALSE );
    leftHeader->setUpdatesEnabled( FALSE );
    // Initialize headers
    int i = 0;
    for ( i = 0; i < numCols(); ++i ) {
	topHeader->setLabel( i, QString::number( i + 1 ) );
	topHeader->resizeSection( i, 100 );
    }
    for ( i = 0; i < numRows(); ++i ) {
	leftHeader->setLabel( i, QString::number( i + 1 ) );
	leftHeader->resizeSection( i, 20 );
    }
    topHeader->setUpdatesEnabled( TRUE );
    leftHeader->setUpdatesEnabled( TRUE );

    // Prepare for contents
    contents.setAutoDelete( FALSE );

    // Connect header, table and scrollbars
    connect( horizontalScrollBar(), SIGNAL( valueChanged( int ) ),
	     topHeader, SLOT( setOffset( int ) ) );
    connect( verticalScrollBar(), SIGNAL( valueChanged( int ) ),
	     leftHeader, SLOT( setOffset( int ) ) );
    connect( topHeader, SIGNAL( sectionSizeChanged( int ) ),
	     this, SLOT( columnWidthChanged( int ) ) );
    connect( topHeader, SIGNAL( indexChange( int, int, int ) ),
	     this, SLOT( columnIndexChanged( int, int, int ) ) );
    connect( topHeader, SIGNAL( sectionClicked( int ) ),
	     this, SLOT( columnClicked( int ) ) );
    connect( leftHeader, SIGNAL( sectionSizeChanged( int ) ),
	     this, SLOT( rowHeightChanged( int ) ) );
    connect( leftHeader, SIGNAL( indexChange( int, int, int ) ),
	     this, SLOT( rowIndexChanged( int, int, int ) ) );

    // Initialize variables
    autoScrollTimer = new QTimer( this );
    connect( autoScrollTimer, SIGNAL( timeout() ),
	     this, SLOT( doAutoScroll() ) );
    curRow = curCol = 0;
    edMode = NotEditing;
    editRow = editCol = -1;

    installEventFilter( this );

    // Initial size
    resize( 640, 480 );
}

/*!  Destructor.
*/

QTable::~QTable()
{
    delete d;
    contents.setAutoDelete( TRUE );
    contents.clear();
    widgets.clear();
}

/*!  Sets the table's selection mode to \a mode. By default multi-range
selections (\c Multi) are allowed.
*/

void QTable::setSelectionMode( SelectionMode mode )
{
    selMode = mode;
}

/*!  Reveals the current selection mode.
*/

QTable::SelectionMode QTable::selectionMode() const
{
    return selMode;
}

/*!  Returns the top QHeader of the table.
*/

QHeader *QTable::horizontalHeader() const
{
    return (QHeader*)topHeader;
}

/*!  Returns the outer left QHeader.
*/

QHeader *QTable::verticalHeader() const
{
    return (QHeader*)leftHeader;
}

/*!  If \a b is TRUE, the table grid is shown, otherwise not.  The
  default is TRUE.
*/

void QTable::setShowGrid( bool b )
{
    if ( sGrid == b )
	return;
    sGrid = b;
    viewport()->repaint( FALSE );
}

/*!  Returns whether the table grid shows up or not.
*/

bool QTable::showGrid() const
{
    return sGrid;
}

/*!  If \a b is set to TRUE, columns can be moved by the user.
*/

void QTable::setColumnMovingEnabled( bool b )
{
    mCols = b;
}

/*!  Returns whether columns can be moved by the user.
*/

bool QTable::columnMovingEnabled() const
{
    return mCols;
}

/*!  If \a b is set to TRUE, rows can be moved by the user.
*/

void QTable::setRowMovingEnabled( bool b )
{
    mRows = b;
}

/*!  Returns whether rows can be moved by the user.
*/

bool QTable::rowMovingEnabled() const
{
    return mRows;
}

/*!  This is called when QTable's internal array needs to be resized.

  If you don't use QTableItems you should reimplement this as an empty
  method, thus no memory is wasted. In addition, you will have to
  reimplement item(), setItem(), and clearCell() as empty functions in
  a different way.

  As soon as you enable sorting or allow the user to change rows or
  columns (see setRowMovingEnabled(), setColumnMovingEnabled()), you
  are strongly advised to reimplement swapColumns(), swapRows(), and
  swapCells() to work with your data.
*/

void QTable::resizeData( int len )
{
    contents.resize( len );
}

/*!  Swaps data of \a row1 and \a row2. This is used by sorting
  mechanisms or when the user changes the order of the rows. If you
  don't use QTableItems you might wish to reimplement this function.
*/

void QTable::swapRows( int row1, int row2 )
{
    QVector<QTableItem> tmpContents;
    tmpContents.resize( numCols() );
    QVector<QWidget> tmpWidgets;
    tmpWidgets.resize( numCols() );
    int i;

    contents.setAutoDelete( FALSE );
    widgets.setAutoDelete( FALSE );
    for ( i = 0; i < numCols(); ++i ) {
	QTableItem *i1, *i2;
	i1 = item( row1, i );
	i2 = item( row2, i );
	if ( i1 || i2 ) {
	    tmpContents.insert( i, i1 );
	    contents.remove( indexOf( row1, i ) );
	    contents.insert( indexOf( row1, i ), i2 );
	    contents.remove( indexOf( row2, i ) );
	    contents.insert( indexOf( row2, i ), tmpContents[ i ] );
	    if ( contents[ indexOf( row1, i ) ] )
		contents[ indexOf( row1, i ) ]->setRow( row1 );
	    if ( contents[ indexOf( row2, i ) ] )
		contents[ indexOf( row2, i ) ]->setRow( row2 );
	}

	QWidget *w1, *w2;
	w1 = cellWidget( row1, i );
	w2 = cellWidget( row2, i );
	if ( w1 || w2 ) {
	    tmpWidgets.insert( i, w1 );
	    widgets.remove( indexOf( row1, i ) );
	    widgets.insert( indexOf( row1, i ), w2 );
	    widgets.remove( indexOf( row2, i ) );
	    widgets.insert( indexOf( row2, i ), tmpWidgets[ i ] );
	}
    }
    contents.setAutoDelete( FALSE );
    widgets.setAutoDelete( TRUE );

    updateRowWidgets( row1 );
    updateRowWidgets( row2 );
    if ( curRow == row1 )
	curRow = row2;
    else if ( curRow == row2 )
	curRow = row1;
    if ( editRow == row1 )
	editRow = row2;
    else if ( editRow == row2 )
	editRow = row1;
}

/*!
  Sets the left margin to \a m pixels.

  To get rid of the left header entirely, use the following code:

  \code
  setLeftMargin( 0 );
  verticalHeader()->hide();
  \endcode
*/

void QTable::setLeftMargin( int m )
{
    setMargins( m, topMargin(), rightMargin(), bottomMargin() );
    updateGeometries();
}

/*!
  Sets the top margin to \a m pixels.

  To get rid of the top header entirely, use the following code:

  \code
  setTopMargin( 0 );
  topHeader()->hide();
  \endcode
*/

void QTable::setTopMargin( int m )
{
    setMargins( leftMargin(), m, rightMargin(), bottomMargin() );
    updateGeometries();
}

/*!  Exchanges \a col1 with \a col2 and vice versa. This is useful for
  sorting, and it allows the user to rearrange the columns in a
  different order. If you don't use QTableItems you will probably
  reimplement this function.
*/

void QTable::swapColumns( int col1, int col2 )
{
    QVector<QTableItem> tmpContents;
    tmpContents.resize( numRows() );
    QVector<QWidget> tmpWidgets;
    tmpWidgets.resize( numRows() );
    int i;

    contents.setAutoDelete( FALSE );
    widgets.setAutoDelete( FALSE );
    for ( i = 0; i < numRows(); ++i ) {
	QTableItem *i1, *i2;
	i1 = item( i, col1 );
	i2 = item( i, col2 );
	if ( i1 || i2 ) {
	    tmpContents.insert( i, i1 );
	    contents.remove( indexOf( i, col1 ) );
	    contents.insert( indexOf( i, col1 ), i2 );
	    contents.remove( indexOf( i, col2 ) );
	    contents.insert( indexOf( i, col2 ), tmpContents[ i ] );
	    if ( contents[ indexOf( i, col1 ) ] )
		contents[ indexOf( i, col1 ) ]->setCol( col1 );
	    if ( contents[ indexOf( i, col2 ) ] )
		contents[ indexOf( i, col2 ) ]->setCol( col2 );
	}

	QWidget *w1, *w2;
	w1 = cellWidget( i, col1 );
	w2 = cellWidget( i, col2 );
	if ( w1 || w2 ) {
	    tmpWidgets.insert( i, w1 );
	    widgets.remove( indexOf( i, col1 ) );
	    widgets.insert( indexOf( i, col1 ), w2 );
	    widgets.remove( indexOf( i, col2 ) );
	    widgets.insert( indexOf( i, col2 ), tmpWidgets[ i ] );
	}
    }
    contents.setAutoDelete( FALSE );
    widgets.setAutoDelete( TRUE );

    columnWidthChanged( col1 );
    columnWidthChanged( col2 );
    if ( curCol == col1 )
	curCol = col2;
    else if ( curCol == col2 )
	curCol = col1;
    if ( editCol == col1 )
	editCol = col2;
    else if ( editCol == col2 )
	editCol = col1;
}

/*!  Swaps the content of the cells \a row1, \a col1 and \a row2, \a
  col2. This function is used for sorting cells.
*/

void QTable::swapCells( int row1, int col1, int row2, int col2 )
{
    contents.setAutoDelete( FALSE );
    widgets.setAutoDelete( FALSE );
    QTableItem *i1, *i2;
    i1 = item( row1, col1 );
    i2 = item( row2, col2 );
    if ( i1 || i2 ) {
	QTableItem *tmp = i1;
	contents.remove( indexOf( row1, col1 ) );
	contents.insert( indexOf( row1, col1 ), i2 );
	contents.remove( indexOf( row2, col2 ) );
	contents.insert( indexOf( row2, col2 ), tmp );
	if ( contents[ indexOf( row1, col1 ) ] ) {
	    contents[ indexOf( row1, col1 ) ]->setRow( row1 );
	    contents[ indexOf( row1, col1 ) ]->setCol( col1 );
	}
	if ( contents[ indexOf( row2, col2 ) ] ) {
	    contents[ indexOf( row2, col2 ) ]->setRow( row2 );
	    contents[ indexOf( row2, col2 ) ]->setCol( col2 );
	}
    }

    QWidget *w1, *w2;
    w1 = cellWidget( row1, col1 );
    w2 = cellWidget( row2, col2 );
    if ( w1 || w2 ) {
	QWidget *tmp = w1;
	widgets.remove( indexOf( row1, col1 ) );
	widgets.insert( indexOf( row1, col1 ), w2 );
	widgets.remove( indexOf( row2, col2 ) );
	widgets.insert( indexOf( row2, col2 ), tmp );
    }

    updateRowWidgets( row1 );
    updateRowWidgets( row2 );
    updateColWidgets( col1 );
    updateColWidgets( col2 );
    contents.setAutoDelete( FALSE );
    widgets.setAutoDelete( TRUE );
}

/*!  Draws the table contents on the painter \a p. The function is
  optimized to exclusively draw the cells inside the relevant clipping
  rectangle \a cx, \a cy, \a cw, \a ch.

  Additionally, drawContents() highlights the current cell.
*/

void QTable::drawContents( QPainter *p, int cx, int cy, int cw, int ch )
{
    int colfirst = columnAt( cx );
    int collast = columnAt( cx + cw );
    int rowfirst = rowAt( cy );
    int rowlast = rowAt( cy + ch );

    if ( rowfirst == -1 || colfirst == -1 ) {
	paintEmptyArea( p, cx, cy, cw, ch );
	return;
    }

    if ( rowlast == -1 )
	rowlast = numRows() - 1;
    if ( collast == -1 )
	collast = numCols() - 1;

    // Go through the rows
    for ( int r = rowfirst; r <= rowlast; ++r ) {
	// get row position and height
	int rowp = rowPos( r );
	int rowh = rowHeight( r );

	if ( rowh == 0 )
	    continue;
	
	// Go through the columns in row r
	// if we know from where to where, go through [colfirst, collast],
	// else go through all of them
	for ( int c = colfirst; c <= collast; ++c ) {
	    // get position and width of column c
	    int colp, colw;
	    colp = columnPos( c );
	    colw = columnWidth( c );
	    int oldrp = rowp;
	    int oldrh = rowh;

	    if ( colw == 0 )
		continue;
	
	    QTableItem *itm = item( r, c );
	    if ( itm &&
		 ( itm->colSpan() > 1 || itm->rowSpan() > 1 ) ) {
		bool goon = r == itm->row() && c == itm->col() ||
			r == rowfirst && c == itm->col() ||
			r == itm->row() && c == colfirst;
		if ( !goon )
		    continue;
		rowp = rowPos( itm->row() );
		rowh = 0;
		int i;
		for ( i = 0; i < itm->rowSpan(); ++i )
		    rowh += rowHeight( i + itm->row() );
		colp = columnPos( itm->col() );
		colw = 0;
		for ( i = 0; i < itm->colSpan(); ++i )
		    colw += columnWidth( i + itm->col() );
	    }

	    // Translate painter and draw the cell
#ifdef QT_NO_TRANSFORMATIONS // ### Need both, or will the first do? (first is in qt/main)
	    p->translate( colp, rowp );
	    paintCell( p, r, c, QRect( colp, rowp, colw, rowh ), isSelected( r, c ) );
	    p->translate( -colp, -rowp );
#else
	    p->saveWorldMatrix();
	    p->translate( colp, rowp );
	    paintCell( p, r, c, QRect( colp, rowp, colw, rowh ), isSelected( r, c ) );
	    p->restoreWorldMatrix();
#endif

	    rowp = oldrp;
	    rowh = oldrh;
	}
    }

    // draw indication of current cell
    QRect focusRect = cellGeometry( curRow, curCol );

#ifdef QT_NO_TRANSFORMATIONS // ### Need both, or will the first do? (first is in qt/main)
    p->translate( focusRect.x(), focusRect.y() );
    paintFocus( p, focusRect );
    p->translate( -focusRect.x(), -focusRect.y() );
#else
    p->saveWorldMatrix();
    p->translate( focusRect.x(), focusRect.y() );
    paintFocus( p, focusRect );
    p->restoreWorldMatrix();
#endif

    // Paint empty rects
    paintEmptyArea( p, cx, cy, cw, ch );
}

/*!  Paints the cell at the position \a row, \a col on the painter \a
  p. The painter has already been translated to the cell's origin. \a
  cr describes the cell coordinates in the content coordinate system..

  If you want to draw custom cell content you have to reimplement
  paintCell() to do the custom drawing, or else subclass QTableItem
  and reimplement QTableItem::paint().

  If you want to change the alignment of your items then you will need to
  reimplement paintCell().

  Reimplementing this function is probably better e.g. for data you
  retrieve from a database and draw at once, while using
  QTableItem::paint() is probably better e.g.  if you wish these data
  to be stored in a data structure in the table.
*/

void QTable::paintCell( QPainter* p, int row, int col,
			const QRect &cr, bool selected )
{
    if ( cr.width() == 0 || cr.height() == 0 )
	return;
    if ( selected &&
	 row == curRow &&
	 col == curCol )
	selected = FALSE;

    int w = cr.width();
    int h = cr.height();
    int x2 = w - 1;
    int y2 = h - 1;


    QTableItem *itm = item( row, col );
    if ( itm ) {
	p->save();
	itm->paint( p, colorGroup(), cr, selected );
	p->restore();
    } else {
	p->fillRect( 0, 0, w, h, selected ? colorGroup().brush( QColorGroup::Highlight ) : colorGroup().brush( QColorGroup::Base ) );
    }

    if ( sGrid ) {
	// Draw our lines
	QPen pen( p->pen() );
	p->setPen( colorGroup().mid() );
	p->drawLine( x2, 0, x2, y2 );
	p->drawLine( 0, y2, x2, y2 );
	p->setPen( pen );
    }
}

/*! Draws the focus rectangle of the current cell (see currentRow(),
  currentColumn()). The painter \a p is already translated to the
  cell's origin, while \a cr specifies the cell's geometry in contents
  coordinates.
*/

void QTable::paintFocus( QPainter *p, const QRect &cr )
{
    QRect focusRect( 0, 0, cr.width(), cr.height() );
    p->setPen( QPen( black, 1 ) );
    p->setBrush( NoBrush );
    bool focusEdited = FALSE;
    if ( edMode != NotEditing &&
	 curRow == editRow && curCol == editCol )
	focusEdited = TRUE;
    if ( !focusEdited ) {
	QTableItem *i = item( curRow, curCol );
	focusEdited = ( i &&
			( i->editType() == QTableItem::Always ||
			  ( i->editType() == QTableItem::WhenCurrent &&
			    curRow == i->row() && curCol == i->col() ) ) );
    }
    p->drawRect( focusRect.x(), focusRect.y(), focusRect.width() - 1, focusRect.height() - 1 );
    if ( !focusEdited ) {
	p->drawRect( focusRect.x() - 1, focusRect.y() - 1, focusRect.width() + 1, focusRect.height() + 1 );
    } else {
	p->drawRect( focusRect.x() - 1, focusRect.y() - 1, focusRect.width() + 1, focusRect.height() + 1 );
    }
}

/*!  This function fills the rectangular \a cx, \a cy, \a cw, \a ch with the
  background color. paintEmptyArea() is invoked by drawContents() to erase
  or fill unused areas.
*/

void QTable::paintEmptyArea( QPainter *p, int cx, int cy, int cw, int ch )
{
    // Region of the rect we should draw
//    contentsToViewport( cx, cy, cx, cy );
    QRegion reg( QRect( cx, cy, cw, ch ) );
    // Subtract the table from it
    reg = reg.subtract( QRect( QPoint( 0, 0 ), tableSize() ) );
//    reg = reg.subtract( QRect( contentsToViewport( QPoint( 0, 0 ) ), tableSize() ) );

    // And draw the rectangles (transformed as needed)
    QArray<QRect> r = reg.rects();
    for ( int i = 0; i < (int)r.count(); ++i)
        p->fillRect( QRect(viewportToContents2(r[i].topLeft()),r[i].size()),
                        colorGroup().brush( QColorGroup::Base ) );
}

/*!  Returns the QTableItem representing the contents of the cell \a
  row, \a col. If \a row or \a col are out of range or no content has
  been set for this cell so far, item() returns 0.
*/

QTableItem *QTable::item( int row, int col ) const
{
    if ( row < 0 || col < 0 || row > numRows() - 1 ||
	 col > numCols() - 1 || row * col >= (int)contents.size() )
	return 0;

    return contents[ indexOf( row, col ) ];	// contents array lookup
}

/*!  Sets the content for the cell \a row, \a col. If cell item
  already exists in that position, the old one is deleted.

  setItem() also repaints the cell.
*/

void QTable::setItem( int row, int col, QTableItem *item )
{
    if ( !item )
	return;

    if ( (int)contents.size() != numRows() * numCols() )
	resizeData( numRows() * numCols() );

    int orow = item->row();
    int ocol = item->col();
    clearCell( row, col );

    contents.insert( indexOf( row, col ), item );
    item->setRow( row );
    item->setCol( col );
    updateCell( row, col );
    item->updateEditor( orow, ocol );
}

/*!  Removes the QTableItem in position \a row, \a col.
*/

void QTable::clearCell( int row, int col )
{
    if ( (int)contents.size() != numRows() * numCols() )
	resizeData( numRows() * numCols() );
    contents.remove( indexOf( row, col ) );
}

/*!  Sets the text in cell \a row, \a col to \a text. If no
  QTableItem belongs to the cell yet, an item is created.
*/

void QTable::setText( int row, int col, const QString &text )
{
    QTableItem *itm = item( row, col );
    if ( itm ) {
	itm->setText( text );
	updateCell( row, col );
    } else {
	QTableItem *i = new QTableItem( this, QTableItem::OnTyping,
					text, QPixmap() );
	setItem( row, col, i );
    }
}

/*!  Sets the pixmap in cell \a row, \a col to \a pix. If no
  QTableItem belongs to the cell yet, an item is created.
*/

void QTable::setPixmap( int row, int col, const QPixmap &pix )
{
    QTableItem *itm = item( row, col );
    if ( itm ) {
	itm->setPixmap( pix );
	updateCell( row, col );
    } else {
	QTableItem *i = new QTableItem( this, QTableItem::OnTyping,
					QString::null, pix );
	setItem( row, col, i );
    }
}

/*!  Returns the text in cell \a row, \a col, or an empty string if
  the relevant item does not exist or includes no text.
*/

QString QTable::text( int row, int col ) const
{
    QTableItem *itm = item( row, col );
    if ( itm )
	return itm->text();
    return QString::null;
}

/*!  Returns the pixmap set for the cell \a row, \a col, or a
  null-pixmap if the cell contains no pixmap.
*/

QPixmap QTable::pixmap( int row, int col ) const
{
    QTableItem *itm = item( row, col );
    if ( itm )
	return itm->pixmap();
    return QPixmap();
}

/*!  Moves the focus to the cell at position \a row, \a col.

  \sa currentRow() currentColumn()
*/

void QTable::setCurrentCell( int row, int col )
{
    QTableItem *itm = item( row, col );
    QTableItem *oldIitem = item( curRow, curCol );
    if ( itm && itm->rowSpan() > 1 && oldIitem == itm && itm->row() != row ) {
	if ( row > curRow )
	    row = itm->row() + itm->rowSpan();
	else if ( row < curRow )
	    row = QMAX( 0, itm->row() - 1 );
    }
    if ( itm && itm->colSpan() > 1 && oldIitem == itm && itm->col() != col ) {
	if ( col > curCol )
	    col = itm->col() + itm->colSpan();
	else if ( col < curCol )
	    col = QMAX( 0, itm->col() - 1 );
    }
    if ( curRow != row || curCol != col ) {
	itm = oldIitem;
	if ( itm && itm->editType() != QTableItem::Always )
	    endEdit( curRow, curCol, TRUE, FALSE );
	int oldRow = curRow;
	int oldCol = curCol;
	curRow = row;
	curCol = col;
	repaintCell( oldRow, oldCol );
	repaintCell( curRow, curCol );
	ensureCellVisible( curRow, curCol );
	emit currentChanged( row, col );
	if ( !isColumnSelected( oldCol ) && !isRowSelected( oldRow ) ) {
	    topHeader->setSectionState( oldCol, QTableHeader::Normal );
	    leftHeader->setSectionState( oldRow, QTableHeader::Normal );
	}
	topHeader->setSectionState( curCol, isColumnSelected( curCol, TRUE ) ? QTableHeader::Selected : QTableHeader::Bold );
	leftHeader->setSectionState( curRow, isRowSelected( curRow, TRUE ) ? QTableHeader::Selected : QTableHeader::Bold );
	itm = item( curRow, curCol );

	if ( cellWidget( oldRow, oldCol ) &&
	     cellWidget( oldRow, oldCol )->hasFocus() )
	    viewport()->setFocus();

	if ( itm && itm->editType() == QTableItem::WhenCurrent ) {
	    if ( beginEdit( curRow, curCol, FALSE ) ) {
		edMode = Editing;
		editRow = row;
		editCol = col;
	    }
	} else if ( itm && itm->editType() == QTableItem::Always ) {
	    if ( cellWidget( itm->row(), itm->col() ) )
		cellWidget( itm->row(), itm->col() )->setFocus();
	}
    }
}

/*!  Scrolls the table until the cell \a row, \a col becomes
  visible.
*/

void QTable::ensureCellVisible( int row, int col )
{
    int cw = columnWidth( col );
    int rh = rowHeight( row );
    ensureVisible( columnPos( col ) + cw / 2, rowPos( row ) + rh / 2, cw / 2, rh / 2 );
}

/*!  Checks whether the cell at position \a row, \a col is selected.
*/

bool QTable::isSelected( int row, int col ) const
{
    QListIterator<QTableSelection> it( selections );
    QTableSelection *s;
    while ( ( s = it.current() ) != 0 ) {
	++it;
	if ( s->isActive() &&
	     row >= s->topRow() &&
	     row <= s->bottomRow() &&
	     col >= s->leftCol() &&
	     col <= s->rightCol() )
	    return TRUE;
	if ( row == currentRow() && col == currentColumn() )
	    return TRUE;
    }
    return FALSE;
}

/*! Returns TRUE if \a row is selected, and FALSE otherwise.

  If \a full is TRUE, the entire row must be selected for this
  function to return TRUE.  If \a full is FALSE, at least one cell in
  \a row must be selected.
*/

bool QTable::isRowSelected( int row, bool full ) const
{
    if ( !full ) {
	QListIterator<QTableSelection> it( selections );
	QTableSelection *s;
	while ( ( s = it.current() ) != 0 ) {
	    ++it;
	    if ( s->isActive() &&
		 row >= s->topRow() &&
		 row <= s->bottomRow() )
	    return TRUE;
	if ( row == currentRow() )
	    return TRUE;
	}
    } else {
	QListIterator<QTableSelection> it( selections );
	QTableSelection *s;
	while ( ( s = it.current() ) != 0 ) {
	    ++it;
	    if ( s->isActive() &&
		 row >= s->topRow() &&
		 row <= s->bottomRow() &&
		 s->leftCol() == 0 &&
		 s->rightCol() == numCols() - 1 )
		return TRUE;
	}
    }
    return FALSE;
}

/*! Returns TRUE if column \a col is selected, and FALSE otherwise.

  If \a full is TRUE, the entire column must be selected for this
  function to return TRUE.  If \a full is FALSE, at least one cell in
  \a col must be selected.
*/

bool QTable::isColumnSelected( int col, bool full ) const
{
    if ( !full ) {
	QListIterator<QTableSelection> it( selections );
	QTableSelection *s;
	while ( ( s = it.current() ) != 0 ) {
	    ++it;
	    if ( s->isActive() &&
		 col >= s->leftCol() &&
		 col <= s->rightCol() )
	    return TRUE;
	if ( col == currentColumn() )
	    return TRUE;
	}
    } else {
	QListIterator<QTableSelection> it( selections );
	QTableSelection *s;
	while ( ( s = it.current() ) != 0 ) {
	    ++it;
	    if ( s->isActive() &&
		 col >= s->leftCol() &&
		 col <= s->rightCol() &&
		 s->topRow() == 0 &&
		 s->bottomRow() == numRows() - 1 )
		return TRUE;
	}
    }
    return FALSE;
}

/*!  Returns the number of selections.
*/

int QTable::numSelections() const
{
    return selections.count();
}

/*!  Returns selection number \a num, or an empty QTableSelection
if \a num is out of range (see QTableSelection::isNull()).
*/

QTableSelection QTable::selection( int num ) const
{
    if ( num < 0 || num >= (int)selections.count() )
	return QTableSelection();

    QTableSelection *s = ( (QTable*)this )->selections.at( num );
    return *s;
}

/*!  Adds a selection described by \a s to the table and returns its
 number or -1 if the selection is invalid. Don't forget
 to call QTableSelection::init() and
 QTableSelection::expandTo() to make it valid (see also
 QTableSelection::isActive()).
*/

int QTable::addSelection( const QTableSelection &s )
{
    if ( !s.isActive() )
	return -1;
    QTableSelection *sel = new QTableSelection( s );
    selections.append( sel );
    viewport()->repaint( FALSE );
    return selections.count() - 1;
}

/*!  Removes the selection matching the values of \a s from the
  table.
*/

void QTable::removeSelection( const QTableSelection &s )
{
    for ( QTableSelection *sel = selections.first(); sel; sel = selections.next() ) {
	if ( s == *sel ) {
	    selections.removeRef( sel );
	    viewport()->repaint( FALSE );
	}
    }
}

/*!  Removes selection number \a num.
*/

void QTable::removeSelection( int num )
{
    if ( num < 0 || num >= (int)selections.count() )
	return;

    QTableSelection *s = selections.at( num );
    selections.removeRef( s );
    viewport()->repaint( FALSE );
}

/*!  Returns the number of the current selection or -1 if there is
  none.
*/

int QTable::currentSelection() const
{
    if ( !currentSel )
	return -1;
    return ( (QTable*)this )->selections.findRef( currentSel );
}

/*!  \reimp
*/

void QTable::contentsMousePressEvent( QMouseEvent* e )
{
    int tmpRow = rowAt( e->pos().y() );
    int tmpCol = columnAt( e->pos().x() );
    if ( e->button() != LeftButton ) {
	emit pressed( tmpRow, tmpCol, e->button(), e->pos() );
	return;
    }

    mousePressed = TRUE;
    if ( isEditing() )
 	endEdit( editRow, editCol, TRUE, edMode != Editing );

    pressedRow = tmpRow;
    pressedCol = tmpCol;
    fixRow( tmpRow, e->pos().y() );
    fixCol( tmpCol, e->pos().x() );
    if ( e->button() != LeftButton ) {
	emit pressed( tmpRow, tmpCol, e->button(), e->pos() );
	return;
    }

    if ( ( e->state() & ShiftButton ) == ShiftButton ) {
	if ( selMode != NoSelection ) {
	    if ( !currentSel ) {
		currentSel = new QTableSelection();
		selections.append( currentSel );
		currentSel->init( curRow, curCol );
	    }
	    QTableSelection oldSelection = *currentSel;
	    currentSel->expandTo( tmpRow, tmpCol );
	    repaintSelections( &oldSelection, currentSel );
	    emit selectionChanged();
	}
	setCurrentCell( tmpRow, tmpCol );
    } else if ( ( e->state() & ControlButton ) == ControlButton ) {
	if ( selMode != NoSelection ) {
	    if ( selMode == Single )
		clearSelection();
	    currentSel = new QTableSelection();
	    selections.append( currentSel );
	    currentSel->init( tmpRow, tmpCol );
	    emit selectionChanged();
	}
	setCurrentCell( tmpRow, tmpCol );
    } else {
	setCurrentCell( tmpRow, tmpCol );
	clearSelection();
	if ( selMode != NoSelection ) {
	    currentSel = new QTableSelection();
	    selections.append( currentSel );
	    currentSel->init( tmpRow, tmpCol );
	    emit selectionChanged();
	}
    }

    emit pressed( tmpRow, tmpCol, e->button(), e->pos() );
}

/*!  \reimp
*/

void QTable::contentsMouseDoubleClickEvent( QMouseEvent *e )
{
    if ( e->button() != LeftButton )
	return;
    int tmpRow = rowAt( e->pos().y() );
    int tmpCol = columnAt( e->pos().x() );
    if ( tmpRow != -1 && tmpCol != -1 ) {
	if ( beginEdit( tmpRow, tmpCol, FALSE ) ) {
	    edMode = Editing;
	    editRow = tmpRow;
	    editCol = tmpCol;
	}
    }

    emit doubleClicked( tmpRow, tmpCol, e->button(), e->pos() );
}

/*!  \reimp
*/

void QTable::contentsMouseMoveEvent( QMouseEvent *e )
{
    if ( !mousePressed )
	return;
    int tmpRow = rowAt( e->pos().y() );
    int tmpCol = columnAt( e->pos().x() );
    fixRow( tmpRow, e->pos().y() );
    fixCol( tmpCol, e->pos().x() );

    QPoint pos = mapFromGlobal( e->globalPos() );
    pos -= QPoint( leftHeader->width(), topHeader->height() );
    autoScrollTimer->stop();
    doAutoScroll();
    if ( pos.x() < 0 || pos.x() > visibleWidth() || pos.y() < 0 || pos.y() > visibleHeight() )
	autoScrollTimer->start( 100, TRUE );
    emit doubleClicked( tmpRow, tmpCol, e->button(), e->globalPos() );
}

/*! \internal
*/

void QTable::doAutoScroll()
{
    if ( !mousePressed )
	return;
    QPoint pos = QCursor::pos();
    pos = mapFromGlobal( pos );
    pos -= QPoint( leftHeader->width(), topHeader->height() );

    int tmpRow = curRow;
    int tmpCol = curCol;
    if ( pos.y() < 0 )
	tmpRow--;
    else if ( pos.y() > visibleHeight() )
	tmpRow++;
    if ( pos.x() < 0 )
	tmpCol--;
    else if ( pos.x() > visibleWidth() )
	tmpCol++;

    pos += QPoint( contentsX(), contentsY() );
    if ( tmpRow == curRow )
	tmpRow = rowAt( pos.y() );
    if ( tmpCol == curCol )
	tmpCol = columnAt( pos.x() );
    pos -= QPoint( contentsX(), contentsY() );

    fixRow( tmpRow, pos.y() );
    fixCol( tmpCol, pos.x() );

    if ( tmpRow < 0 || tmpRow > numRows() - 1 )
	tmpRow = currentRow();
    if ( tmpCol < 0 || tmpCol > numCols() - 1 )
	tmpCol = currentColumn();

    ensureCellVisible( tmpRow, tmpCol );

    if ( currentSel && selMode != NoSelection ) {
	QTableSelection oldSelection = *currentSel;
	currentSel->expandTo( tmpRow, tmpCol );
	setCurrentCell( tmpRow, tmpCol );
	repaintSelections( &oldSelection, currentSel );
	emit selectionChanged();
    } else {
	setCurrentCell( tmpRow, tmpCol );
    }

    if ( pos.x() < 0 || pos.x() > visibleWidth() || pos.y() < 0 || pos.y() > visibleHeight() )
	autoScrollTimer->start( 100, TRUE );
}

/*! \reimp
*/

void QTable::contentsMouseReleaseEvent( QMouseEvent *e )
{
    if ( pressedRow == curRow && pressedCol == curCol )
	emit clicked( curRow, curCol, e->button(), e->pos() );
    if ( e->button() != LeftButton )
	return;
    mousePressed = FALSE;
    autoScrollTimer->stop();
}

/*!  \reimp
*/

bool QTable::eventFilter( QObject *o, QEvent *e )
{
    if ( !o || !e )
	return QScrollView::eventFilter( o, e );

    QWidget *editorWidget = cellWidget( editRow, editCol );
    switch ( e->type() ) {
    case QEvent::KeyPress: {
	QTableItem *itm = item( curRow, curCol );

	if ( isEditing() && editorWidget && o == editorWidget ) {
	    itm = item( editRow, editCol );
	    QKeyEvent *ke = (QKeyEvent*)e;
	    if ( ke->key() == Key_Escape ) {
		if ( !itm || itm->editType() == QTableItem::OnTyping )
		    endEdit( editRow, editCol, FALSE, edMode != Editing );
		return TRUE;
	    }

	    if ( ke->key() == Key_Return || ke->key() == Key_Enter ) {
		if ( !itm || itm->editType() == QTableItem::OnTyping )
		    endEdit( editRow, editCol, TRUE, edMode != Editing );
		activateNextCell();
		return TRUE;
	    }

	    if ( ( edMode == Replacing ||
		   itm && itm->editType() == QTableItem::WhenCurrent ) &&
		 ( ke->key() == Key_Up || ke->key() == Key_Prior ||
		   ke->key() == Key_Home || ke->key() == Key_Down ||
		   ke->key() == Key_Next || ke->key() == Key_End ||
		   ke->key() == Key_Left || ke->key() == Key_Right ) ) {
		if ( !itm || itm->editType() == QTableItem::OnTyping )
		    endEdit( editRow, editCol, TRUE, edMode != Editing );
		keyPressEvent( ke );
		return TRUE;
	    }
	} else {
	    QObjectList *l = viewport()->queryList( "QWidget" );
	    if ( l && l->find( o ) != -1 ) {
		QKeyEvent *ke = (QKeyEvent*)e;
		if ( ( ke->state() & ControlButton ) == ControlButton ||
		     ( ke->key() != Key_Left && ke->key() != Key_Right &&
		       ke->key() != Key_Up && ke->key() != Key_Down &&
		       ke->key() != Key_Prior && ke->key() != Key_Next &&
		       ke->key() != Key_Home && ke->key() != Key_End ) )
		    return FALSE;
		keyPressEvent( (QKeyEvent*)e );
		return TRUE;
	    }
	    delete l;
	}

	} break;
    case QEvent::FocusOut:
	if ( o == this || o == viewport() )
	    return TRUE;
	if ( isEditing() && editorWidget && o == editorWidget && ( (QFocusEvent*)e )->reason() != QFocusEvent::Popup ) {
	    QTableItem *itm = item( editRow, editCol );
 	    if ( !itm || itm->editType() == QTableItem::OnTyping ) {
 		endEdit( editRow, editCol, TRUE, edMode != Editing );
		return TRUE;
	    }
	}
	break;
    case QEvent::FocusIn:
 	if ( o == this || o == viewport() ) {
	    if ( isEditing() && editorWidget )
		editorWidget->setFocus();
	    return TRUE;
	}
	break;
    default:
	break;
    }

    return QScrollView::eventFilter( o, e );
}

/*!  \reimp
*/

void QTable::keyPressEvent( QKeyEvent* e )
{
    if ( isEditing() && item( editRow, editCol ) &&
	 item( editRow, editCol )->editType() == QTableItem::OnTyping )
	return;

    int tmpRow = curRow;
    int tmpCol = curCol;
    int oldRow = tmpRow;
    int oldCol = tmpCol;

    bool navigationKey = FALSE;
    int r;
    switch ( e->key() ) {
    case Key_Left:
	tmpCol = QMAX( 0, tmpCol - 1 );
	navigationKey = TRUE;
    	break;
    case Key_Right:
	tmpCol = QMIN( numCols() - 1, tmpCol + 1 );
	navigationKey = TRUE;
	break;
    case Key_Up:
	//tmpRow = QMAX( 0, tmpRow - 1 );
	while ( --tmpRow >= 0 )
	    if ( !isRowHidden( tmpRow ) )
		break;
	if ( tmpRow < 0 )
	    tmpRow = curRow;
	navigationKey = TRUE;
	break;
    case Key_Down:
	// tmpRow = QMIN( numRows() - 1, tmpRow + 1 );
	while ( ++tmpRow < numRows() )
	    if ( !isRowHidden( tmpRow ) )
		break;
	if ( tmpRow >= numRows() )
	    tmpRow = curRow;
	navigationKey = TRUE;
	break;
    case Key_Prior:
	r = QMAX( 0, rowAt( rowPos( tmpRow ) - visibleHeight() ) );
	if ( r < tmpRow )
	    tmpRow = r;
	navigationKey = TRUE;
	break;
    case Key_Next:
	r = QMIN( numRows() - 1, rowAt( rowPos( tmpRow ) + visibleHeight() ) );
	if ( r > tmpRow )
	    tmpRow = r;
	else
	    tmpRow = numRows() - 1;
	navigationKey = TRUE;
	break;
    case Key_Home:
	tmpRow = 0;
	navigationKey = TRUE;
	break;
    case Key_End:
	tmpRow = numRows() - 1;
	navigationKey = TRUE;
	break;
    case Key_F2:
	if ( beginEdit( tmpRow, tmpCol, FALSE ) ) {
	    edMode = Editing;
	    editRow = tmpRow;
	    editCol = tmpCol;
	}
	break;
    default: // ... or start in-place editing
	if ( e->text()[ 0 ].isPrint() ) {
	    QTableItem *itm = item( tmpRow, tmpCol );
	    if ( !itm || itm->editType() == QTableItem::OnTyping ) {
		QWidget *w;
		if ( ( w = beginEdit( tmpRow, tmpCol,
				      itm ? itm->isReplaceable() : TRUE ) ) ) {
		    edMode = ( !itm || itm && itm->isReplaceable()
			       ? Replacing : Editing );
		    editRow = tmpRow;
		    editCol = tmpCol;
		    QApplication::sendEvent( w, e );
		}
	    }
	}
    }

    if ( navigationKey ) {
	if ( ( e->state() & ShiftButton ) == ShiftButton &&
	     selMode != NoSelection ) {
	    setCurrentCell( tmpRow, tmpCol );
	    bool justCreated = FALSE;
	    if ( !currentSel ) {
		justCreated = TRUE;
		currentSel = new QTableSelection();
		selections.append( currentSel );
		currentSel->init( oldRow, oldCol );
	    }
	    QTableSelection oldSelection = *currentSel;
	    currentSel->expandTo( tmpRow, tmpCol );
	    repaintSelections( justCreated ? 0 : &oldSelection, currentSel );
	    emit selectionChanged();
	} else {
	    clearSelection();
	    setCurrentCell( tmpRow, tmpCol );
	}
    } else {
	setCurrentCell( tmpRow, tmpCol );
    }
}

/*!  \reimp
*/

void QTable::focusInEvent( QFocusEvent* )
{
}


/*!  \reimp
*/

void QTable::focusOutEvent( QFocusEvent* )
{
}

/*!  \reimp
*/

QSize QTable::sizeHint() const
{
    QSize s = tableSize();
    if ( s.width() < 500 && s.height() < 500 )
	return QSize( tableSize().width() + leftMargin() + 5,
		      tableSize().height() + topMargin() + 5 );
    return QScrollView::sizeHint();
}

/*!  \reimp
*/

void QTable::resizeEvent( QResizeEvent *e )
{
    QScrollView::resizeEvent( e );
    updateGeometries();
}

/*!  \reimp
*/

void QTable::showEvent( QShowEvent *e )
{
    QScrollView::showEvent( e );
    QRect r( cellGeometry( numRows() - 1, numCols() - 1 ) );
    resizeContents( r.right() + 1, r.bottom() + 1 );
    updateGeometries();
}

static bool inUpdateCell = FALSE;

/*!  Repaints the cell at position \a row, \a col.
*/

void QTable::updateCell( int row, int col )
{
    if ( inUpdateCell || row == -1 || col == -1 )
	return;
    inUpdateCell = TRUE;
    QRect cg = cellGeometry( row, col );
    QRect r( contentsToViewport( QPoint( cg.x() - 2, cg.y() - 2 ) ),
	     QSize( cg.width() + 4, cg.height() + 4 ) );
    QApplication::postEvent( viewport(), new QPaintEvent( r, FALSE ) );
    inUpdateCell = FALSE;
}

void QTable::repaintCell( int row, int col )
{
    if ( row == -1 || col == -1 )
	return;
    QRect cg = cellGeometry( row, col );
    QRect r( QPoint( cg.x() - 2, cg.y() - 2 ),
	     QSize( cg.width() + 4, cg.height() + 4 ) );
    repaintContents( r, FALSE );
}

void QTable::contentsToViewport2( int x, int y, int& vx, int& vy )
{
    const QPoint v = contentsToViewport2( QPoint( x, y ) );
    vx = v.x();
    vy = v.y();
}

QPoint QTable::contentsToViewport2( const QPoint &p )
{
    return QPoint( p.x() - contentsX(),
		   p.y() - contentsY() );
}

QPoint QTable::viewportToContents2( const QPoint& vp )
{
    return QPoint( vp.x() + contentsX(),
		   vp.y() + contentsY() );
}

void QTable::viewportToContents2( int vx, int vy, int& x, int& y )
{
    const QPoint c = viewportToContents2( QPoint( vx, vy ) );
    x = c.x();
    y = c.y();
}

/*!  This function should be called whenever the column width of \a col
  has been changed. It will then rearrange the content appropriately.
*/

void QTable::columnWidthChanged( int col )
{
    updateContents( columnPos( col ), 0, contentsWidth(), contentsHeight() );
    QSize s( tableSize() );
    int w = contentsWidth();
    resizeContents( s.width(), s.height() );
    if ( contentsWidth() < w )
	repaintContents( s.width(), 0,
			 w - s.width() + 1, contentsHeight(), TRUE );
    else
	repaintContents( w, 0,
			 s.width() - w + 1, contentsHeight(), FALSE );

    updateGeometries();
    qApp->processEvents();

    for ( int j = col; j < numCols(); ++j ) {
	for ( int i = 0; i < numRows(); ++i ) {
	    QWidget *w = cellWidget( i, j );
	    if ( !w )
		continue;
	    moveChild( w, columnPos( j ), rowPos( i ) );
	    w->resize( columnWidth( j ) - 1, rowHeight( i ) - 1 );
	}
 	qApp->processEvents();
    }
}

/*!  Call this function whenever the height of row \a row has
  changed in order to rearrange its contents.
*/

void QTable::rowHeightChanged( int row )
{
    updateContents( 0, rowPos( row ), contentsWidth(), contentsHeight() );
    QSize s( tableSize() );
    int h = contentsHeight();
    resizeContents( s.width(), s.height() );
    if ( contentsHeight() < h )
	repaintContents( 0, contentsHeight(),
			 contentsWidth(), h - s.height() + 1, TRUE );
    else
	repaintContents( 0, h,
			 contentsWidth(), s.height() - h + 1, FALSE );

    updateGeometries();
    qApp->processEvents();

    for ( int j = row; j < numRows(); ++j ) {
	for ( int i = 0; i < numCols(); ++i ) {
	    QWidget *w = cellWidget( j, i );
	    if ( !w )
		continue;
	    moveChild( w, columnPos( i ), rowPos( j ) );
	    w->resize( columnWidth( i ) - 1, rowHeight( j ) - 1 );
	}
 	qApp->processEvents();
    }
}

void QTable::updateRowWidgets( int row )
{
    for ( int i = 0; i < numCols(); ++i ) {
	QWidget *w = cellWidget( row, i );
	if ( !w )
	    continue;
	moveChild( w, columnPos( i ), rowPos( row ) );
	w->resize( columnWidth( i ) - 1, rowHeight( row ) - 1 );
    }
}

void QTable::updateColWidgets( int col )
{
    for ( int i = 0; i < numRows(); ++i ) {
	QWidget *w = cellWidget( i, col );
	if ( !w )
	    continue;
	moveChild( w, columnPos( col ), rowPos( i ) );
	w->resize( columnWidth( col ) - 1, rowHeight( i ) - 1 );
    }
}
//ed: the end...

/*!  This function is called if the order of the columns has been
  changed. If you want to change the order programmatically, call
  swapRows() or swapColumns().
*/

void QTable::columnIndexChanged( int, int, int )
{
    repaintContents( contentsX(), contentsY(),
		     visibleWidth(), visibleHeight(), FALSE );
}

/*!  This function is called if the order of the rows has been
  changed. If you want to change the order programmatically, call
  swapRows() or swapColumns().
*/

void QTable::rowIndexChanged( int, int, int )
{
    repaintContents( contentsX(), contentsY(),
		     visibleWidth(), visibleHeight(), FALSE );
}

/*!  This function is called when the column \c col has been
  clicked. The default implementation sorts this column if
  sorting() is TRUE
*/

void QTable::columnClicked( int col )
{
    if ( !sorting() )
	return;

    if ( col == lastSortCol ) {
	asc = !asc;
    } else {
	lastSortCol = col;
	asc = TRUE;
    }

    sortColumn( lastSortCol, asc );
}

/*!  If \a b is set to TRUE, clicking on the header of a column sorts
  this column.

  \sa sortColumn()
*/

void QTable::setSorting( bool b )
{
    doSort = b;
}

/*!  Returns wheather clicking on a column header sorts the column.

 \sa setSorting()
*/

bool QTable::sorting() const
{
    return doSort;
}

static bool inUpdateGeometries = FALSE;

/*!  This function updates the geometries of the left and top header.
*/

void QTable::updateGeometries()
{
    if ( inUpdateGeometries )
	return;
    inUpdateGeometries = TRUE;
    QSize ts = tableSize();
    if ( topHeader->offset() &&
	 ts.width() < topHeader->offset() + topHeader->width() )
	horizontalScrollBar()->setValue( ts.width() - topHeader->width() );
    if ( leftHeader->offset() &&
	 ts.height() < leftHeader->offset() + leftHeader->height() )
	verticalScrollBar()->setValue( ts.height() - leftHeader->height() );

    leftHeader->setGeometry( frameWidth(), topMargin() + frameWidth(),
			     leftMargin(), visibleHeight() );
    topHeader->setGeometry( leftMargin() + frameWidth(), frameWidth(),
			    visibleWidth(), topMargin() );
    inUpdateGeometries = FALSE;
}

/*!  Returns the width of the column \a col.
*/

int QTable::columnWidth( int col ) const
{
    return topHeader->sectionSize( col );
}

/*!  Returns the height of the row \a row.
*/

int QTable::rowHeight( int row ) const
{
    return leftHeader->sectionSize( row );
}

/*!  Returns the x-position of the column \a col in contents
  coordinates
*/

int QTable::columnPos( int col ) const
{
    return topHeader->sectionPos( col );
}

/*!  Returns the y-position of the row \a row in contents coordinates
*/

int QTable::rowPos( int row ) const
{
    return leftHeader->sectionPos( row );
}

/*!  Returns the column which is at \a pos. \a pos has to be given in
  contents coordinates.
*/

int QTable::columnAt( int pos ) const
{
    return topHeader->sectionAt( pos );
}

/*!  Returns the row which is at \a pos. \a pos has to be given in
  contents coordinates.
*/

int QTable::rowAt( int pos ) const
{
    return leftHeader->sectionAt( pos );
}

/*!  Returns the bounding rect of the cell \a row, \a col in contents
  coordinates.
*/

QRect QTable::cellGeometry( int row, int col ) const
{
    QTableItem *itm = item( row, col );
    if ( !itm || itm->rowSpan() == 1 && itm->colSpan() == 1 )
	return QRect( columnPos( col ), rowPos( row ),
		      columnWidth( col ), rowHeight( row ) );

    while ( row != itm->row() )
	row--;
    while ( col != itm->col() )
	col--;

    QRect rect( columnPos( col ), rowPos( row ),
		columnWidth( col ), rowHeight( row ) );

    for ( int r = 1; r < itm->rowSpan(); ++r )
	    rect.setHeight( rect.height() + rowHeight( r + row ) );
    for ( int c = 1; c < itm->colSpan(); ++c )
	rect.setWidth( rect.width() + columnWidth( c + col ) );

    return rect;
}

/*!  Returns the size of the table (same as the right/bottom edge of
  the last table cell.
*/

QSize QTable::tableSize() const
{
    return QSize( columnPos( numCols() - 1 ) + columnWidth( numCols() - 1 ),
		  rowPos( numRows() - 1 ) + rowHeight( numRows() - 1 ) );
}

/*!  Returns the number of rows of the table.
*/

int QTable::numRows() const
{
    return leftHeader->count();
}

/*!  Returns the number of columns of the table.
*/

int QTable::numCols() const
{
    return topHeader->count();
}

/*!  Sets the number of rows to \a r.
*/

void QTable::setNumRows( int r )
{
    if ( r < 0 )
	return;
    QVector<QTableItem> tmp;
    tmp.resize( contents.size() );
    int i;
    for ( i = 0; i < (int)tmp.size(); ++i ) {
	QTableItem *item = contents[ i ];
	if ( item && indexOf( item->row(), item->col() ) == i )
	    tmp.insert( i, item );
	else
	    tmp.insert( i, 0 );
    }
    bool isUpdatesEnabled = leftHeader->isUpdatesEnabled();
    leftHeader->setUpdatesEnabled( FALSE );
    bool updateBefore = r < numRows();
    if ( r > numRows() ) {
	clearSelection( FALSE );
	while ( numRows() < r ) {
	    leftHeader->addLabel( QString::number( numRows() + 1 ), 20 );
	}
    } else {
	clearSelection( FALSE );
	while ( numRows() > r )
	    leftHeader->removeLabel( numRows() - 1 );
    }

    contents.setAutoDelete( FALSE );
    contents.clear();
    contents.setAutoDelete( TRUE );
    resizeData( numRows() * numCols() );

    for ( i = 0; i < (int)tmp.size(); ++i ) {
	QTableItem *it = tmp [ i ];
	int idx = it ? indexOf( it->row(), it->col() ) : 0;
	if ( it && (uint)idx < contents.size() )
	    contents.insert( idx, it );
    }

    QRect r2( cellGeometry( numRows() - 1, numCols() - 1 ) );
    resizeContents( r2.right() + 1, r2.bottom() + 1 );
    updateGeometries();
    leftHeader->setUpdatesEnabled( isUpdatesEnabled );
    if ( updateBefore )
	repaintContents( contentsX(), contentsY(),
			 visibleWidth(), visibleHeight(), TRUE );
    else
	repaintContents( contentsX(), contentsY(),
			 visibleWidth(), visibleHeight(), FALSE );
    if ( isUpdatesEnabled )
	leftHeader->update();
}

/*!  Sets the number of columns to \a c.
*/

void QTable::setNumCols( int c )
{
    if ( c < 0 )
	return;
    QVector<QTableItem> tmp;
    tmp.resize( contents.size() );
    int i;
    for ( i = 0; i < (int)tmp.size(); ++i ) {
	QTableItem *item = contents[ i ];
	if ( item && indexOf( item->row(), item->col() ) == i )
	    tmp.insert( i, item );
	else
	    tmp.insert( i, 0 );
    }
    bool isUpdatesEnabled = topHeader->isUpdatesEnabled();
    topHeader->setUpdatesEnabled( FALSE );
    bool updateBefore = c < numCols();
    if ( c > numCols() ) {
	clearSelection( FALSE );
	while ( numCols() < c ) {
	    topHeader->addLabel( QString::number( numCols() + 1 ), 100 );
	}
    } else {
	clearSelection( FALSE );
	while ( numCols() > c )
	    topHeader->removeLabel( numCols() - 1 );
    }

    contents.setAutoDelete( FALSE );
    contents.clear();
    contents.setAutoDelete( TRUE );
    resizeData( numRows() * numCols() );

    for ( i = 0; i < (int)tmp.size(); ++i ) {
	QTableItem *it = tmp[ i ];
	int idx = it ? indexOf( it->row(), it->col() ) : 0;
	if ( it && (uint)idx < contents.size() )
	    contents.insert( idx, it );
    }

    QRect r( cellGeometry( numRows() - 1, numCols() - 1 ) );
    resizeContents( r.right() + 1, r.bottom() + 1 );
    updateGeometries();
    topHeader->setUpdatesEnabled( isUpdatesEnabled );
    if ( updateBefore )
	repaintContents( contentsX(), contentsY(),
			 visibleWidth(), visibleHeight(), TRUE );
    else
	repaintContents( contentsX(), contentsY(),
			 visibleWidth(), visibleHeight(), FALSE );
    if ( isUpdatesEnabled )
	topHeader->update();
}

/*!  This function returns a widget which should be used as editor for
  the cell \a row, \a col. If \a initFromCell is TRUE, the editor is
  used to edit the current content of the cell (so the editor widget
  should be initialized with that content). Otherwise the content of
  this cell will be replaced by a new content which the user will
  enter into the widget which this function should create.

  The default implementation looks if there exists a QTableItem for
  the cell. If this is the case and \a initFromCell is TRUE or
  QTableItem::isReplaceable() of the item is FALSE, the item of
  that cell is asked to create the editor (using QTableItem::createEditor)).

  If this is not the case, a QLineEdit is created as editor.

  So if you want to create your own editor for certain cells,
  implement your own QTableItem and reimplement
  QTableItem::createEditor(). If you want to use a different editor
  than a QLineEdit as default editor, reimplement this function and
  use a code like

  \code
    QTableItem *i = item( row, col );
    if ( initFromCell || i && !i->isReplaceable() )
        return QTable::createEditor( row, col, initFromCell );
    else
        return ...(create your editor)
    \endcode

    So normally you do not need to reimplement this function. But if
    you want e.g. work without QTableItems, you will reimplement this
    function to create the correct editor for the cells.

    The ownership of the editor widget is transferred to the caller.

  Returning 0 here means that the cell is not editable.

  \sa QTableItem::createEditor()
*/

QWidget *QTable::createEditor( int row, int col, bool initFromCell ) const
{
    QWidget *e = 0;

    // the current item in the cell should be edited if possible
    QTableItem *i = item( row, col );
    if ( initFromCell || i && !i->isReplaceable() ) {
	if ( i ) {
	    e = i->createEditor();
	    if ( !e || i->editType() == QTableItem::Never )
		return 0;
	}
    }

    // no contents in the cell yet, so open the default editor
    if ( !e ) {
	e = new QLineEdit( viewport() );
	( (QLineEdit*)e )->setFrame( FALSE );
    }

    return e;
}

/*!  This function is called to start in-place editing of the cell \a
  row, \a col. If \a replace is TRUE the content of this cell will be
  replaced by the content of the editor later, else the current
  content of that cell (if existing) will be edited by the editor.

  This function calls createEditor() to get the editor which should be
  used for editing the cell and after that setCellWidget() to set this
  editor as the widget of that cell.

  \sa createEditor(), setCellWidget(), endEdit()
*/

QWidget *QTable::beginEdit( int row, int col, bool replace )
{
    QTableItem *itm = item( row, col );
    if ( itm && cellWidget( itm->row(), itm->col() ) )
	return 0;
    ensureCellVisible( curRow, curCol );
    QWidget *e = createEditor( row, col, !replace );
    if ( !e )
	return 0;
    setCellWidget( row, col, e );
    e->setFocus();
    updateCell( row, col );
    return e;
}

/*!  This function is called if in-place editing of the cell \a row,
  \a col has to be ended. If \a accept is TRUE the content of the
  editor of this cell has to be transferred to the cell. If \a replace
  is TRUE the current content of that cell should be replaced by the
  content of the editor (this means removing the current QTableItem of
  the cell and creating a new one for the cell), else (if possible)
  the content of the editor should just be set to the existing
  QTableItem of this cell.

  So, if the cell contents should be replaced or if no QTableItem
  exists for the cell yet, setCellContentFromEditor() is called, else
  QTableItem::setContentFromEditor() is called on the QTableItem
  of the cell.

  After that clearCellWidget() is called to get rid of the editor
  widget.

  \sa setCellContentFromEditor(), beginEdit()
*/

void QTable::endEdit( int row, int col, bool accept, bool replace )
{
    QWidget *editor = cellWidget( row, col );
    if ( !editor )
	return;

    if ( !accept ) {
	if ( row == editRow && col == editCol ) {
	    editRow = -1;
	    editCol = -1;
	    edMode = NotEditing;
	}
	clearCellWidget( row, col );
    	updateCell( row, col );
	viewport()->setFocus();
	updateCell( row, col );
	return;
    }

    QTableItem *i = item( row, col );
    if ( replace && i ) {
	clearCell( row, col );
	i = 0;
    }

    if ( !i )
	setCellContentFromEditor( row, col );
    else
	i->setContentFromEditor( editor );

    if ( row == editRow && col == editCol ) {
	editRow = -1;
	editCol = -1;
	edMode = NotEditing;
    }

    viewport()->setFocus();
    updateCell( row, col );

    clearCellWidget( row, col );
    emit valueChanged( row, col );
}

/*!  This function is called to set the contents of the cell \a row,
  \a col from the editor of this cell to this cell. If there existed
  already a QTableItem for this cell, this is removed first (see
  clearCell()).

  If you want to create e.g different QTableItems depending on the
  contents of the editor, you might reimplement this function. Also if
  you want to work without QTableItems, you will reimplement this
  function to set the data which the user entered to your
  datastructure.

  \sa QTableItem::setContentFromEditor()
*/

void QTable::setCellContentFromEditor( int row, int col )
{
    QWidget *editor = cellWidget( row, col );
    if ( !editor )
	return;
    clearCell( row, col );
    if ( editor->inherits( "QLineEdit" ) )
	setText( row, col, ( (QLineEdit*)editor )->text() );
}

/*!  Returns wheather the user is just editing a cell, which is not in
  permanent edit mode (see QTableItem::EditType).
*/

bool QTable::isEditing() const
{
    return edMode != NotEditing;
}

/*!  Maps 2D table to 1D array index.
*/

int QTable::indexOf( int row, int col ) const
{
    return ( row * numCols() ) + col; // mapping from 2D table to 1D array
}

/*!  \internal
*/

void QTable::repaintSelections( QTableSelection *oldSelection,
				QTableSelection *newSelection,
				bool updateVertical, bool updateHorizontal )
{
    if ( oldSelection && *oldSelection == *newSelection )
	return;
    if ( oldSelection && !oldSelection->isActive() )
	oldSelection = 0;

    bool optimize1, optimize2;
    QRect old;
    if ( oldSelection )
	old = rangeGeometry( oldSelection->topRow(),
			     oldSelection->leftCol(),
			     oldSelection->bottomRow(),
			     oldSelection->rightCol(),
			     optimize1 );
    else
	old = QRect( 0, 0, 0, 0 );

    QRect cur = rangeGeometry( newSelection->topRow(),
			       newSelection->leftCol(),
			       newSelection->bottomRow(),
			       newSelection->rightCol(),
			       optimize2 );
    int i;

    if ( !optimize1 || !optimize2 ||
	 old.width() > SHRT_MAX || old.height() > SHRT_MAX ||
	 cur.width() > SHRT_MAX || cur.height() > SHRT_MAX ) {
	QRect rr = cur.unite( old );
	repaintContents( rr, FALSE );
    } else {
	old = QRect( contentsToViewport2( old.topLeft() ), old.size() );
	cur = QRect( contentsToViewport2( cur.topLeft() ), cur.size() );
	QRegion r1( old );
	QRegion r2( cur );
	QRegion r3 = r1.subtract( r2 );
	QRegion r4 = r2.subtract( r1 );

	for ( i = 0; i < (int)r3.rects().count(); ++i ) {
	    QRect r( r3.rects()[ i ] );
	    r = QRect( viewportToContents2( r.topLeft() ), r.size() );
	    repaintContents( r, FALSE );
	}
	for ( i = 0; i < (int)r4.rects().count(); ++i ) {
	    QRect r( r4.rects()[ i ] );
	    r = QRect( viewportToContents2( r.topLeft() ), r.size() );
	    repaintContents( r, FALSE );
	}
    }

    topHeader->setUpdatesEnabled( FALSE );
    if ( updateHorizontal ) {
	for ( i = 0; i <= numCols(); ++i ) {
	    if ( !isColumnSelected( i ) )
		topHeader->setSectionState( i, QTableHeader::Normal );
	    else if ( isColumnSelected( i, TRUE ) )
		topHeader->setSectionState( i, QTableHeader::Selected );
	    else
		topHeader->setSectionState( i, QTableHeader::Bold );
	}
    }
    topHeader->setUpdatesEnabled( TRUE );
    topHeader->repaint( FALSE );

    leftHeader->setUpdatesEnabled( FALSE );
    if ( updateVertical ) {
	for ( i = 0; i <= numRows(); ++i ) {
	    if ( !isRowSelected( i ) )
		leftHeader->setSectionState( i, QTableHeader::Normal );
	    else if ( isRowSelected( i, TRUE ) )
		leftHeader->setSectionState( i, QTableHeader::Selected );
	    else
		leftHeader->setSectionState( i, QTableHeader::Bold );
	}
    }
    leftHeader->setUpdatesEnabled( TRUE );
    leftHeader->repaint( FALSE );
}

/*!  Clears all selections.
*/

void QTable::clearSelection( bool repaint )
{
    bool needRepaint = !selections.isEmpty();

    QRect r;
    for ( QTableSelection *s = selections.first(); s; s = selections.next() ) {
	bool b;
	r = r.unite( rangeGeometry( s->topRow(),
				    s->leftCol(),
				    s->bottomRow(),
				    s->rightCol(), b ) );
    }

    currentSel = 0;
    selections.clear();
    if ( needRepaint && repaint )
	repaintContents( r, FALSE );

    int i;
    bool b = topHeader->isUpdatesEnabled();
    topHeader->setUpdatesEnabled( FALSE );
    for ( i = 0; i <= numCols(); ++i ) {
	if ( !isColumnSelected( i ) && i != curCol )
	    topHeader->setSectionState( i, QTableHeader::Normal );
	else if ( isColumnSelected( i, TRUE ) )
	    topHeader->setSectionState( i, QTableHeader::Selected );
	else
	    topHeader->setSectionState( i, QTableHeader::Bold );
    }
    topHeader->setUpdatesEnabled( b );
    topHeader->repaint( FALSE );

    b = leftHeader->isUpdatesEnabled();
    leftHeader->setUpdatesEnabled( FALSE );
    for ( i = 0; i <= numRows(); ++i ) {
	if ( !isRowSelected( i ) && i != curRow )
	    leftHeader->setSectionState( i, QTableHeader::Normal );
	else if ( isRowSelected( i, TRUE ) )
	    leftHeader->setSectionState( i, QTableHeader::Selected );
	else
	    leftHeader->setSectionState( i, QTableHeader::Bold );
    }
    leftHeader->setUpdatesEnabled( b );
    leftHeader->repaint( FALSE );
    emit selectionChanged();
}

/*!  \internal
*/

QRect QTable::rangeGeometry( int topRow, int leftCol,
			     int bottomRow, int rightCol, bool &optimize )
{
    topRow = QMAX( topRow, rowAt( contentsY() ) );
    leftCol = QMAX( leftCol, columnAt( contentsX() ) );
    int ra = rowAt( contentsY() + visibleHeight() );
    if ( ra != -1 )
	bottomRow = QMIN( bottomRow, ra );
    int ca = columnAt( contentsX() + visibleWidth() );
    if ( ca != -1 )
	rightCol = QMIN( rightCol, ca );
    optimize = TRUE;
    QRect rect;
    for ( int r = topRow; r <= bottomRow; ++r ) {
	for ( int c = leftCol; c <= rightCol; ++c ) {
	    rect = rect.unite( cellGeometry( r, c ) );
	    QTableItem *i = item( r, c );
	    if ( i && ( i->rowSpan() > 1 || i->colSpan() > 1 ) )
		optimize = FALSE;
	}
    }
    return rect;
}

/*!  This is called to activate the next cell if in-place editing was
  finished by pressing the Return key.

  If you want a different behaviour then going from top to bottom,
  reimplement this function.
*/

void QTable::activateNextCell()
{
    if ( !currentSel || !currentSel->isActive() ) {
	if ( curRow < numRows() - 1 )
	    setCurrentCell( curRow + 1, curCol );
	else if ( curCol < numCols() - 1 )
	    setCurrentCell( 0, curCol + 1 );
	else
	    setCurrentCell( 0, 0 );
    } else {
	if ( curRow < currentSel->bottomRow() )
	    setCurrentCell( curRow + 1, curCol );
	else if ( curCol < currentSel->rightCol() )
	    setCurrentCell( currentSel->topRow(), curCol + 1 );
	else
	    setCurrentCell( currentSel->topRow(), currentSel->leftCol() );
    }

}

/*!  \internal
*/

void QTable::fixRow( int &row, int y )
{
    if ( row == -1 ) {
	if ( y < 0 )
	    row = 0;
	else
	    row = numRows() - 1;
    }
}

/*!  \internal
*/

void QTable::fixCol( int &col, int x )
{
    if ( col == -1 ) {
	if ( x < 0 )
	    col = 0;
	else
	    col = numCols() - 1;
    }
}

struct SortableTableItem
{
    QTableItem *item;
};

#if defined(Q_C_CALLBACKS)
extern "C" {
#endif

static int cmpTableItems( const void *n1, const void *n2 )
{
    if ( !n1 || !n2 )
	return 0;

    SortableTableItem *i1 = (SortableTableItem *)n1;
    SortableTableItem *i2 = (SortableTableItem *)n2;

    return i1->item->key().compare( i2->item->key() );
}

#if defined(Q_C_CALLBACKS)
}
#endif

/*!  Sorts the column \a col in ascending order if \a ascending is
  TRUE, else in descending order. If \a wholeRows is TRUE, for
  changing data of the cells swapRows() is called, else swapCells() is
  called.

  \sa swapRows()
*/

void QTable::sortColumn( int col, bool ascending, bool wholeRows )
{
    int filledRows = 0, i;
    for ( i = 0; i < numRows(); ++i ) {
	QTableItem *itm = item( i, col );
	if ( itm )
	    filledRows++;
    }

    if ( !filledRows )
	return;

    SortableTableItem *items = new SortableTableItem[ filledRows ];
    int j = 0;
    for ( i = 0; i < numRows(); ++i ) {
	QTableItem *itm = item( i, col );
	if ( !itm )
	    continue;
	items[ j++ ].item = itm;
    }

    qsort( items, filledRows, sizeof( SortableTableItem ), cmpTableItems );

    setUpdatesEnabled( FALSE );
    viewport()->setUpdatesEnabled( FALSE );
    for ( i = 0; i < numRows(); ++i ) {
	if ( i < filledRows ) {
	    if ( ascending ) {
		if ( items[ i ].item->row() == i )
		    continue;
		if ( wholeRows )
		    swapRows( items[ i ].item->row(), i );
		else
		    swapCells( items[ i ].item->row(), col, i, col );
	    } else {
		if ( items[ i ].item->row() == filledRows - i - 1 )
		    continue;
		if ( wholeRows )
		    swapRows( items[ i ].item->row(), filledRows - i - 1 );
		else
		    swapCells( items[ i ].item->row(), col,
			       filledRows - i - 1, col );
	    }
	}
    }
    setUpdatesEnabled( TRUE );
    viewport()->setUpdatesEnabled( TRUE );

    if ( !wholeRows )
	repaintContents( columnPos( col ), contentsY(),
			 columnWidth( col ), visibleHeight(), FALSE );
    else
	repaintContents( contentsX(), contentsY(),
			 visibleWidth(), visibleHeight(), FALSE );

    delete [] items;
}

/*!  Hides the row \a row.

  \sa showRow()
*/

void QTable::hideRow( int row )
{
    leftHeader->resizeSection( row, 0 );
    leftHeader->setResizeEnabled( FALSE, row );
    rowHeightChanged( row );
}

/*!  Hides the column \a col.

  \sa showCol()
*/

void QTable::hideColumn( int col )
{
    topHeader->resizeSection( col, 0 );
    topHeader->setResizeEnabled( FALSE, col );
    columnWidthChanged( col );
}


bool QTable::isRowHidden( int row ) const
{
    return leftHeader->sectionSize( row ) == 0 &&
	   !leftHeader->isResizeEnabled( row );
}

bool QTable::isColumnHidden( int col ) const
{
    return topHeader->sectionSize( col ) == 0 &&
	   !topHeader->isResizeEnabled( col );
}

/*!  Shows the row \a row.

  \sa hideRow()
*/

void QTable::showRow( int row )
{
    leftHeader->resizeSection( row, 100 );
    leftHeader->setResizeEnabled( TRUE, row );
    rowHeightChanged( row );
}

/*!  Shows the column \a col.

  \sa hideColumn()
*/

void QTable::showColumn( int col )
{
    topHeader->resizeSection( col, 100 );
    topHeader->setResizeEnabled( TRUE, col );
    columnWidthChanged( col );
}

/*! Resizes the column to \a w pixel wide.
*/

void QTable::setColumnWidth( int col, int w )
{
    topHeader->resizeSection( col, w );
    columnWidthChanged( col );
}

/*! Resizes the row to be \a h pixel height.
*/

void QTable::setRowHeight( int row, int h )
{
    leftHeader->resizeSection( row, h );
    rowHeightChanged( row );
}

/*!  Resizes the column \a col to be exactly wide enough so that the
  whole contents is visible.
*/

void QTable::adjustColumn( int col )
{
    int w = topHeader->fontMetrics().width( topHeader->label( col ) ) + 10;
    if ( topHeader->iconSet( col ) )
	w += topHeader->iconSet( col )->pixmap().width();
    w = QMAX( w, 20 );
    for ( int i = 0; i < numRows(); ++i ) {
	QTableItem *itm = item( i, col );
	if ( !itm )
	    continue;
	w = QMAX( w, itm->sizeHint().width() );
    }
    setColumnWidth( col, w );
}

/*!  Resizes the row \a row to be exactly high enough so that the
  whole contents is visible.
*/

void QTable::adjustRow( int row )
{
    int h = 20;
    h = QMAX( h, leftHeader->fontMetrics().height() );
    if ( leftHeader->iconSet( row ) )
	h += leftHeader->iconSet( row )->pixmap().height();
    for ( int i = 0; i < numCols(); ++i ) {
	QTableItem *itm = item( row, i );
	if ( !itm )
	    continue;
	h = QMAX( h, itm->sizeHint().height() );
    }
    setRowHeight( row, h );
}

/*!  Sets the column \a col to stretchable if \a stretch is TRUE, else
  to non-stretchable. So, if the table widgets gets wider than its
  contents, stretchable columns are stretched so that the contents
  fits exactly into to widget.
*/

void QTable::setColumnStretchable( int col, bool stretch )
{
    topHeader->setSectionStretchable( col, stretch );
}

/*!  Sets the row \a row to stretchable if \a stretch is TRUE, else to
  non-stretchable. So, if the table widgets gets higher than its
  contents, stretchable rows are stretched so that the contents fits
  exactly into to widget.
*/

void QTable::setRowStretchable( int row, bool stretch )
{
    leftHeader->setSectionStretchable( row, stretch );
}

/*! Returns wheather the column \a col is stretchable or not.

  \sa setColumnStretchable()
*/

bool QTable::isColumnStretchable( int col ) const
{
    return topHeader->isSectionStretchable( col );
}

/*! Returns wheather the row \a row is stretchable or not.

  \sa setRowStretchable()
*/

bool QTable::isRowStretchable( int row ) const
{
    return leftHeader->isSectionStretchable( row );
}

/*!  Takes the item \a i out of the table. This functions doesn't
  delete it.
*/

void QTable::takeItem( QTableItem *i )
{
    QRect rect = cellGeometry( i->row(), i->col() );
    if ( !i )
	return;
    contents.setAutoDelete( FALSE );
    for ( int r = 0; r < i->rowSpan(); ++r ) {
	for ( int c = 0; c < i->colSpan(); ++c )
	    clearCell( i->row() + r, i->col() + c );
    }
    contents.setAutoDelete( TRUE );
    repaintContents( rect, FALSE );
    int orow = i->row();
    int ocol = i->col();
    i->setRow( -1 );
    i->setCol( -1 );
    i->updateEditor( orow, ocol );
}

/*! Sets the widget \a e to the cell \a row, \a col and does all the
 placement and further stuff and takes care about correctly placing
 are resizing it when the cell geometry changes.

 By default widgets are inserted into a vector with numRows() *
 numCols() elements. In very big tables you probably want to store the
 widgets in a datastructure which needs less memory (like a
 hash-table). To make this possible this functions calls
 insertWidget() to add the widget to the internal datastructure. So if
 you want to use your own datastructure, reimplement insertWidget(),
 cellWidget() and clearCellWidget().
*/

void QTable::setCellWidget( int row, int col, QWidget *e )
{
    if ( !e )
	return;

    QWidget *w = cellWidget( row, col );
    if ( w && row == editRow && col == editCol )
 	endEdit( editRow, editCol, FALSE, edMode != Editing );

    e->installEventFilter( this );
    clearCellWidget( row, col );
    if ( e->parent() != viewport() )
	e->reparent( viewport(), QPoint( 0,0 ) );
    insertWidget( row, col, e );
    QRect cr = cellGeometry( row, col );
    e->resize( cr.size() - QSize( 1, 1 ) );
    moveChild( e, cr.x(), cr.y() );
    e->show();
    viewport()->setFocus();
}

/*!  Inserts the widget \a w into the internal datastructure. See the
  documentation of setCellWidget() for further details.
*/

void QTable::insertWidget( int row, int col, QWidget *w )
{
    if ( row < 0 || col < 0 || row > numRows() - 1 || col > numCols() - 1 )
	return;

    if ( (int)widgets.size() != numRows() * numCols() )
	widgets.resize( numRows() * numCols() );

    widgets.insert( indexOf( row, col ), w );
}

/*!
  Returns the widget which has been set to the cell \a row, \a col
  of 0 if there is no widget.
*/

QWidget *QTable::cellWidget( int row, int col ) const
{
    if ( row < 0 || col < 0 || row > numRows() - 1 || col > numCols() - 1 )
	return 0;

    if ( (int)widgets.size() != numRows() * numCols() )
	( (QTable*)this )->widgets.resize( numRows() * numCols() );

    return widgets[ indexOf( row, col ) ];
}

/*!
  Removes the widget (if there is any) which is set for the cell \a row, \a col.
*/

void QTable::clearCellWidget( int row, int col )
{
    if ( row < 0 || col < 0 || row > numRows() - 1 || col > numCols() - 1 )
	return;

    if ( (int)widgets.size() != numRows() * numCols() )
	widgets.resize( numRows() * numCols() );

    QWidget *w = cellWidget( row, col );
    if ( w )
	w->removeEventFilter( this );
    widgets.remove( indexOf( row, col ) );
}









QTableHeader::QTableHeader( int i, QTable *t,
			    QWidget *parent, const char *name )
    : QHeader( i, parent, name ),
      table( t ), caching( FALSE ), numStretches( 0 )
{
    d = 0;
    states.resize( i );
    stretchable.resize( i );
    states.fill( Normal, -1 );
    stretchable.fill( FALSE, -1 );
    mousePressed = FALSE;
    autoScrollTimer = new QTimer( this );
    connect( autoScrollTimer, SIGNAL( timeout() ),
	     this, SLOT( doAutoScroll() ) );
    line1 = new QWidget( table->viewport() );
    line1->hide();
    line1->setBackgroundMode( PaletteText );
    table->addChild( line1 );
    line2 = new QWidget( table->viewport() );
    line2->hide();
    line2->setBackgroundMode( PaletteText );
    table->addChild( line2 );

    connect( this, SIGNAL( sizeChange( int, int, int ) ),
	     this, SLOT( sectionWidthChanged( int, int, int ) ) );
    connect( this, SIGNAL( indexChange( int, int, int ) ),
	     this, SLOT( indexChanged( int, int, int ) ) );

    stretchTimer = new QTimer( this );
    widgetStretchTimer = new QTimer( this );
    connect( stretchTimer, SIGNAL( timeout() ),
	     this, SLOT( updateStretches() ) );
    connect( widgetStretchTimer, SIGNAL( timeout() ),
	     this, SLOT( updateWidgetStretches() ) );
}

void QTableHeader::addLabel( const QString &s , int size )
{
    states.resize( states.count() + 1 );
    states[ (int)states.count() - 1 ] = Normal;
    stretchable.resize( stretchable.count() + 1 );
    stretchable[ (int)stretchable.count() - 1 ] = FALSE;
    QHeader::addLabel( s , size );
}

void QTableHeader::setSectionState( int s, SectionState astate )
{
    if ( s < 0 || s >= (int)states.count() )
	return;
    if ( states[ s ] == astate )
	return;

    states[ s ] = astate;
    if ( isUpdatesEnabled() ) {
	if ( orientation() == Horizontal )
	    repaint( FALSE );
	else
	    repaint( FALSE );
    }
}

QTableHeader::SectionState QTableHeader::sectionState( int s ) const
{
    return (QTableHeader::SectionState)states[ s ];
}

void QTableHeader::paintEvent( QPaintEvent *e )
{
    QPainter p( this );
    p.setPen( colorGroup().buttonText() );
    int pos = orientation() == Horizontal
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
	p.save();
	if ( sectionState( i ) == Bold || sectionState( i ) == Selected ) {
	    QFont f( font() );
	    f.setBold( TRUE );
	    p.setFont( f );
	}
	paintSection( &p, i, r );
	p.restore();
	if ( orientation() == Horizontal && r. right() >= e->rect().right() ||
	     orientation() == Vertical && r. bottom() >= e->rect().bottom() )
	    return;
    }
}

void QTableHeader::paintSection( QPainter *p, int index, QRect fr )
{
    if ( sectionSize( index ) == 0 )
	return;
    int section = mapToSection( index );
    if ( section < 0 )
	return;

    if ( sectionState( index ) != Selected ) {
	QHeader::paintSection( p, index, fr );
    } else {
	style().drawBevelButton( p, fr.x(), fr.y(), fr.width(), fr.height(),
				 colorGroup(), TRUE );
	paintSectionLabel( p, index, fr );
    }
}

static int real_pos( const QPoint &p, Qt::Orientation o )
{
    if ( o == Qt::Horizontal )
	return p.x();
    return p.y();
}

void QTableHeader::mousePressEvent( QMouseEvent *e )
{
    if ( e->button() != LeftButton )
	return;
    QHeader::mousePressEvent( e );
    mousePressed = TRUE;
    pressPos = real_pos( e->pos(), orientation() );
    startPos = -1;
    setCaching( TRUE );
    resizedSection = -1;
#ifdef QT_NO_CURSOR
    isResizing = FALSE;
#else
    isResizing = cursor().shape() != ArrowCursor;
    if ( !isResizing && sectionAt( pressPos ) != -1 )
	doSelection( e );
#endif
}

void QTableHeader::mouseMoveEvent( QMouseEvent *e )
{
    if ( !mousePressed
#ifndef QT_NO_CURSOR
	|| cursor().shape() != ArrowCursor
#endif
	 || ( ( e->state() & ControlButton ) == ControlButton &&
	   ( orientation() == Horizontal
	     ? table->columnMovingEnabled() : table->rowMovingEnabled() ) ) ) {
	QHeader::mouseMoveEvent( e );
	return;
    }

    if ( !doSelection( e ) )
	QHeader::mouseMoveEvent( e );
}

bool QTableHeader::doSelection( QMouseEvent *e )
{
    int p = real_pos( e->pos(), orientation() ) + offset();
    if ( startPos == -1 ) {
	startPos = p;
	int secAt = sectionAt( p );
	if ( ( e->state() & ControlButton ) != ControlButton
	     || table->selectionMode() == QTable::Single ) {
	    bool upd = TRUE;
	    if ( ( orientation() == Horizontal &&
		   table->isColumnSelected( secAt, TRUE ) ||
		   orientation() == Vertical &&
		   table->isRowSelected( secAt, TRUE ) ) &&
		 table->numSelections() == 1 )
		upd = FALSE;
	    table->viewport()->setUpdatesEnabled( upd );
	    table->clearSelection();
	    table->viewport()->setUpdatesEnabled( TRUE );
	}
	saveStates();
	if ( table->selectionMode() != QTable::NoSelection ) {
	    table->currentSel = new QTableSelection();
	    table->selections.append( table->currentSel );
	    if ( orientation() == Vertical ) {
		table->currentSel->init( secAt, 0 );
	    } else {
		table->currentSel->init( 0, secAt );
	    }
	}
    }
    if ( sectionAt( p ) != -1 )
	endPos = p;
    if ( startPos != -1 ) {
	updateSelections();
	p -= offset();
	if ( orientation() == Horizontal && ( p < 0 || p > width() ) ) {
	    doAutoScroll();
	    autoScrollTimer->start( 100, TRUE );
	} else if ( orientation() == Vertical && ( p < 0 || p > height() ) ) {
	    doAutoScroll();
	    autoScrollTimer->start( 100, TRUE );
	}
	return TRUE;
    }
    return FALSE;
}

void QTableHeader::mouseReleaseEvent( QMouseEvent *e )
{
    if ( e->button() != LeftButton )
	return;
    autoScrollTimer->stop();
    mousePressed = FALSE;
    bool hasCached = resizedSection != -1;
    setCaching( FALSE );
    QHeader::mouseReleaseEvent( e );
    line1->hide();
    line2->hide();
    if ( hasCached ) {
	emit sectionSizeChanged( resizedSection );
	updateStretches();
    }
}

void QTableHeader::mouseDoubleClickEvent( QMouseEvent *e )
{
    if ( isResizing ) {
	int p = real_pos( e->pos(), orientation() ) + offset();
	int section = sectionAt( p );
	if ( section == -1 )
	    return;
	section--;
	if ( p >= sectionPos( count() - 1 ) + sectionSize( count() - 1 ) )
	    ++section;
	if ( orientation() == Horizontal )
	    table->adjustColumn( section );
	else
	    table->adjustRow( section );
    }
}

void QTableHeader::resizeEvent( QResizeEvent *e )
{
    stretchTimer->stop();
    widgetStretchTimer->stop();
    QHeader::resizeEvent( e );
    if ( numStretches == 0 )
	return;
    stretchTimer->start( 0, TRUE );
}

void QTableHeader::updateStretches()
{
    if ( numStretches == 0 )
	return;
    if ( orientation() == Horizontal ) {
	if ( sectionPos( count() - 1 ) + sectionSize( count() - 1 ) ==
	     width() )
	    return;
	int i;
	int pw = width() -
		 ( sectionPos( count() - 1 ) + sectionSize( count() - 1 ) ) -
		 1;
	bool block = signalsBlocked();
	blockSignals( TRUE );
	for ( i = 0; i < (int)stretchable.count(); ++i ) {
	    if ( !stretchable[ i ] )
		continue;
	    pw += sectionSize( i );
	}
	pw /= numStretches;
	for ( i = 0; i < (int)stretchable.count(); ++i ) {
	    if ( !stretchable[ i ] )
		continue;
	    if ( i == (int)stretchable.count() - 1 &&
		 sectionPos( i ) + pw < width() )
		pw = width() - sectionPos( i );
	    resizeSection( i, QMAX( 20, pw ) );
	}
	blockSignals( block );
	table->viewport()->repaint( FALSE );
	widgetStretchTimer->start( 100, TRUE );
    } else {
	if ( sectionPos( count() - 1 ) + sectionSize( count() - 1 ) == height() )
	    return;
	int i;
	int ph = height() - ( sectionPos( count() - 1 ) + sectionSize( count() - 1 ) ) - 1;
	bool block = signalsBlocked();
	blockSignals( TRUE );
	for ( i = 0; i < (int)stretchable.count(); ++i ) {
	    if ( !stretchable[ i ] )
		continue;
	    ph += sectionSize( i );
	}
	ph /= numStretches;
	for ( i = 0; i < (int)stretchable.count(); ++i ) {
	    if ( !stretchable[ i ] )
		continue;
	    if ( i == (int)stretchable.count() - 1 && sectionPos( i ) + ph < height() )
		ph = height() - sectionPos( i );
	    resizeSection( i, QMAX( 20, ph ) );
	}
	blockSignals( block );
	table->viewport()->repaint( FALSE );
	widgetStretchTimer->start( 100, TRUE );
    }
}

void QTableHeader::updateWidgetStretches()
{
    QSize s = table->tableSize();
    table->resizeContents( s.width(), s.height() );
    for ( int i = 0; i < table->numCols(); ++i )
	table->updateColWidgets( i );
}

void QTableHeader::updateSelections()
{
    if ( table->selectionMode() == QTable::NoSelection )
	return;
    int a = sectionAt( startPos );
    int b = sectionAt( endPos );
    int start = QMIN( a, b );
    int end = QMAX( a, b );
    setUpdatesEnabled( FALSE );
    for ( int i = 0; i < count(); ++i ) {
	if ( i < start || i > end )
	    setSectionState( i, (QTableHeader::SectionState)oldStates[ i ] );
	else
	    setSectionState( i, Selected );
    }
    setUpdatesEnabled( TRUE );
    repaint( FALSE );

    QTableSelection oldSelection = *table->currentSel;
    if ( orientation() == Vertical )
	table->currentSel->expandTo( b, table->horizontalHeader()->count() - 1 );
    else
	table->currentSel->expandTo( table->verticalHeader()->count() - 1, b );
    table->repaintSelections( &oldSelection, table->currentSel,
			      orientation() == Horizontal,
			      orientation() == Vertical );
    emit table->selectionChanged();
}

void QTableHeader::saveStates()
{
    oldStates.resize( count() );
    for ( int i = 0; i < count(); ++i )
	oldStates[ i ] = sectionState( i );
}

void QTableHeader::doAutoScroll()
{
    QPoint pos = mapFromGlobal( QCursor::pos() );
    int p = real_pos( pos, orientation() ) + offset();
    if ( sectionAt( p ) != -1 )
	endPos = p;
    if ( orientation() == Horizontal )
	table->ensureVisible( endPos, table->contentsY() );
    else
	table->ensureVisible( table->contentsX(), endPos );
    updateSelections();
    autoScrollTimer->start( 100, TRUE );
}

void QTableHeader::sectionWidthChanged( int col, int, int )
{
    resizedSection = col;
    if ( orientation() == Horizontal ) {
	table->moveChild( line1, QHeader::sectionPos( col ) - 1,
			  table->contentsY() );
	line1->resize( 1, table->visibleHeight() );
	line1->show();
	line1->raise();
	table->moveChild( line2,
			  QHeader::sectionPos( col ) + QHeader::sectionSize( col ) - 1,
			  table->contentsY() );
	line2->resize( 1, table->visibleHeight() );
	line2->show();
	line2->raise();
    } else {
	table->moveChild( line1, table->contentsX(),
			  QHeader::sectionPos( col ) - 1 );
	line1->resize( table->visibleWidth(), 1 );
	line1->show();
	line1->raise();
	table->moveChild( line2, table->contentsX(),
			  QHeader::sectionPos( col ) + QHeader::sectionSize( col ) - 1 );
	line2->resize( table->visibleWidth(), 1 );
	line2->show();
	line2->raise();
    }
}

int QTableHeader::sectionSize( int section ) const
{
    if ( count() <= 0 || section < 0 )
	return -1;
    if ( caching )
	return sectionSizes[ section ];
    return QHeader::sectionSize( section );
}

int QTableHeader::sectionPos( int section ) const
{
    if ( count() <= 0 || section < 0 )
	return -1;
    if ( caching )
	return sectionPoses[ section ];
    return QHeader::sectionPos( section );
}

int QTableHeader::sectionAt( int pos ) const
{
    if ( !caching )
	return QHeader::sectionAt( pos );
    if ( count() <= 0 || pos > sectionPoses[ count() - 1 ] + sectionSizes[ count() - 1 ] )
	return -1;
    int l = 0;
    int r = count() - 1;
    int i = ( (l+r+1) / 2 );
    while ( r - l ) {
	if ( sectionPoses[i] > pos )
	    r = i -1;
	else
	    l = i;
	i = ( (l+r+1) / 2 );
    }
    if ( sectionPoses[i] <= pos &&
	 pos <= sectionPoses[i] + sectionSizes[ mapToSection( i ) ] )
	return mapToSection( i );
    return -1;
}

void QTableHeader::setCaching( bool b )
{
    if ( caching == b )
	return;
    caching = b;
    sectionPoses.resize( count() );
    sectionSizes.resize( count() );
    if ( b ) {
	for ( int i = 0; i < count(); ++i ) {
	    sectionSizes[ i ] = QHeader::sectionSize( i );
	    sectionPoses[ i ] = QHeader::sectionPos( i );
	}
    }
}

void QTableHeader::setSectionStretchable( int s, bool b )
{
    if ( stretchable[ s ] == b )
	return;
    stretchable[ s ] = b;
    if ( b )
	numStretches++;
    else
	numStretches--;
}

bool QTableHeader::isSectionStretchable( int s ) const
{
    return stretchable[ s ];
}

void QTableHeader::swapSections( int oldIdx, int newIdx )
{
    QIconSet is;
    bool his = FALSE;
    if ( iconSet( oldIdx ) ) {
	his = TRUE;
	is = *iconSet( oldIdx );
    }
    QString l = label( oldIdx );
    if ( iconSet( newIdx ) )
	setLabel( oldIdx, *iconSet( newIdx ), label( newIdx ) );
    else
	setLabel( oldIdx, label( newIdx ) );

    if ( his )
	setLabel( newIdx, is, l );
    else
	setLabel( newIdx, l );

    int w1 = sectionSize( oldIdx );
    int w2 = sectionSize( newIdx );
    resizeSection( oldIdx, w2 );
    resizeSection( newIdx, w1 );

    if ( orientation() == Horizontal )
	table->swapColumns( oldIdx, newIdx );
    else
	table->swapRows( oldIdx, newIdx );
}

void QTableHeader::indexChanged( int sec, int oldIdx, int newIdx )
{
    newIdx = mapToIndex( sec );
    if ( oldIdx > newIdx )
	moveSection( sec, oldIdx + 1 );
    else
	moveSection( sec, oldIdx );

    if ( oldIdx < newIdx ) {
	while ( oldIdx < newIdx ) {
	    swapSections( oldIdx, oldIdx + 1 );
	    oldIdx++;
	}
    } else {
	while ( oldIdx > newIdx ) {
	    swapSections( oldIdx - 1, oldIdx );
	    oldIdx--;
	}
    }

    table->repaintContents( table->contentsX(), table->contentsY(),
			    table->visibleWidth(), table->visibleHeight() );
}
#endif // QT_NO_TABLE
