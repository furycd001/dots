/****************************************************************************
** $Id: qt/examples/life/life.h   2.3.2   edited 2001-01-26 $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef LIFE_H
#define LIFE_H

#include <qframe.h>


class LifeWidget : public QFrame
{
    Q_OBJECT
public:
    LifeWidget( int s = 10, QWidget *parent = 0, const char *name = 0 );

    void	setPoint( int i, int j );

    int		maxCol() { return maxi; }
    int		maxRow() { return maxj; }

public slots:
    void	nextGeneration();
    void	clear();

protected:
    virtual void paintEvent( QPaintEvent * );
    virtual void mouseMoveEvent( QMouseEvent * );
    virtual void mousePressEvent( QMouseEvent * );
    virtual void resizeEvent( QResizeEvent * );
    void	 mouseHandle( const QPoint &pos );

private:
    enum { MAXSIZE = 50, MINSIZE = 10, BORDER = 5 };

    bool	cells[2][MAXSIZE + 2][MAXSIZE + 2];
    int		current;
    int		maxi, maxj;

    int pos2index( int x )
    {
	return ( x - BORDER ) / SCALE + 1;
    }
    int index2pos( int i )
    {
	return	( i - 1 ) * SCALE + BORDER;
    }

    int SCALE;
};


#endif // LIFE_H
