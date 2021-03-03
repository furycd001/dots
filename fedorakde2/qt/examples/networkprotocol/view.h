/****************************************************************************
** $Id: qt/examples/networkprotocol/view.h   2.3.2   edited 2001-01-26 $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef VIEW_H
#define VIEW_H

#include <qvbox.h>
#include <qcstring.h>
#include <qurloperator.h>

class QMultiLineEdit;

class View : public QVBox
{
    Q_OBJECT
    
public:
    View();
    
private slots:
    void downloadFile();
    void newData( const QByteArray &ba );

private:
    QMultiLineEdit *fileView;
    QUrlOperator op;
    
    QString getOpenFileName();
};

#endif
