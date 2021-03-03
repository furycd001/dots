/* This file is part of the KDE kdebase package
 *
 * Copyright (C) 1999-2000 Kurt Granroth <granroth@kde.org>
 *
 * This file is distributed under the BSD license. See the file "BSD"
 * in the subdirectory "licenses" of the package for the full license
 * text which has to be applied for this file.
 */
#include <kxmlrpcdaemon.h>
#include <kxmlrpcutil.h>
#include <kdebug.h>
#include <dcopclient.h>
#include <dcopref.h>

#include <qdir.h>
#include <qfile.h>
#include <qtextstream.h>
#include <kuniqueapp.h>

#include <ktrader.h>
#include <klocale.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>

#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

template QDataStream&operator>><QString, double>(QDataStream&, QMap<QString, double>&);
template QDataStream&operator>><QString,QString>(QDataStream&, QMap<QString, QString>&);
template QDataStream&operator>><QString,QArray<char> >(QDataStream&,QMap<QString,QArray<char> >&);
template QDataStream&operator>><QString,QDateTime>(QDataStream&,QMap<QString,QDateTime>&);
template QDataStream&operator>><QString,int>(QDataStream&,QMap<QString,int>&);

KXmlRpcDaemon::KXmlRpcDaemon()
    : KXmlRpcServer(0)
{
    // get our auth token
    m_authToken = KXmlRpcUtil::generateAuthToken();
    if (m_authToken == "")
    {
        kdFatal() << "kxmlrpcd: Could not get auth token. Exiting now!" << endl;
        exit(1);
    }

    // write out our port and auth token
    QFile auth_file(QDir::homeDirPath() + "/.kxmlrpcd");
    auth_file.remove();
    if (auth_file.open(IO_WriteOnly))
    {
        QTextStream auth_stream(&auth_file);
        auth_stream << port() << ",";
        auth_stream << m_authToken;
    }
    else
    {
        kdFatal() << "kxmlrpcd: Could not write " << QString(QDir::homeDirPath() + "/.kxmlrpcd") << ". Exiting now!" << endl;
        exit(1);
    }
    // make sure only the user can read the file
    fchmod(auth_file.handle(), S_IRUSR);
    auth_file.close();

    // WABA:
    // KUniqueApplication has already registered us
    m_dcopClient = kapp->dcopClient();

    // and start our proxy
    m_proxy = new KXmlRpcProxy(m_dcopClient);
}

KXmlRpcDaemon::~KXmlRpcDaemon()
{
    // unregister us
    m_dcopClient->detach();

    // nuke our .kxmlrpcd file
    QFile auth_file(QDir::homeDirPath() + "/.kxmlrpcd");
    auth_file.remove();
}

void KXmlRpcDaemon::dispatch(const QString& _name, const QString& _object,
                             const QString& _proto, const QByteArray& _params,
                             const QString& _auth)
{
    // do our authentication stuff, first.
    if (m_authToken != _auth)
    {
        // uh oh.. they don't authenticate!  give 'em hell!
        replyError("Unable to authenticate you!");
        return;
    }

    // first grab any requests meant for me
    if (_name == "kxmlrpcd")
    {
        if (_proto == "shutdown()")
        {
            reply();
            shutdown();
        }
        return;
    }

    // the other "pseudo server" is the trader
    if (_name == "trader")
    {
        processTrader(_proto, _params);
        return;
    }

    QByteArray reply_bytes;
    QDataStream reply_stream(reply_bytes, IO_ReadWrite);
    QCString reply_type;

    // do the actual call to the DCOP client
    if (m_dcopClient->call(_name.latin1(), _object.latin1(), _proto.latin1(),
                           _params, reply_type, reply_bytes, true))
    {
        processReturnValue(reply_type, reply_bytes);
    }
    else
        replyError("Could not complete request");
}

bool KXmlRpcDaemon::needAuth() const
{
    return true;
}

void KXmlRpcDaemon::processTrader(const QString& _proto,
                                  const QByteArray& _params)
{
    QDataStream ps(_params, IO_ReadWrite);
    if (_proto == "query(QMap<QString, QString>)")
    {
        QMap<QString, QString> args;
        ps >> args;

        kdDebug() << "ServiceType: " << args["ServiceType"] << endl;
        kdDebug() << "Constraint: " << args["Constraint"] << endl;
        kdDebug() << "Preferences: " << args["Preferences"] << endl;

        if (args["ServiceType"] == QString())
        {
            replyError("ServiceType cannot be null in query");
            return;
        }
        QString service_type(args["ServiceType"]);

        QString constraint(QString::null);
        if (args["Constraint"] != QString())
            constraint = args["Constraint"];
        QString preferences(QString::null);
        if (args["Preferences"] != QString())
            preferences = args["Preferences"];

        KTrader::OfferList offers = KTrader::self()->query(service_type,
                                                           constraint,
                                                           preferences);

        kdDebug() << "number of offers: " << offers.count() << endl;
        QValueList<QString> reply_list;
        KTrader::OfferList::Iterator it = offers.begin();
        for ( ; it != offers.end(); it++)
        {
            kdDebug() << "Returning " << (*it)->name() << endl;
            reply_list.append((*it)->name().latin1());
        }
        reply(reply_list);
    }
    else
        replyError("Cannot find object");
}

