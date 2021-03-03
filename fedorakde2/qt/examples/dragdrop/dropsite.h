/****************************************************************************
** $Id: qt/examples/dragdrop/dropsite.h   2.3.2   edited 2001-01-26 $
**
** Drop site example implementation
**
** Created : 979899
**
** Copyright (C) 1997 by Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef DROPSITE_H
#define DROPSITE_H

#include <qlabel.h>
#include <qmovie.h>
#include "qdropsite.h"

class QDragObject;

class DropSite: public QLabel
{
    Q_OBJECT
public:
    DropSite( QWidget * parent = 0, const char * name = 0 );
    ~DropSite();

signals:
    void message( const QString& );

protected:
    void dragEnterEvent( QDragEnterEvent * );
    void dragMoveEvent( QDragMoveEvent * );
    void dragLeaveEvent( QDragLeaveEvent * );
    void dropEvent( QDropEvent * );
    void backgroundColorChange( const QColor& );

    // this is a normal even
    void mousePressEvent( QMouseEvent * );
};

class DragMoviePlayer : public QObject {
    Q_OBJECT
    QDragObject* dobj;
    QMovie movie;
public:
    DragMoviePlayer(QDragObject*);
private slots:
    void updatePixmap( const QRect& );
};


#endif
