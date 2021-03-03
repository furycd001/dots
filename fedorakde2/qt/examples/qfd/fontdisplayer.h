/****************************************************************************
** $Id: qt/examples/qfd/fontdisplayer.h   2.3.2   edited 2001-01-26 $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef FontDisplayer_H
#define FontDisplayer_H

#include <qframe.h>
#include <qmainwindow.h>

class QSlider;

class FontRowTable : public QFrame {
    Q_OBJECT
public:
    FontRowTable( QWidget* parent=0, const char* name=0 );

    QSize sizeHint() const;

signals:
    void fontInformation(const QString&);

public slots:
    void setRow(int);


protected:
    QSize cellSize() const;
    void paintEvent( QPaintEvent* );
private:
    int row;
};

class FontDisplayer : public QMainWindow {
    Q_OBJECT
public:
    FontDisplayer( QWidget* parent=0, const char* name=0 );
};

#endif
