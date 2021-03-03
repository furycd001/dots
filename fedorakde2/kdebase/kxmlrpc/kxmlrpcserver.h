/* This file is part of the KDE kdebase package
 *
 * Copyright (C) 1999-2000 Kurt Granroth <granroth@kde.org>
 *
 * This file is distributed under the BSD license. See the file "BSD"
 * in the subdirectory "licenses" of the package for the full license
 * text which has to be applied for this file.
 */
#ifndef KXMLRPCSERVER_H
#define KXMLRPCSERVER_H
#include <qobject.h>
#include <qmap.h>
#include <qstring.h>
#include <qcstring.h>
#include <qdatetime.h>
#include <qarray.h>

class KSocket;
class KServerSocket;
class DCOPRef;

/**
 * This class is the base server class for all applications that wish
 * to be Xml-Rpc servers.  It is an abstract class, so all applications
 * MUST inherit from it and implement the dispatch() function.
 *
 * @short Base server class for embedding XmlRpc into applications
 * @author Kurt Granroth <granroth@kde.org>
 * @version 0.1
 */
class KXmlRpcServer : public QObject
{
    Q_OBJECT
public:
    /**
     * Constructor.  This accepts the port that we will be listening
     * on.  For most web servers (which this is), this value will be
     * 80.  However, because this object is almost certainly embedded,
     * this value may be wildly different -- hence, no default.
     */
    KXmlRpcServer(unsigned short int port);

    /**
     * Standard destructor
     */
    virtual ~KXmlRpcServer();

    /**
     * Return the current port
     */
    unsigned short int port();

protected:
    /**
     * You MUST implement this function!  When an incoming methodCall
     * comes from a client, this function will be called.  It your
     * responsibility to "map" this method to an actual callback or
     * function in your code.  Note that this implementation can be 
     * quite simple.
     *
     * This function will pass along the name of the server (you, in
     * this case) in case you want to double check that this is the
     * right call.  It also passes along the object name.  If you only
     * have one object, then you're set.. but if you have multiple
     * objects, you'll have to switch on this.
     *
     * The two most important params are '_proto' and '_param'.  The
     * former is a string resembling a function prototype.  The second
     * is a data stream of all of the function parameters.  You'll
     * have to match the former and pass along the latter.
     *
     * Here's an example:
     *
     * Say you have a slot 'slotAction(int)' in class Action.  This
     * should be called whenever the XmlRpc method action(int)
     * is invoked.  You need only two steps to do this:
     *
     * 1. Create a signal in your derived server and connect it to the
     *    slotAction 
     *    <code>
     *    connect(server, SIGNAL(sigAction(int)),
     *            action, SLOT(slotAction(int)));
     *    </code>
     *
     * 2. Map the XmlRpc method to this signal in your dispatch function
     *    <code>
     *    if (_proto == "action(int)")
     *    {
     *       QDataStream args(_param, IO_ReadWrite);
     *       int param;
     *       args >> param;
     *       sigAction(param);
     *    }
     *    </code>
     *
     * That's it!
     */
    virtual void dispatch(const QString& _name, const QString& _object,
                          const QString& _proto, const QByteArray& _param,
                          const QString& _auth) = 0;

    /**
     * This should be implemented by the derived class if
     * authentication is needed.  If this is true, then the first
     * incoming parameter is assumed to be the auth token
     */
    virtual bool needAuth() const
    {
        return false;
    }

protected:
    /**
     * Use this function is there is nothing to return.  This is the
     * equivalent of a 'void' return
     */
    void reply();

    /**
     * Returns a string
     */
    void reply(const QString& _return);

    /**
     * Returns an integer
     */
    void reply(int _return);

    /**
     * Returns a double
     */
    void reply(double _return);

    /**
     * Returns a base64/QByteArray
     */
    void reply(const QByteArray& _return);

    /**
     * Returns a dateTime/QDateTime
     */
    void reply(const QDateTime& _return);

    /**
     * Returns an array of integers
     */
    void reply(const QValueList<int>& _return);

    /**
     * Returns an array of doubles
     */
    void reply(const QValueList<double>& _return);

    /**
     * Returns an array of strings
     */
    void reply(const QValueList<QString>& _return);

    /**
     * Returns an array of base64
     */
    void reply(const QValueList<QByteArray>& _return);

    /**
     * Returns an array of dates
     */
    void reply(const QValueList<QDateTime>& _return);

    /**
     * Returns a struct of integers
     */
    void reply(const QMap<QString, int>& _return);

    /**
     * Returns a struct of doubles
     */
    void reply(const QMap<QString, double>& _return);

    /**
     * Returns a struct of strings
     */
    void reply(const QMap<QString, QString>& _return);

    /**
     * Returns a struct of base64
     */
    void reply(const QMap<QString, QByteArray>& _return);

    /**
     * Returns a struct of dates
     */
    void reply(const QMap<QString, QDateTime>& _return);

    /**
     * Returns a boolean.  Note that the function name has to be
     * different as C++ insists that bool == int
     */
    void replyBool(bool _return);

    /**
     * Returns a DCOPRef object
     */
    void reply(const DCOPRef& _return);

    /**
     * Return a fault.  The _msg is a text message describing the
     * problem.  The _code is a numeric code suitable for parsing.
     * 999 should be used for most non system faults
     */
    void replyError(const QString& _msg, int _code = 999);

    /**
     * Safely shuts down the server
     */
    void shutdown();

private:
    void sendReply(const QString& _type, const QString& _value);

    bool addData(const QString& _data);
    void updateAttack(KSocket *);

private slots:
    void acceptConnection(KSocket *);

    void incomingData(KSocket *);
    void outgoingData(KSocket *);

    void socketTimeout();

private:
    KServerSocket *m_serverSocket;
    KSocket       *m_currentSocket;

    QString m_incomingXml;
    QString m_outgoingXml;

    QString m_server;
    bool     m_keepalive;

    bool m_complete;

    unsigned short int m_port;

    QMap<unsigned long, unsigned int> m_retryMap;
};

#endif // KXMLRPCSERVER_H
