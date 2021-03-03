/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Qt Designer.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#include "formwindow.h"
#include "layout.h"
#include <widgetdatabase.h>
#include "widgetfactory.h"

#include <qlayout.h>
#include <qevent.h>
#include <qpainter.h>
#include <qpen.h>
#include <qbitmap.h>

bool operator<( const QGuardedPtr<QWidget> &p1, const QGuardedPtr<QWidget> &p2 )
{
    return p1.operator->() < p2.operator->();
}

/*!
  \class Layout layout.h
  \brief Baseclass for layouting widgets in the Designer

  Classes derived from this abstract base class are used for layouting
  operations in the Designer.

*/

/*!  \a p specifies the parent of the layoutBase \a lb. The parent
  might be changed in setup(). If the layoutBase is a
  container, the parent and the layoutBase are the same. Also they
  always have to be a widget known to the designer (e.g. in the case
  of the tabwidget parent and layoutBase are the tabwidget and not the
  page which actually gets laid out. For actual usage the correct
  widget is found later by Layout.)
 */

Layout::Layout( const QWidgetList &wl, QWidget *p, FormWindow *fw, QWidget *lb, bool doSetup )
    : widgets( wl ), parent( p ), formWindow( fw ), isBreak( !doSetup )
{
    widgets.setAutoDelete( FALSE );
    layoutBase = lb;
    if ( !doSetup && layoutBase )
	oldGeometry = layoutBase->geometry();
}

/*!  The widget list we got in the constructor might contain too much
  widgets (like widgets with different parents, already laid out
  widgets, etc.). Here we set up the list and so the only the "best"
  widgets get laid out.
*/

void Layout::setup()
{
    startPoint = QPoint( 32767, 32767 );
    QValueList<QWidgetList> lists;
    QWidget *lastParent = 0;
    QWidgetList *lastList = 0;
    QWidget *w = 0;

    // Go through all widgets of the list we got. As we can only
    // layout widgets which have the same parent, we first do some
    // sorting which means create a list for each parent containing
    // its child here. After that we keep working on the list of
    // childs which has the most entries.
    // Widgets which are already laid out are thrown away here too
    for ( w = widgets.first(); w; w = widgets.next() ) {
	if ( w->parentWidget() && WidgetFactory::layoutType( w->parentWidget() ) != WidgetFactory::NoLayout )
	    continue;
	if ( lastParent != w->parentWidget() ) {
	    lastList = 0;
	    lastParent = w->parentWidget();
	    QValueList<QWidgetList>::Iterator it = lists.begin();
	    for ( ; it != lists.end(); ++it ) {
		if ( ( *it ).first()->parentWidget() == w->parentWidget() )
		    lastList = &( *it );
	    }
	    if ( !lastList ) {
		QWidgetList l;
		l.setAutoDelete( FALSE );
		lists.append( l );
		lastList = &lists.last();
	    }
	}
	lastList->append( w );
    }

    // So, now find the list with the most entries
    lastList = 0;
    QValueList<QWidgetList>::Iterator it = lists.begin();
    for ( ; it != lists.end(); ++it ) {
	if ( !lastList || ( *it ).count() > lastList->count() )
	    lastList = &( *it );
    }

    // If we found no list (because no widget did fit at all) or the
    // best list has only one entry and we do not layout a container,
    // we leave here.
    if ( !lastList || ( lastList->count() < 2 &&
			( !layoutBase ||
			  ( !WidgetDatabase::isContainer( WidgetDatabase::idFromClassName( WidgetFactory::classNameOf( layoutBase ) ) ) &&
			    layoutBase != formWindow->mainContainer() ) )
			) ) {
	widgets.clear();
	startPoint = QPoint( 0, 0 );
	return;
    }

    // Now we have a new and clean widget list, which makes sense
    // to layout
    widgets = *lastList;
    // Also use the only correct parent later, so store it
    parent = WidgetFactory::widgetOfContainer( widgets.first()->parentWidget() );
    // Now calculate the position where the layout-meta-widget should
    // be placed and connect to widgetDestroyed() signals of the
    // widgets to get informed if one gets deleted to be able to
    // handle that and do not crash in this case
    for ( w = widgets.first(); w; w = widgets.next() ) {
	connect( w, SIGNAL( destroyed() ),
		 this, SLOT( widgetDestroyed() ) );
	startPoint = QPoint( QMIN( startPoint.x(), w->x() ),
			     QMIN( startPoint.y(), w->y() ) );
	geometries.insert( w, QRect( w->pos(), w->size() ) );
	// Change the Z-order, as saving/loading uses the Z-order for
	// writing/creating widgets and this has to be the same as in
	// the layout. Else saving + loading will give different results
	w->raise();
    }
}

