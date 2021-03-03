/****************************************************************************
** $Id: qt/examples/networkprotocol/nntp.h   2.3.2   edited 2001-01-26 $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef NNTP_H
#define NNTP_H

#include <qsocket.h>
#include <qnetworkprotocol.h>

class Nntp : public QNetworkProtocol
{
    Q_OBJECT

public:
    Nntp();
    virtual ~Nntp();
    virtual int supportedOperations() const;

protected:
    virtual void operationListChildren( QNetworkOperation *op );
    virtual void operationGet( QNetworkOperation *op );

    QSocket *commandSocket;
    bool connectionReady;
    bool readGroups;
    bool readArticle;

private:
    bool checkConnection( QNetworkOperation *op );
    void close();
    void parseGroups();
    void parseArticle();

protected slots:
    void hostFound();
    void connected();
    void closed();
    void readyRead();
    void error( int );

};

#endif
