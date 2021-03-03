/**********************************************************************
** $Id: qt/src/widgets/qtableview.h   2.3.2   edited 2001-01-26 $
**
** Definition of QTableView class
**
** Created : 941115
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

#ifndef QTABLEVIEW_H
#define QTABLEVIEW_H

#ifndef QT_H
#include "qframe.h"
#endif // QT_H

#ifndef QT_NO_TABLEVIEW

class QScrollBar;
class QCornerSquare;


class Q_EXPORT QTableView : public QFrame
{
    Q_OBJECT
public:
    virtual void setBackgroundColor( const QColor & );
    virtual void setPalette( const QPalette & );
    void	show();

    void	repaint( bool erase=TRUE );
    void	repaint( int x, int y, int w, int h, bool erase=TRUE );
    void	repaint( const QRect &, bool erase=TRUE );

protected:
    QTableView( QWidget *parent=0, const char *name=0, WFlags f=0 );
   ~QTableView();

    int		numRows()	const;
    virtual void setNumRows( int );
    int		numCols()	const;
    virtual void setNumCols( int );

    int		topCell()	const;
    virtual void setTopCell( int row );
    int		leftCell()	const;
    virtual void setLeftCell( int col );
    virtual void setTopLeftCell( int row, int col );

    int		xOffset()	const;
    virtual void setXOffset( int );
    int		yOffset()	const;
    virtual void setYOffset( int );
    virtual void setOffset( int x, int y, bool updateScrBars = TRUE );

    virtual int cellWidth( int col );
    virtual int cellHeight( int row );
    int		cellWidth()	const;
    int		cellHeight()	const;
    virtual void setCellWidth( int );
    virtual void setCellHeight( int );

    virtual int totalWidth();
    virtual int totalHeight();

    uint	tableFlags()	const;
    bool	testTableFlags( uint f ) const;
    virtual void setTableFlags( uint f );
    void	clearTableFlags( uint f = ~0 );

    bool	autoUpdate()	 const;
    virtual void setAutoUpdate( bool );

    void	updateCell( int row, int column, bool erase=TRUE );

    QRect	cellUpdateRect() const;
    QRect	viewRect()	 const;

    int		lastRowVisible() const;
    int		lastColVisible() const;

    bool	rowIsVisible( int row ) const;
    bool	colIsVisible( int col ) const;

    QScrollBar *verticalScrollBar() const;
    QScrollBar *horizontalScrollBar() const;

private slots:
    void	horSbValue( int );
    void	horSbSliding( int );
    void	horSbSlidingDone();
    void	verSbValue( int );
    void	verSbSliding( int );
    void	verSbSlidingDone();

protected:
    virtual void paintCell( QPainter *, int row, int col ) = 0;
    virtual void setupPainter( QPainter * );

    void	paintEvent( QPaintEvent * );
    void	resizeEvent( QResizeEvent * );

    int		findRow( int yPos ) const;
    int		findCol( int xPos ) const;

    bool	rowYPos( int row, int *yPos ) const;
    bool	colXPos( int col, int *xPos ) const;

    int		maxXOffset();
    int		maxYOffset();
    int		maxColOffset();
    int		maxRowOffset();

    int		minViewX()	const;
    int		minViewY()	const;
    int		maxViewX()	const;
    int		maxViewY()	const;
    int		viewWidth()	const;
    int		viewHeight()	const;

    void	scroll( int xPixels, int yPixels );
    void	updateScrollBars();
    void	updateTableSize();

private:
    void	coverCornerSquare( bool );
    void	snapToGrid( bool horizontal, bool vertical );
    virtual void	setHorScrollBar( bool on, bool update = TRUE );
    virtual void	setVerScrollBar( bool on, bool update = TRUE );
    void	updateView();
    int		findRawRow( int yPos, int *cellMaxY, int *cellMinY = 0,
			    bool goOutsideView = FALSE ) const;
    int		findRawCol( int xPos, int *cellMaxX, int *cellMinX = 0,
			    bool goOutsideView = FALSE ) const;
    int		maxColsVisible() const;

    void	updateScrollBars( uint );
    void	updateFrameSize();

    void	doAutoScrollBars();
    void	showOrHideScrollBars();

    int		nRows;
    int		nCols;
    int		xOffs, yOffs;
    int		xCellOffs, yCellOffs;
    short	xCellDelta, yCellDelta;
    short	cellH, cellW;

    uint	eraseInPaint		: 1;
    uint	verSliding		: 1;
    uint	verSnappingOff		: 1;
    uint	horSliding		: 1;
    uint	horSnappingOff		: 1;
    uint	coveringCornerSquare	: 1;
    uint	sbDirty			: 8;
    uint	inSbUpdate		: 1;

    uint	tFlags;
    QRect	cellUpdateR;

    QScrollBar *vScrollBar;
    QScrollBar *hScrollBar;
    QCornerSquare *cornerSquare;

private:	// Disabled copy constructor and operator=
#if defined(Q_DISABLE_COPY)
    QTableView( const QTableView & );
    QTableView &operator=( const QTableView & );
#endif
};


const uint Tbl_vScrollBar	= 0x00000001;
const uint Tbl_hScrollBar	= 0x00000002;
const uint Tbl_autoVScrollBar	= 0x00000004;
const uint Tbl_autoHScrollBar	= 0x00000008;
const uint Tbl_autoScrollBars	= 0x0000000C;

const uint Tbl_clipCellPainting = 0x00000100;
const uint Tbl_cutCellsV	= 0x00000200;
const uint Tbl_cutCellsH	= 0x00000400;
const uint Tbl_cutCells		= 0x00000600;

const uint Tbl_scrollLastHCell	= 0x00000800;
const uint Tbl_scrollLastVCell	= 0x00001000;
const uint Tbl_scrollLastCell	= 0x00001800;

const uint Tbl_smoothHScrolling = 0x00002000;
const uint Tbl_smoothVScrolling = 0x00004000;
const uint Tbl_smoothScrolling	= 0x00006000;

const uint Tbl_snapToHGrid	= 0x00008000;
const uint Tbl_snapToVGrid	= 0x00010000;
const uint Tbl_snapToGrid	= 0x00018000;


inline int QTableView::numRows() const
{ return nRows; }

inline int QTableView::numCols() const
{ return nCols; }

inline int QTableView::topCell() const
{ return yCellOffs; }

inline int QTableView::leftCell() const
{ return xCellOffs; }

inline int QTableView::xOffset() const
{ return xOffs; }

inline int QTableView::yOffset() const
{ return yOffs; }

inline int QTableView::cellHeight() const
{ return cellH; }

inline int QTableView::cellWidth() const
{ return cellW; }

inline uint QTableView::tableFlags() const
{ return tFlags; }

inline bool QTableView::testTableFlags( uint f ) const
{ return (tFlags & f) != 0; }

inline QRect QTableView::cellUpdateRect() const
{ return cellUpdateR; }

inline bool QTableView::autoUpdate() const
{ return isUpdatesEnabled(); }

inline void QTableView::repaint( bool erase )
{ repaint( 0, 0, width(), height(), erase ); }

inline void QTableView::repaint( const QRect &r, bool erase )
{ repaint( r.x(), r.y(), r.width(), r.height(), erase ); }

inline void QTableView::updateScrollBars()
{ updateScrollBars( 0 ); }


#endif // QT_NO_TABLEVIEW

#endif // QTABLEVIEW_H