void Layout::widgetDestroyed()
{
     if ( sender() && sender()->isWidgetType() )
	widgets.removeRef( (QWidget*)sender() );
}

bool Layout::prepareLayout( bool &needMove, bool &needReparent )
{
    if ( !widgets.count() )
	return FALSE;
    for ( QWidget *w = widgets.first(); w; w = widgets.next() )
	w->raise();
    needMove = !layoutBase;
    needReparent = needMove || layoutBase->inherits( "QLayoutWidget" );
    if ( !layoutBase ) {
	layoutBase = WidgetFactory::create( WidgetDatabase::idFromClassName( "QLayoutWidget" ),
					    WidgetFactory::containerOfWidget( parent ) );
    } else {
	WidgetFactory::deleteLayout( layoutBase );
    }

    return TRUE;
}

void Layout::finishLayout( bool needMove, QLayout *layout )
{
    if ( needMove )
	layoutBase->move( startPoint );
    QRect g( QRect( layoutBase->pos(), layoutBase->size() ) );
    if ( WidgetFactory::layoutType( layoutBase->parentWidget() ) == WidgetFactory::NoLayout && !isBreak )
	layoutBase->adjustSize();
    else if ( isBreak )
	layoutBase->setGeometry( oldGeometry );
    oldGeometry = g;
    layoutBase->show();
    layout->activate();
    formWindow->insertWidget( layoutBase );
    formWindow->selectWidget( layoutBase );
}

void Layout::undoLayout()
{
    if ( !widgets.count() )
	return;
    QMap<QGuardedPtr<QWidget>, QRect>::Iterator it = geometries.begin();
    for ( ; it != geometries.end(); ++it ) {
	if ( !it.key() )
	    continue;
	it.key()->reparent( WidgetFactory::containerOfWidget( parent ), 0, ( *it ).topLeft(), it.key()->isVisibleTo( formWindow ) );
	it.key()->resize( ( *it ).size() );
    }
    formWindow->selectWidget( layoutBase, FALSE );
    WidgetFactory::deleteLayout( layoutBase );
    if ( parent != layoutBase )
	layoutBase->hide();
    else
	layoutBase->setGeometry( oldGeometry );
    if ( widgets.first() )
	formWindow->selectWidget( widgets.first() );
    else
	formWindow->selectWidget( formWindow );
}

void Layout::breakLayout()
{
    WidgetFactory::deleteLayout( layoutBase );
    bool needReparent = qstrcmp( layoutBase->className(), "QLayoutWidget" ) == 0 ||
			( !WidgetDatabase::isContainer( WidgetDatabase::idFromClassName( WidgetFactory::classNameOf( layoutBase ) ) ) &&
			  layoutBase != formWindow->mainContainer() );
    bool add = geometries.isEmpty();
    for ( QWidget *w = widgets.first(); w; w = widgets.next() ) {
	if ( needReparent )
	    w->reparent( layoutBase->parentWidget(), 0,
			 layoutBase->pos() + w->pos(), TRUE );
	if ( add )
	    geometries.insert( w, QRect( w->pos(), w->size() ) );
    }
    if ( needReparent ) {
	layoutBase->hide();
	parent = layoutBase->parentWidget();
    } else {
	parent = layoutBase;
    }
    if ( widgets.first() && widgets.first()->isVisibleTo( formWindow ) )
	formWindow->selectWidget( widgets.first() );
    else
	formWindow->selectWidget( formWindow );
}

