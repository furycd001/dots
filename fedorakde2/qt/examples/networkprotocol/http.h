/****************************************************************************
** $Id: qt/examples/networkprotocol/http.h   2.3.2   edited 2001-01-26 $
**
** Implementation of Network Extension Library
**
** Created : 970521
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef QHTTP_H
#define QHTTP_H

#ifndef QT_H
#include "qsocket.h"
#include "qapplication.h"
#include "qstring.h"
#include "qurlinfo.h"
#include "qnetworkprotocol.h"
#include "qurloperator.h"
#endif // QT_H

#ifndef QT_NO_NETWORKPROTOCOL_HTTP

class Http : public QNetworkProtocol
{
    Q_OBJECT

public:
    Http();
    virtual ~Http();
    virtual int supportedOperations() const;

protected:
    virtual void operationGet( QNetworkOperation *op );
    virtual void operationPut( QNetworkOperation *op );

    QSocket *commandSocket;
    bool connectionReady, passiveMode;

private:
    bool checkConnection( QNetworkOperation *op );
    void close();

protected slots:
    void hostFound();
    void connected();
    void closed();
    void readyRead();

};

#endif // QT_NO_NETWORKPROTOCOL_HTTP

#endif // QHTTP_H
