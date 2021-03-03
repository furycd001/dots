/*****************************************************************
ksmserver - the KDE session management server

Copyright (C) 2000 Matthias Ettrich <ettrich@kde.org>
******************************************************************/

#ifndef SERVER_H
#define SERVER_H

// needed to avoid clash with INT8 defined in X11/Xmd.h on solaris
#define QT_CLEAN_NAMESPACE 1
#include <qobject.h>
#include <qstring.h>
#include <qsocketnotifier.h>
#include <qlist.h>
#include <qvaluelist.h>
#include <qcstring.h>
#include <qdict.h>
#include <qqueue.h>
#include <qptrdict.h>
#include <qapplication.h>
#include <qtimer.h>

#define INT32 QINT32
#include <X11/Xlib.h>
#include <X11/Xmd.h>
#include <X11/ICE/ICElib.h>
extern "C" {
#include <X11/ICE/ICEutil.h>
#include <X11/ICE/ICEmsg.h>
#include <X11/ICE/ICEproto.h>
#include <X11/SM/SM.h>
#include <X11/SM/SMlib.h>
}

#include "dcopobject.h"

typedef QValueList<QCString> QCStringList;
class KSMListener;
class KSMConnection;
class KSMClient
{
public:
    KSMClient( SmsConn );
    ~KSMClient();

    void registerClient( const char* previousId  = 0 );
    SmsConn connection() const { return smsConn; }

    void resetState();
    uint saveYourselfDone : 1;
    uint pendingInteraction : 1;
    uint waitForPhase2 : 1;
    uint wasPhase2 : 1;

    QList<SmProp> properties;
    SmProp* property( const char* name ) const;

    QString program() const;
    QStringList restartCommand() const;
    QStringList discardCommand() const;
    int restartStyleHint() const;
    QString userId() const;

private:
    const char* clientId;
    SmsConn smsConn;
};

class KSMServer : public QObject, DCOPObject
{
Q_OBJECT
public:
    KSMServer( const QString& windowManager, bool only_local );
    ~KSMServer();

    void* watchConnection( IceConn iceConn );
    void removeConnection( KSMConnection* conn );

    KSMClient* newClient( SmsConn );
    void  deleteClient( KSMClient* client );

    // callbacks
    void saveYourselfDone( KSMClient* client, bool success );
    void interactRequest( KSMClient* client, int dialogType );
    void interactDone( KSMClient* client, bool cancelShutdown );
    void phase2Request( KSMClient* client );

    // error handling
    void ioError( IceConn iceConn );

    // DCOP processing
    bool process(const QCString &, const QByteArray &,
                 QCString&, QByteArray &);

    // notification
    void clientSetProgram( KSMClient* client );

    // public API
    void restoreSession();
    void startDefaultSession();

public slots:
    void shutdown();
    // If fast shutdown is select, the confirmation dialog will be skipped.
    // This allows users whose default is to confirm to shutdown immediately.
    // It does not yet allow for confirmation when immediate shutdown is the default.
    void shutdown( bool bFast );
    void cleanUp();

private slots:
    void newConnection( int socket );
    void processData( int socket );
    void timeoutQuit();
    void restoreSessionInternal();

    void protectionTimeout();
    void cancelShutdown();

    void autoStart();

private:
    void handlePendingInteractions();
    void completeShutdown();
    void completeKilling();

    void discardSession();
    void storeSesssion();

    void startProtection();
    void endProtection();

    void startApplication( const QStringList& command );
    void executeCommand( const QStringList& command );

 private:
    QList<KSMListener> listener;
    QList<KSMClient> clients;

    enum State { Idle, Shutdown, Killing }; // KSMServer does not support pure checkpoints yet.
    State state;
    bool dialogActive;
    bool saveSession;

    bool clean;
    KSMClient* clientInteracting;
    QString wm;
    QCString launcher;
    QTimer protection;

    // ksplash interface
    void upAndRunning( const QString& msg );
    void publishProgress( int progress, bool max  = false  );

    int progress;
    int appsToStart;
};

#endif