class HorizontalLayoutList : public QWidgetList
{
public:
    HorizontalLayoutList( const QWidgetList &l )
	: QWidgetList( l ) {}

    int compareItems( QCollection::Item item1, QCollection::Item item2 ) {
	QWidget *w1 = (QWidget*)item1;
	QWidget *w2 = (QWidget*)item2;
	if ( w1->x() == w2->x() )
	    return 0;
	if ( w1->x() > w2->x() )
	    return 1;
	return -1;
    }

};

HorizontalLayout::HorizontalLayout( const QWidgetList &wl, QWidget *p, FormWindow *fw, QWidget *lb, bool doSetup )
    : Layout( wl, p, fw, lb, doSetup )
{
    if ( doSetup )
	setup();
}

void HorizontalLayout::setup()
{
    HorizontalLayoutList l( widgets );
    l.sort();
    widgets = l;
    Layout::setup();
}

void HorizontalLayout::doLayout()
{
    bool needMove, needReparent;
    if ( !prepareLayout( needMove, needReparent ) )
	return;

    QHBoxLayout *layout = (QHBoxLayout*)WidgetFactory::createLayout( layoutBase, 0, WidgetFactory::HBox );

    for ( QWidget *w = widgets.first(); w; w = widgets.next() ) {
	if ( needReparent && w->parent() != layoutBase )
	    w->reparent( layoutBase, 0, QPoint( 0, 0 ), FALSE );
	if ( qstrcmp( w->className(), "Spacer" ) == 0 )
	    layout->addWidget( w, 0, ( (Spacer*)w )->alignment() );
	else
	    layout->addWidget( w );
	w->show();
    }

    finishLayout( needMove, layout );
}




class VerticalLayoutList : public QWidgetList
{
public:
    VerticalLayoutList( const QWidgetList &l )
	: QWidgetList( l ) {}

    int compareItems( QCollection::Item item1, QCollection::Item item2 ) {
	QWidget *w1 = (QWidget*)item1;
	QWidget *w2 = (QWidget*)item2;
	if ( w1->y() == w2->y() )
	    return 0;
	if ( w1->y() > w2->y() )
	    return 1;
	return -1;
    }

};

VerticalLayout::VerticalLayout( const QWidgetList &wl, QWidget *p, FormWindow *fw, QWidget *lb, bool doSetup )
    : Layout( wl, p, fw, lb, doSetup )
{
    if ( doSetup )
	setup();
}

void VerticalLayout::setup()
{
    VerticalLayoutList l( widgets );
    l.sort();
    widgets = l;
    Layout::setup();
}

void VerticalLayout::doLayout()
{
    bool needMove, needReparent;
    if ( !prepareLayout( needMove, needReparent ) )
	return;

    QVBoxLayout *layout = (QVBoxLayout*)WidgetFactory::createLayout( layoutBase, 0, WidgetFactory::VBox );

    for ( QWidget *w = widgets.first(); w; w = widgets.next() ) {
	if ( needReparent && w->parent() != layoutBase )
	    w->reparent( layoutBase, 0, QPoint( 0, 0 ), FALSE );
 	if ( qstrcmp( w->className(), "Spacer" ) == 0 )
 	    layout->addWidget( w, 0, ( (Spacer*)w )->alignment() );
 	else
	    layout->addWidget( w );
	w->show();
    }

    finishLayout( needMove, layout );
}





class Grid
{
public:
    Grid( int rows, int cols );
    ~Grid();

