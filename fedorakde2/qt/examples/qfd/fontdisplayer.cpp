/****************************************************************************
** $Id: qt/examples/qfd/fontdisplayer.cpp   2.3.2   edited 2001-01-26 $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "fontdisplayer.h"
#include <qslider.h>
#include <qspinbox.h>
#include <qpainter.h>
#include <qtoolbar.h>
#include <qstatusbar.h>
#include <stdlib.h>


FontRowTable::FontRowTable( QWidget* parent, const char* name ) :
    QFrame(parent,name)
{
    setBackgroundMode(PaletteBase);
    setFrameStyle(Panel|Sunken);
    setMargin(8);
    setRow(0);
}

QSize FontRowTable::sizeHint() const
{
    return 16*cellSize()+QSize(2,2)*(margin()+frameWidth());
}

QSize FontRowTable::cellSize() const
{
    QFontMetrics fm = fontMetrics();
    return QSize( fm.maxWidth(), fm.lineSpacing()+1 );
}

void FontRowTable::paintEvent( QPaintEvent* e )
{
    QFrame::paintEvent(e);
    QPainter p(this);
    p.setClipRegion(e->region());
    QRect r = e->rect();
    QFontMetrics fm = fontMetrics();
    int ml = frameWidth()+margin() + 1 + QMAX(0,-fm.minLeftBearing());
    int mt = frameWidth()+margin();
    QSize cell((width()-15-ml)/16,(height()-15-mt)/16);

    if ( !cell.width() || !cell.height() )
	return;

    int mini = r.left() / cell.width();
    int maxi = (r.right()+cell.width()-1) / cell.width();
    int minj = r.top() / cell.height();
    int maxj = (r.bottom()+cell.height()-1) / cell.height();

    int h = fm.height();

    QColor body(255,255,192);
    QColor negative(255,192,192);
    QColor positive(192,192,255);
    QColor rnegative(255,128,128);
    QColor rpositive(128,128,255);

    for (int j = minj; j<=maxj; j++) {
	for (int i = mini; i<=maxi; i++) {
	    if ( i < 16 && j < 16 ) {
		int x = i*cell.width();
		int y = j*cell.height();

		QChar ch = QChar(j*16+i,row);

		if ( fm.inFont(ch) ) {
		    int w = fm.width(ch);
		    int l = fm.leftBearing(ch);
		    int r = fm.rightBearing(ch);

		    x += ml;
		    y += mt+h;

		    p.fillRect(x,y,w,-h,body);
		    if ( w ) {
			if ( l ) {
			    p.fillRect(x+(l>0?0:l), y-h/2, abs(l),-h/2,
				       l < 0 ? negative : positive);
			}
			if ( r ) {
			    p.fillRect(x+w-(r>0?r:0),y+2, abs(r),-h/2,
				       r < 0 ? rnegative : rpositive);
			}
		    }
		    QString s;
		    s += ch;
		    p.setPen(QPen(Qt::black));
		    p.drawText(x,y,s);
		}
	    }
	}
    }
}

void FontRowTable::setRow(int r)
{
    row = r;

    QFontMetrics fm = fontMetrics();
    QString str;
    str.sprintf("mLB=%d mRB=%d mW=%d",
	fm.minLeftBearing(),
	fm.minRightBearing(),
	fm.maxWidth()
	);
	
    emit fontInformation(str);
    update();
}

FontDisplayer::FontDisplayer( QWidget* parent, const char* name ) :
    QMainWindow(parent,name)
{
    FontRowTable* table = new FontRowTable(this);
    QToolBar* controls = new QToolBar(this);
    QSpinBox *row = new QSpinBox(0,255,1,controls);
    connect(row,SIGNAL(valueChanged(int)),table,SLOT(setRow(int)));
    connect(table,SIGNAL(fontInformation(const QString&)),
	statusBar(),SLOT(message(const QString&)));
    table->setRow(0);
    setCentralWidget(table);
}
