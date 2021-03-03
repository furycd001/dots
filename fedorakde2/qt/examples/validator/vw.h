/****************************************************************************
** $Id: qt/examples/validator/vw.h   2.3.2   edited 2001-01-26 $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef VW_H
#define VW_H

#include <qvalidator.h>
#include <qstring.h>
#include <qwidget.h>

class VW: public QWidget {
    Q_OBJECT
public:
    VW( QWidget * parent = 0, const char * name = 0 );
    ~VW();

private slots:
    void modelSelected( const QString& );
    void motorSelected( int );
    void yearSelected( int );

signals:
    void validSelectionMade( const QString& );

private:
    void computeSelection();

    QString currentModel;
    int currentMotorSize;
    int currentYear;
};


#endif