    QWidget* cell( int row, int col ) const { return cells[ row * ncols + col]; }
    void setCell( int row, int col, QWidget* w ) { cells[ row*ncols + col] = w; }
    int numRows() const { return nrows; }
    int numCols() const { return ncols; }

    void simplify();
    bool locateWidget( QWidget* w, int& row, int& col, int& rowspan, int& colspan );

private:
    void merge();
    int countRow( int r, int c ) const;
    int countCol( int r, int c ) const;
    void setRow( int r, int c, QWidget* w, int count );
    void setCol( int r, int c, QWidget* w, int count );
    bool isWidgetStartCol( int c ) const;
    bool isWidgetEndCol( int c ) const;
    bool isWidgetStartRow( int r ) const;
    bool isWidgetEndRow( int r ) const;
    bool isWidgetTopLeft( int r, int c ) const;
    void extendLeft();
    void extendRight();
    void extendUp();
    void extendDown();
    QWidget** cells;
    bool* cols;
    bool* rows;
    int nrows, ncols;

};

Grid::Grid( int r, int c )
    : nrows( r ), ncols( c )
{
    cells = new QWidget*[ r * c ];
    memset( cells, 0, sizeof( cells ) * r * c );
    rows = new bool[ r ];
    cols = new bool[ c ];

}

Grid::~Grid()
{
    delete [] cells;
    delete [] cols;
    delete [] rows;
}

int Grid::countRow( int r, int c ) const
{
    QWidget* w = cell( r, c );
    int i = c + 1;
    while ( i < ncols && cell( r, i ) == w )
	i++;
    return i - c;
}

int Grid::countCol( int r, int c ) const
{
    QWidget* w = cell( r, c );
    int i = r + 1;
    while ( i < nrows && cell( i, c ) == w )
	i++;
    return i - r;
}

void Grid::setCol( int r, int c, QWidget* w, int count )
{
    for (int i = 0; i < count; i++ )
	setCell( r + i, c, w );
}

void Grid::setRow( int r, int c, QWidget* w, int count )
{
    for (int i = 0; i < count; i++ )
	setCell( r, c + i, w );
}

bool Grid::isWidgetStartCol( int c ) const
{
    int r;
    for ( r = 0; r < nrows; r++ ) {
	if ( cell( r, c ) && ( (c==0) || (cell( r, c)  != cell( r, c-1) )) ) {
	    return TRUE;
	}
    }
    return FALSE;
}

bool Grid::isWidgetEndCol( int c ) const
{
    int r;
    for ( r = 0; r < nrows; r++ ) {
	if ( cell( r, c ) && ((c == ncols-1) || (cell( r, c) != cell( r, c+1) )) )
	    return TRUE;
    }
    return FALSE;
}

bool Grid::isWidgetStartRow( int r ) const
{
    int c;
    for ( c = 0; c < ncols; c++ ) {
	if ( cell( r, c ) && ( (r==0) || (cell( r, c) != cell( r-1, c) )) )
	    return TRUE;
    }
    return FALSE;
}

bool Grid::isWidgetEndRow( int r ) const
{
    int c;
    for ( c = 0; c < ncols; c++ ) {
	if ( cell( r, c ) && ((r == nrows-1) || (cell( r, c) != cell( r+1, c) )) )
	    return TRUE;
    }
    return FALSE;
}


bool Grid::isWidgetTopLeft( int r, int c ) const
{
    QWidget* w = cell( r, c );
    if ( !w )
	return FALSE;
    return ( !r || cell( r-1, c) != w ) && (!c || cell( r, c-1) != w);
}

void Grid::extendLeft()
{
    int r,c,i;
    for ( c = 1; c < ncols; c++ ) {
	for ( r = 0; r < nrows; r++ ) {
	    QWidget* w = cell( r, c );
	    if ( !w )
		continue;
	    int cc = countCol( r, c);
	    int stretch = 0;
	    for ( i = c-1; i >= 0; i-- ) {
		if ( cell( r, i ) )
		    break;
		if ( countCol( r, i ) < cc )
		    break;
		if ( isWidgetEndCol( i ) )
		    break;
		if ( isWidgetStartCol( i ) ) {
		    stretch = c - i;
		    break;
		}
	    }
	    if ( stretch ) {
		for ( i = 0; i < stretch; i++ )
		    setCol( r, c-i-1, w, cc );
	    }
	}
    }
}


