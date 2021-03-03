/****************************************************************************
** $Id: qt/examples/mail/smtp.h   2.3.2   edited 2001-01-26 $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef SMTP_H
#define SMTP_H

#include <qobject.h>
#include <qstring.h>

class QSocket;
class QTextStream;
class QDns;

class Smtp : public QObject
{
    Q_OBJECT

public:
    Smtp( const QString &, const QString &, const QString &, const QString & );
    ~Smtp();

signals:
    void finished();
    void status( const QString & );

private slots:
    void readyRead();
    void connected();
    void deleteMe();
    void dnsLookupHelper();

private:
    enum State {
	Init,
	Mail,
	Rcpt,
	Data,
	Body,
	Quit,
	Close
    };

    QString message;
    QString from;
    QString rcpt;
    QSocket *socket;
    QTextStream * t;
    int state;
    QString response;
    QDns * mxLookup;
};

#endif
