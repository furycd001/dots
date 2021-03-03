/****************************************************************************
**
** Definition of QTable widget class
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

#ifndef QTABLE_H
#define QTABLE_H

#ifndef QT_H
#include <qscrollview.h>
#include <qpixmap.h>
#include <qvector.h>
#include <qheader.h>
#include <qarray.h>
#include <qlist.h>
#include <qguardedptr.h>
#include <qshared.h>
#endif // QT_H


#ifndef QT_NO_TABLE

class QTableHeader;
class QValidator;
class QTable;
class QPaintEvent;
class QTimer;
class QResizeEvent;


struct QTablePrivate;
struct QTableHeaderPrivate;


class Q_EXPORT QTableSelection
{
public:
    QTableSelection();
    void init( int row, int col );
    void expandTo( int row, int col );
    bool operator==( const QTableSelection &s ) const;

    int topRow() const { return tRow; }
    int bottomRow() const { return bRow; }
    int leftCol() const { return lCol; }
    int rightCol() const { return rCol; }
    int anchorRow() const { return aRow; }
    int anchorCol() const { return aCol; }

    bool isActive() const { return active; }

private:
    uint active : 1;
    uint inited : 1;
    int tRow, lCol, bRow, rCol;
    int aRow, aCol;
};


class Q_EXPORT QTableItem : public Qt
{
public:
    enum EditType { Never, OnTyping, WhenCurrent, Always };

    QTableItem( QTable *table, EditType et, const QString &text );
    QTableItem( QTable *table, EditType et, const QString &text,
                const QPixmap &p );
    virtual ~QTableItem();

    virtual QPixmap pixmap() const;
    virtual QString text() const;
    virtual void setPixmap( const QPixmap &p );
    virtual void setText( const QString &t );
    QTable *table() const { return t; }

    virtual int alignment() const;
    virtual void setWordWrap( bool b );
    bool wordWrap() const;

    EditType editType() const;
    virtual QWidget *createEditor() const;
    virtual void setContentFromEditor( QWidget *w );
    virtual void setReplaceable( bool );
    bool isReplaceable() const;

    virtual QString key() const;
    virtual QSize sizeHint() const;

    virtual void setSpan( int rs, int cs );
    int rowSpan() const;
    int colSpan() const;

    virtual void setRow( int r );
    virtual void setCol( int c );
    int row() const;
    int col() const;

    virtual void paint( QPainter *p, const QColorGroup &cg,
                        const QRect &cr, bool selected );

    void updateEditor( int oldRow, int oldCol );

private:
    QString txt;
    QPixmap pix;
    QTable *t;
    EditType edType;
    uint wordwrap : 1;
    uint tcha : 1;
    int rw, cl;
    int rowspan, colspan;

};


#if defined(Q_TEMPLATEDLL)
// MOC_SKIP_BEGIN
template class Q_EXPORT QVector<QTableItem>;
template class Q_EXPORT QVector<QWidget>;
template class Q_EXPORT QList<QTableSelection>;
// MOC_SKIP_END
#endif


class Q_EXPORT QTable : public QScrollView
{
    Q_OBJECT
    Q_PROPERTY( int numRows READ numRows WRITE setNumRows )
    Q_PROPERTY( int numCols READ numCols WRITE setNumCols )
    Q_PROPERTY( bool showGrid READ showGrid WRITE setShowGrid )
    Q_PROPERTY( bool rowMovingEnabled READ rowMovingEnabled WRITE setRowMovingEnabled )
    Q_PROPERTY( bool columnMovingEnabled READ columnMovingEnabled WRITE setColumnMovingEnabled )

    friend class QTableHeader;

public:
    QTable( QWidget *parent = 0, const char *name = 0 );
    QTable( int numRows, int numCols,
            QWidget *parent = 0, const char *name = 0 );
    ~QTable();

    QHeader *horizontalHeader() const;
    QHeader *verticalHeader() const;

    enum SelectionMode { Single, Multi, NoSelection  };
    virtual void setSelectionMode( SelectionMode mode );
    SelectionMode selectionMode() const;

    virtual void setItem( int row, int col, QTableItem *item );
    virtual void setText( int row, int col, const QString &text );
    virtual void setPixmap( int row, int col, const QPixmap &pix );
    virtual QTableItem *item( int row, int col ) const;
    virtual QString text( int row, int col ) const;
    virtual QPixmap pixmap( int row, int col ) const;
    virtual void clearCell( int row, int col );

    virtual QRect cellGeometry( int row, int col ) const;
    virtual int columnWidth( int col ) const;
    virtual int rowHeight( int row ) const;
    virtual int columnPos( int col ) const;
    virtual int rowPos( int row ) const;
    virtual int columnAt( int pos ) const;
    virtual int rowAt( int pos ) const;

    int numRows() const;
    int numCols() const;

    void updateCell( int row, int col );

    bool eventFilter( QObject * o, QEvent * );

    int currentRow() const { return curRow; }
    int currentColumn() const { return curCol; }
    void ensureCellVisible( int row, int col );

    bool isSelected( int row, int col ) const;
    bool isRowSelected( int row, bool full = FALSE ) const;
    bool isColumnSelected( int col, bool full = FALSE ) const;
    int numSelections() const;
    QTableSelection selection( int num ) const;
    virtual int addSelection( const QTableSelection &s );
    virtual void removeSelection( const QTableSelection &s );
    virtual void removeSelection( int num );
    virtual int currentSelection() const;

    bool showGrid() const;

    bool columnMovingEnabled() const;
    bool rowMovingEnabled() const;

    virtual void sortColumn( int col, bool ascending = TRUE,
                             bool wholeRows = FALSE );
    bool sorting() const;

    virtual void takeItem( QTableItem *i );

    virtual void setCellWidget( int row, int col, QWidget *e );
    virtual QWidget *cellWidget( int row, int col ) const;
    virtual void clearCellWidget( int row, int col );

    virtual void paintCell( QPainter *p, int row, int col,
                            const QRect &cr, bool selected );
    virtual void paintFocus( QPainter *p, const QRect &r );
    QSize sizeHint() const;

public slots:
    virtual void setNumRows( int r );
    virtual void setNumCols( int r );
    virtual void setShowGrid( bool b );
    virtual void hideRow( int row );
    virtual void hideColumn( int col );
    virtual void showRow( int row );
    virtual void showColumn( int col );

    virtual void setColumnWidth( int col, int w );
    virtual void setRowHeight( int row, int h );

    virtual void adjustColumn( int col );
    virtual void adjustRow( int row );

    virtual void setColumnStretchable( int col, bool stretch );
    virtual void setRowStretchable( int row, bool stretch );
    bool isColumnStretchable( int col ) const;
    bool isRowStretchable( int row ) const;
    virtual void setSorting( bool b );
    virtual void swapRows( int row1, int row2 );
    virtual void swapColumns( int col1, int col2 );
    virtual void swapCells( int row1, int col1, int row2, int col2 );

    virtual void setLeftMargin( int m );
    virtual void setTopMargin( int m );
    virtual void setCurrentCell( int row, int col );
    void clearSelection( bool repaint = TRUE );
    virtual void setColumnMovingEnabled( bool b );
    virtual void setRowMovingEnabled( bool b );

protected:
    void drawContents( QPainter *p, int cx, int cy, int cw, int ch );
    void contentsMousePressEvent( QMouseEvent* );
    void contentsMouseMoveEvent( QMouseEvent* );
    void contentsMouseDoubleClickEvent( QMouseEvent* );
    void contentsMouseReleaseEvent( QMouseEvent* );
    void keyPressEvent( QKeyEvent* );
    void focusInEvent( QFocusEvent* );
    void focusOutEvent( QFocusEvent* );
    void resizeEvent( QResizeEvent * );
    void showEvent( QShowEvent *e );

    virtual void paintEmptyArea( QPainter *p, int cx, int cy, int cw, int ch );
    virtual void activateNextCell();
    virtual QWidget *createEditor( int row, int col, bool initFromCell ) const;
    virtual void setCellContentFromEditor( int row, int col );
    virtual QWidget *beginEdit( int row, int col, bool replace );
    virtual void endEdit( int row, int col, bool accept, bool replace );

    virtual void resizeData( int len );
    virtual void insertWidget( int row, int col, QWidget *w );
    int indexOf( int row, int col ) const;

protected slots:
    virtual void columnWidthChanged( int col );
    virtual void rowHeightChanged( int row );
    virtual void columnIndexChanged( int s, int oi, int ni );
    virtual void rowIndexChanged( int s, int oi, int ni );
    virtual void columnClicked( int col );

signals:
    void currentChanged( int row, int col );
    void clicked( int row, int col, int button, const QPoint &mousePos );
    void doubleClicked( int row, int col, int button, const QPoint &mousePos );
    void pressed( int row, int col, int button, const QPoint &mousePos );
    void selectionChanged();
    void valueChanged( int row, int col );

private slots:
    void doAutoScroll();

private:
    enum EditMode { NotEditing, Editing, Replacing };

    void updateGeometries();
    void repaintSelections( QTableSelection *oldSelection,
                            QTableSelection *newSelection,
                            bool updateVertical = TRUE,
                            bool updateHorizontal = TRUE );
    QRect rangeGeometry( int topRow, int leftCol,
                         int bottomRow, int rightCol, bool &optimize );
    void fixRow( int &row, int y );
    void fixCol( int &col, int x );

    void init( int numRows, int numCols );
    QSize tableSize() const;
    bool isEditing() const;
    void repaintCell( int row, int col );
    void contentsToViewport2( int x, int y, int& vx, int& vy );
    QPoint contentsToViewport2( const QPoint &p );
    void viewportToContents2( int vx, int vy, int& x, int& y );
    QPoint viewportToContents2( const QPoint &p );

    void updateRowWidgets( int row );
    void updateColWidgets( int col );

    bool isRowHidden( int ) const;
    bool isColumnHidden( int ) const;
private:
    QVector<QTableItem> contents;
    QVector<QWidget> widgets;
    int curRow;
    int curCol;
    QTableHeader *leftHeader, *topHeader;
    EditMode edMode;
    int editCol, editRow;
    QList<QTableSelection> selections;
    QTableSelection *currentSel;
    QTimer *autoScrollTimer;
    bool sGrid, mRows, mCols;
    int lastSortCol;
    bool asc;
    bool doSort;
    bool mousePressed;
    SelectionMode selMode;
    int pressedRow, pressedCol;
    QTablePrivate *d;

};


#if defined(Q_TEMPLATEDLL)
// MOC_SKIP_BEGIN
template class Q_EXPORT QArray<int>;
template class Q_EXPORT QArray<bool>;
// MOC_SKIP_END
#endif


class Q_EXPORT QTableHeader : public QHeader
{
    Q_OBJECT
public:
    enum SectionState {
        Normal,
        Bold,
        Selected
    };

    QTableHeader( int, QTable *t, QWidget *parent=0, const char *name=0 );
    ~QTableHeader() {};
    void addLabel( const QString &s , int size );

    void setSectionState( int s, SectionState state );
    SectionState sectionState( int s ) const;

    int sectionSize( int section ) const;
    int sectionPos( int section ) const;
    int sectionAt( int section ) const;

    void setSectionStretchable( int s, bool b );
    bool isSectionStretchable( int s ) const;

signals:
    void sectionSizeChanged( int s );

protected:
    void paintEvent( QPaintEvent *e );
    void paintSection( QPainter *p, int index, QRect fr );
    void mousePressEvent( QMouseEvent *e );
    void mouseMoveEvent( QMouseEvent *e );
    void mouseReleaseEvent( QMouseEvent *e );
    void mouseDoubleClickEvent( QMouseEvent *e );
    void resizeEvent( QResizeEvent *e );

private slots:
    void doAutoScroll();
    void sectionWidthChanged( int col, int os, int ns );
    void indexChanged( int sec, int oldIdx, int newIdx );
    void updateStretches();
    void updateWidgetStretches();

private:
    void updateSelections();
    void saveStates();
    void setCaching( bool b );
    void swapSections( int oldIdx, int newIdx );
    bool doSelection( QMouseEvent *e );

private:
    QArray<int> states, oldStates;
    QArray<bool> stretchable;
    QArray<int> sectionSizes, sectionPoses;
    bool mousePressed;
    int pressPos, startPos, endPos;
    QTable *table;
    QTimer *autoScrollTimer;
    QWidget *line1, *line2;
    bool caching;
    int resizedSection;
    bool isResizing;
    int numStretches;
    QTimer *stretchTimer, *widgetStretchTimer;
    QTableHeaderPrivate *d;

};

#endif // QT_NO_TABLE
#endif // TABLE_H