void Grid::extendRight()
{
    int r,c,i;
    for ( c = ncols - 2; c >= 0; c-- ) {
	for ( r = 0; r < nrows; r++ ) {
	    QWidget* w = cell( r, c );
	    if ( !w )
		continue;
	    int cc = countCol( r, c);
	    int stretch = 0;
	    for ( i = c+1; i < ncols; i++ ) {
		if ( cell( r, i ) )
		    break;
		if ( countCol( r, i ) < cc )
		    break;
		if ( isWidgetStartCol( i ) )
		    break;
		if ( isWidgetEndCol( i ) ) {
		    stretch = i - c;
		    break;
		}
	    }
	    if ( stretch ) {
		for ( i = 0; i < stretch; i++ )
		    setCol( r, c+i+1, w, cc );
	    }
	}
    }

}

void Grid::extendUp()
{
    int r,c,i;
    for ( r = 1; r < nrows; r++ ) {
	for ( c = 0; c < ncols; c++ ) {
	    QWidget* w = cell( r, c );
	    if ( !w )
		continue;
	    int cr = countRow( r, c);
	    int stretch = 0;
	    for ( i = r-1; i >= 0; i-- ) {
		if ( cell( i, c ) )
		    break;
		if ( countRow( i, c ) < cr )
		    break;
		if ( isWidgetEndRow( i ) )
		    break;
		if ( isWidgetStartRow( i ) ) {
		    stretch = r - i;
		    break;
		}
	    }
	    if ( stretch ) {
		for ( i = 0; i < stretch; i++ )
		    setRow( r-i-1, c, w, cr );
	    }
	}
    }
}

void Grid::extendDown()
{
    int r,c,i;
    for ( r = nrows - 2; r >= 0; r-- ) {
	for ( c = 0; c < ncols; c++ ) {
	    QWidget* w = cell( r, c );
	    if ( !w )
		continue;
	    int cr = countRow( r, c);
	    int stretch = 0;
	    for ( i = r+1; i < nrows; i++ ) {
		if ( cell( i, c ) )
		    break;
		if ( countRow( i, c ) < cr )
		    break;
		if ( isWidgetStartRow( i ) )
		    break;
		if ( isWidgetEndRow( i ) ) {
		    stretch = i - r;
		    break;
		}
	    }
	    if ( stretch ) {
		for ( i = 0; i < stretch; i++ )
		    setRow( r+i+1, c, w, cr );
	    }
	}
    }

}

void Grid::simplify()
{
    extendLeft();
    extendRight();
    extendUp();
    extendDown();
    merge();
}


void Grid::merge()
{
    int r,c;
    for ( c = 0; c < ncols; c++ )
	cols[c] = FALSE;
	
    for ( r = 0; r < nrows; r++ )
	rows[r] = FALSE;

    for ( c = 0; c < ncols; c++ ) {
	for ( r = 0; r < nrows; r++ ) {
	    if ( isWidgetTopLeft( r, c ) ) {
		rows[r] = TRUE;
		cols[c] = TRUE;
	    }
	}
    }
}

