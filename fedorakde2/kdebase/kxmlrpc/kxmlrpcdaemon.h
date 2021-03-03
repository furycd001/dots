/* This file is part of the KDE kdebase package
 *
 * Copyright (C) 1999-2000 Kurt Granroth <granroth@kde.org>
 *
 * This file is distributed under the BSD license. See the file "BSD"
 * in the subdirectory "licenses" of the package for the full license
 * text which has to be applied for this file.
 */
#ifndef KXMLRPCDAEMON_H
#define KXMLRPCDAEMON_H

#include <kxmlrpcserver.h>
#include <dcopobject.h>

class DCOPClient;
class KXmlRpcProxy;

/**
 * This class implements the KDE XmlRpc Daemon.  It is an XmlRpc
 * server running at a known port. It "translates" incoming XmlRpc
 * requests into DCOP requests and forwards them to the specified DCOP
 * client.  It then, as a DCOP client, will accept the return value
 * from the DCOP client and "translate" it back to XmlRpc.
 *
 * The advantage of this approach is that neither the XmlRpc client
 * nor the DCOP client/server need to know that they are "talking" the
 * other protocol.
 *
 * @short KDE XmlRpc Daemon
 * @author Kurt Granroth <granroth@kde.org>
 * @version 0.1
 */
class KXmlRpcDaemon : public KXmlRpcServer
{
public:
    /**
     * Standard constructor.  This will attach and register a DCOP
     * client.  It will register as 'xmlrpcd'.  If registration fails
     * for some reason, then the daemon will exit immediately.
     */
    KXmlRpcDaemon();

    /**
     * Standard destructor.  This detaches the client
     */
    virtual ~KXmlRpcDaemon();

protected:
    /**
     * Implementation of the virtual function 'dispatch'.  This
     * function is responsible for doing the actual DCOP call.  It
     * will also get the return value if there is one.
     */
    virtual void dispatch(const QString& _name, const QString& _object,
                          const QString& _proto, const QByteArray& _param,
                          const QString& _auth);

    /**
     * Implementation of needAuth().
     */
    virtual bool needAuth() const;

private:
    void processTrader(const QString& _proto, const QByteArray& _params);
    void processReturnValue(const QString& _type, const QByteArray& _reply);

private:
    DCOPClient   *m_dcopClient;
    QString       m_authToken;
    KXmlRpcProxy *m_proxy;
};

class KXmlRpcProxy : public DCOPObjectProxy
{
public:
    KXmlRpcProxy(DCOPClient *_client);

    virtual bool process(const QCString& obj, const QCString& fun,
                         const QByteArray& data, QCString& replyType,
                         QByteArray &replyData);

};
#endif // KXMLRPCDAEMON_H