void KXmlRpcDaemon::processReturnValue(const QString& _type,
                                       const QByteArray& _reply)
{
    QDataStream rs(_reply, IO_ReadWrite);
    // handle the case where there is no reply
    if ((_type == "") || (_type == "void"))
        reply();

    // integers
    else if (_type == "int")
    {
        int elem;
        rs >> elem;
        reply(elem);
    }

    // double
    else if (_type == "double")
    {
        double elem;
        rs >> elem;
        reply(elem);
    }

    // any kind of string
    else if ((_type == "string") || (_type == "QString") ||
             (_type == "QString"))
    {
        QString elem;
        rs >> elem;
        reply(elem);
    }

    // base64
    else if (_type == "QByteArray")
    {
        QByteArray elem;
        rs >> elem;
        reply(elem);
    }

    // dateTime
    else if (_type == "QDateTime")
    {
        QDateTime elem;
        rs >> elem;
        reply(elem);
    }

    // all of the arrays (represented as lists).  unfortunately, all
    // of the possible permutations must be listed
    else if (_type == "QValueList<int>")
    {
        QValueList<int> elem;
        rs >> elem;
        reply(elem);
    }
    else if (_type == "QValueList<double>")
    {
        QValueList<double> elem;
        rs >> elem;
        reply(elem);
    }
    else if (_type == "QValueList<QString>")
    {
        QValueList<QString> elem;
        rs >> elem;
        reply(elem);
    }
    else if (_type == "QValueList<QByteArray>")
    {
        QValueList<QByteArray> elem;
        rs >> elem;
        reply(elem);
    }
    else if (_type == "QValueList<QDateTime>")
    {
        QValueList<QDateTime> elem;
        rs >> elem;
        reply(elem);
    }

    // all of the structs (represented as maps).  unfortunately, all
    // of the possible permutations must be listed
    else if (_type == "QMap<QString,int>")
    {
        QMap<QString, int> elem;
        rs >> elem;
        reply(elem);
    }
    else if (_type == "QMap<QString,double>")
    {
        QMap<QString, double> elem;
        rs >> elem;
        reply(elem);
    }
    else if (_type == "QMap<QString,QString>")
    {
        QMap<QString, QString> elem;
        rs >> elem;
        reply(elem);
    }
    else if (_type == "QMap<QString,QString>")
    {
        QMap<QString, QString> elem;
        rs >> elem;
        reply(elem);
    }
    else if (_type == "QMap<QString,QByteArray>")
    {
        QMap<QString, QByteArray> elem;
        rs >> elem;
        reply(elem);
    }
    else if (_type == "QMap<QString,QDateTime>")
    {
        QMap<QString, QDateTime> elem;
        rs >> elem;
        reply(elem);
    }

    // booleans
    else if (_type == "bool")
    {
        int elem;
        rs >> elem;
        replyBool((bool)elem);
    }

    // DCOPRefs
    else if (_type == "DCOPRef")
    {
        DCOPRef elem;
        rs >> elem;
        reply(elem);
    }

    // dunno what to do with this value
    else
    {
        QString err("Unknown return type: ");
        err += _type;
        replyError(err);
    }
}

KXmlRpcProxy::KXmlRpcProxy(DCOPClient *_client)
    : DCOPObjectProxy(_client)
{
}

bool KXmlRpcProxy::process(const QCString& obj, const QCString& fun,
                          const QByteArray& data, QCString& replyType,
                          QByteArray &replyData)
{
    // when I get around to it, this function will handle the proxy
    // service
    kdDebug() << "Proxy obj = " << obj.data() << endl;
    return false;
}

static void sighandler(int)
{
    // nuke our .kxmlrpcd file
    QFile auth_file(QDir::homeDirPath() + "/.kxmlrpcd");
    auth_file.remove();
    exit(0);
}

static const char *description =
    I18N_NOOP("XmlRpc to DCOP bridge daemon");

static const char *version = "v1.0.1";

static KCmdLineOptions cmdOptions[] =
{
   { "daemon", I18N_NOOP("Run as a daemon (detached from controlling process)"), 0 },
   { 0, 0, 0 }
};

int main(int argc, char **argv)
{
    KAboutData about("kxmlrpcd", I18N_NOOP("KXmlRpc Daemon"),
                     version, description, KAboutData::License_BSD);
    KCmdLineArgs::init(argc, argv, &about);
    KCmdLineArgs::addCmdLineOptions(cmdOptions);

    // WABA: Make sure not to enable session management.
    putenv("SESSION_MANAGER=");

    if (KUniqueApplication::start() == false)
        exit(0);
    KUniqueApplication app(false, false);
    app.disableSessionManagement();

    KXmlRpcDaemon daemon;

    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
    if (args->isSet("daemon"))
    {
        // let's be a "real" daemon
        // KUniqueApplication did the first fork, we now need
        // to do a second fork to get rid of the controlling terminal
        setsid();
        if (fork() > 0)
            exit(0);    // controlling terminal
    }

    signal(SIGTERM, sighandler);
    signal(SIGINT,  sighandler);

    return app.exec();
}