bool Grid::locateWidget( QWidget* w, int& row, int& col, int& rowspan, int & colspan )
{
    int r,c, r2, c2;
    for ( c = 0; c < ncols; c++ ) {
	for ( r = 0; r < nrows; r++ ) {
	    if ( cell( r, c ) == w  ) {
		row = 0;
		for ( r2 = 1; r2 <= r; r2++ ) {
		    if ( rows[ r2-1 ] )
			row++;
		}
		col = 0;
		for ( c2 = 1; c2 <= c; c2++ ) {
		    if ( cols[ c2-1 ] )
			col++;
		}
		rowspan = 0;
		for ( r2 = r ; r2 < nrows && cell( r2, c) == w; r2++ ) {
		    if ( rows[ r2 ] )
			rowspan++;
		}
		colspan = 0;
		for ( c2 = c; c2 < ncols && cell( r, c2) == w; c2++ ) {
		    if ( cols[ c2] )
			colspan++;
		}
		return TRUE;
	    }
	}
    }
    return FALSE;
}




GridLayout::GridLayout( const QWidgetList &wl, QWidget *p, FormWindow *fw, QWidget *lb, const QSize &res, bool doSetup )
    : Layout( wl, p, fw, lb, doSetup ), resolution( res )
{
    grid = 0;
    if ( doSetup )
	setup();
}

GridLayout::~GridLayout()
{
    delete grid;
}

void GridLayout::doLayout()
{
    bool needMove, needReparent;
    if ( !prepareLayout( needMove, needReparent ) )
	return;

    QDesignerGridLayout *layout = (QDesignerGridLayout*)WidgetFactory::createLayout( layoutBase, 0, WidgetFactory::Grid );

    if ( !grid )
	buildGrid();

    QWidget* w;
    int r, c, rs, cs;
    for ( w = widgets.first(); w; w = widgets.next() ) {
	if ( grid->locateWidget( w, r, c, rs, cs) ) {
	    if ( needReparent && w->parent() != layoutBase )
		w->reparent( layoutBase, 0, QPoint( 0, 0 ), FALSE );
	    if ( rs * cs == 1 ) {
		layout->addWidget( w, r, c, w->inherits( "Spacer" ) ? ( (Spacer*)w )->alignment() : 0 );
	    } else {
		layout->addMultiCellWidget( w, r, r+rs-1, c, c+cs-1, w->inherits( "Spacer" ) ? ( (Spacer*)w )->alignment() : 0 );
	    }
	    w->show();
	} else {
	    qWarning("ooops, widget '%s' does not fit in layout", w->name() );
	}
    }
    finishLayout( needMove, layout );
}

void GridLayout::setup()
{
    Layout::setup();
    buildGrid();
}

void GridLayout::buildGrid()
{
    QWidget* w;
    QRect br;
    for ( w = widgets.first(); w; w = widgets.next() )
	br = br.unite( w->geometry() );

    delete grid;
    grid = new Grid( br.height() / resolution.height() + 1,
		     br.width() / resolution.width() + 1 );

    int r,c;
    for ( r = 0; r < grid->numRows(); r++ ) {
	for ( c = 0; c < grid->numCols(); c++ ) {
	    QPoint p( br.left() + c * resolution.width(),
		      br.top() + r* resolution.height() );
	    QRect cr( p, resolution );
	    for ( w = widgets.first(); w; w = widgets.next() ) {
 		    // check that the overlap is significant
 		    QRect intersect = cr.intersect( w->geometry() );
 		    if ( intersect.size().width() > resolution.width()/2 &&
 			 intersect.size().height() > resolution.height()/2 ) {
 			grid->setCell( r, c, w );
 		    }
	    }
	}
    }
    grid->simplify();
}







Spacer::Spacer( QWidget *parent, const char *name )
    : QWidget( parent, name, WMouseNoMask ), orient( Vertical )
{
    setSizeType( Expanding );
    setAutoMask( TRUE );
    ar = TRUE;
}

