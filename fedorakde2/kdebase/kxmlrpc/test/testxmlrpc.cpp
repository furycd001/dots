/* This file is part of the KDE kdebase package
 *
 * Copyright (C) 1999-2000 Kurt Granroth <granroth@kde.org>
 *
 * This file is distributed under the BSD license. See the file "BSD"
 * in the subdirectory "licenses" of the package for the full license
 * text which has to be applied for this file.
 */
#include <kuniqueapp.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <qstring.h>
#include <qcstring.h>
#include <dcopclient.h>
#include <dcopobject.h>
#include <qmap.h>
#include <qdatetime.h>

#include <kdebug.h>
/**
g++ -o testxmlrpc testxmlrpc.cpp -I$KDEDIR/include -I$QTDIR/include -L$KDEDIR/lib -L$QTDIR/lib -lkdecore -lqt
**/

class AddressServer : public DCOPObject
{
public:
    AddressServer() : DCOPObject("email")
    {
        m_addressMap.insert("Kurt Granroth", "granroth@kde.org");
        m_addressMap.insert("Preston Brown", "pbrown@kde.org");
        m_addressMap.insert("Matthias Ettrich", "ettrich@kde.org");
    }

    virtual bool process(const QCString &func, const QByteArray &data,
                         QCString &replyType, QByteArray &replyData)
    {
        kdDebug() << "Processing: " << func.data() << endl;
        if (func == "lookup(QString)")
        {
            QDataStream args(data, IO_ReadOnly);
            QString name;
            args >> name;
            kdDebug() << "name = " << name.data() << endl;

            QDataStream reply(replyData, IO_ReadWrite);
            reply << lookup(name);
            replyType = "QString";

            return true;
        }

        if (func == "addAddress(QString,QString)")
        {
            QDataStream args(data, IO_ReadOnly);
            QString name, address;
            args >> name >> address;

            addAddress(name, address);
            replyType = "void";

            return true;
        }

        if (func == "deleteAddress(QString)")
        {
            QDataStream args(data, IO_ReadOnly);
            QString name;
            args >> name;

            deleteAddress(name);
            replyType = "void";

            return true;
        }

        return false;

    }

private:
    QString lookup(const QString& _name)
    {
        kdDebug() << _name.data() << " -> " << m_addressMap[_name].data() << endl;
        return m_addressMap[_name];
    }

    void addAddress(const QString& _name, const QString& _address)
    {
        m_addressMap.insert(_name, _address);
    }

    void deleteAddress(const QString& _name)
    {
        m_addressMap.remove(_name);
    }

private:
    QMap<QString, QString> m_addressMap;
};

class TypeServer : public DCOPObject
{
public:
    TypeServer() : DCOPObject("types")
    {
    }

    virtual bool process(const QCString &func, const QByteArray &data,
                         QCString &replyType, QByteArray &replyData)
    {
        kdDebug() << "Processing: " << func.data() << endl;
        if (func == "fromBase64(QByteArray)")
        {
            QDataStream args(data, IO_ReadOnly);
            QByteArray array;
            args >> array;

            QDataStream reply(replyData, IO_ReadWrite);
            reply << QString(array.data());
            replyType = "QString";

            return true;
        }

        if (func == "toBase64(QString)")
        {
            QDataStream args(data, IO_ReadOnly);
            QString param;
            args >> param;

            QDataStream reply(replyData, IO_ReadWrite);
            reply << (QByteArray)QCString(param.data());
            replyType = "QByteArray";

            return true;
        }

        if (func == "currentTime()")
        {
            QDataStream reply(replyData, IO_ReadWrite);
            reply << QDateTime::currentDateTime();
            replyType = "QDateTime";

            return true;
        }

        if (func == "getTime(QDateTime)")
        {
            QDataStream args(data, IO_ReadOnly);
            QDateTime param;
            args >> param;

            QDataStream reply(replyData, IO_ReadWrite);
            reply << param.toString();
            replyType = "QString";

            return true;
        }

        if (func == "returnMyself(QMap<QString,int>)")
        {
            QDataStream args(data, IO_ReadOnly);
            QMap<QString, int> param;
            args >> param;

            QDataStream reply(replyData, IO_ReadWrite);
            reply << param;
            replyType = "QMap<QString,int>";

            return true;
        }


        return false;

    }
};

int main(int argc, char **argv)
{
    KAboutData aboutData( "testxmlrpc", "Test Xmlrpc",
        "$Id: testxmlrpc.cpp,v 1.13 2001/07/16 00:26:44 waba Exp $",
        "Test program for xmlrpc.");
 
    KCmdLineArgs::init(argc, argv, &aboutData);
    KUniqueApplication app("testxmlrpc", false);

    // start up my servers
    DCOPObject *address_server = new AddressServer;
    DCOPObject *type_server    = new TypeServer;

    int ret = app.exec();
    kapp->dcopClient()->detach();
    return ret;
}
