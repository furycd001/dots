/*
Copyright (c) 1999 Preston Brown <pbrown@kde.org>
Copyright (c) 1999 Matthias Ettrich <ettrich@kde.org>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#ifndef DCOPSERVER_H
#define DCOPSERVER_H "$Id$"

#include <qobject.h>
#include <qstring.h>
#include <qsocketnotifier.h>
#include <qlist.h>
#include <qvaluelist.h>
#include <qcstring.h>
#include <qdict.h>
#include <qptrdict.h>
#include <qapplication.h>

#define INT32 QINT32
#include <X11/Xlib.h>
#include <X11/Xmd.h>
#include <X11/ICE/ICElib.h>
extern "C" {
#include <X11/ICE/ICEutil.h>
#include <X11/ICE/ICEmsg.h>
#include <X11/ICE/ICEproto.h>
}

class DCOPConnection;
class DCOPListener;
class DCOPSignalConnectionList;
class DCOPSignals;
class QTimer;

typedef QValueList<QCString> QCStringList;

/**
 * @internal
 */
class DCOPConnection : public QSocketNotifier
{
public:
    DCOPConnection( IceConn conn );
    ~DCOPConnection();

    DCOPSignalConnectionList *signalConnectionList();

    // Add the data from offset @p start in @p _data to the output 
    // buffer and schedule it for later transmission.
    void waitForOutputReady(const QByteArray &_data, int start);

    // Called from DCOPServer::slotOutputReady()
    // Flush the output buffer.
    void slotOutputReady();

    QCString appId;
    QCString plainAppId;
    IceConn iceConn;
    int notifyRegister;
    /**
     * When client A has called client B then for the duration of the call:
     * A->waitingOnReply contains B 
     *   and either
     * B->waitingForReply contains A
     *   or
     * B->waitingForDelayedReply contains A
     *
     * This allows us to do proper bookkeeping in case client A, client B 
     * or both unregister during the call.
     */
    QList <_IceConn> waitingOnReply;
    QList <_IceConn> waitingForReply;
    QList <_IceConn> waitingForDelayedReply;
    DCOPSignalConnectionList *_signalConnectionList;
    bool daemon;
    bool outputBlocked;
    QValueList <QByteArray> outputBuffer;
    unsigned long outputBufferStart;
    QSocketNotifier *outputBufferNotifier;
};


/**
 * @internal
 */
class DCOPServer : public QObject
{
    Q_OBJECT
public:
    DCOPServer(bool _only_local, bool _suicide);
    ~DCOPServer();

    void* watchConnection( IceConn iceConn );
    void removeConnection( void* data );
    void processMessage( IceConn iceConn, int opcode, unsigned long length, Bool swap);
    void ioError( IceConn iceConn );

    bool receive(const QCString &app, const QCString &obj,
                 const QCString &fun, const QByteArray& data,
                 QCString& replyType, QByteArray &replyData, IceConn iceConn);

    DCOPConnection *findApp(const QCString &appId);
    DCOPConnection *findConn(IceConn iceConn)
       { return clients.find(iceConn); }
       
    void sendMessage(DCOPConnection *conn, const QCString &sApp,
                     const QCString &rApp, const QCString &rObj,
                     const QCString &rFun, const QByteArray &data);

private slots:
    void newClient( int socket );
    void processData( int socket );
    void slotTerminate();
    void slotSuicide();
    void slotCleanDeadConnections();
    void slotOutputReady(int socket );

private:
    bool suicide;
    int majorOpcode;
    int currentClientNumber;
    CARD32 serverKey;
    DCOPSignals *dcopSignals;
    QTimer *m_timer;
    QTimer *m_deadConnectionTimer;
    QList<DCOPListener> listener;
    QAsciiDict<DCOPConnection> appIds; // index on app id
    QPtrDict<DCOPConnection> clients; // index on iceConn
    QIntDict<DCOPConnection> fd_clients; // index on fd
    QList<_IceConn> deadConnections;
};

extern DCOPServer* the_server;

#endif