void Spacer::paintEvent( QPaintEvent * )
{
    QPainter p( this );
    p.setPen( Qt::blue );

    if ( orient == Horizontal ) {
	const int dist = 3;
	const int amplitude = QMIN( 3, height() / 3 );
	const int base = height() / 2;
	int i = 0;
	p.setPen( white );
	for ( i = 0; i < width() / 3 +2; ++i )
	    p.drawLine( i * dist, base - amplitude, i * dist + dist / 2, base + amplitude );
	p.setPen( blue );
	for ( i = 0; i < width() / 3 +2; ++i )
	    p.drawLine( i * dist + dist / 2, base + amplitude, i * dist + dist, base - amplitude );
	p.drawLine( 0, 0, 0, height() );
	p.drawLine( width() - 1, 0, width() - 1, height());
    } else {
	const int dist = 3;
	const int amplitude = QMIN( 3, width() / 3 );
	const int base = width() / 2;
	int i = 0;
	p.setPen( white );
	for ( i = 0; i < height() / 3 +2; ++i )
	    p.drawLine( base - amplitude, i * dist, base + amplitude,i * dist + dist / 2 );
	p.setPen( blue );
	for ( i = 0; i < height() / 3 +2; ++i )
	    p.drawLine( base + amplitude, i * dist + dist / 2, base - amplitude, i * dist + dist );
	p.drawLine( 0, 0, width(), 0 );
	p.drawLine( 0, height() - 1, width(), height() - 1 );
    }
}

void Spacer::updateMask()
{
    QRegion r( rect() );
    if ( orient == Horizontal ) {
	const int amplitude = QMIN( 3, height() / 3 );
	const int base = height() / 2;
	r = r.subtract( QRect(1, 0, width() - 2, base - amplitude ) );
	r = r.subtract( QRect(1, base + amplitude, width() - 2, height() - base - amplitude ) );
    } else {
	const int amplitude = QMIN( 3, width() / 3 );
	const int base = width() / 2;
	r = r.subtract( QRect(0, 1, base - amplitude, height() - 2 ) );
	r = r.subtract( QRect( base + amplitude, 1, width() - base - amplitude, height() - 2 ) );
    }
    setMask( r );
}

void Spacer::setSizeType( SizeType t )
{
    QSizePolicy sizeP;
    if ( orient == Vertical )
	sizeP = QSizePolicy( QSizePolicy::Minimum, (QSizePolicy::SizeType)t );
    else
	sizeP = QSizePolicy( (QSizePolicy::SizeType)t, QSizePolicy::Minimum );
    setSizePolicy( sizeP );
}


Spacer::SizeType Spacer::sizeType() const
{
    if ( orient == Vertical )
	return (SizeType)sizePolicy().verData();
    return (SizeType)sizePolicy().horData();
}

int Spacer::alignment() const
{
    if ( orient == Vertical )
	return AlignHCenter;
    return AlignVCenter;
}

QSize Spacer::minimumSize() const
{
    return QSize( 20, 20 );
}

QSize Spacer::sizeHint() const
{
    if ( parentWidget() && WidgetFactory::layoutType( parentWidget() ) != WidgetFactory::NoLayout &&
	 ( sizeType() == Fixed || sizeType() == Maximum ) )
	return size();
    return QSize( 20, 20 );
}

Qt::Orientation Spacer::orientation() const
{
    return orient;
}

void Spacer::setOrientation( Qt::Orientation o )
{
    if ( orient == o )
 	return;
	
    SizeType st = sizeType();
    orient = o;
    setSizeType( st );
    if ( ar )
	resize( QSize( size().height(), size().width() ) );
    updateMask();
    update();
    updateGeometry();
}


void QDesignerGridLayout::addWidget( QWidget *w, int row, int col, int align_ )
{
    items.insert( w, Item(row, col, 1, 1) );
    QGridLayout::addWidget( w, row, col, align_ );
}

void QDesignerGridLayout::addMultiCellWidget( QWidget *w, int fromRow, int toRow,
					      int fromCol, int toCol, int align_ )
{
    items.insert( w, Item(fromRow, fromCol, toRow - fromRow + 1, toCol - fromCol +1) );
    QGridLayout::addMultiCellWidget( w, fromRow, toRow, fromCol, toCol, align_ );
}
