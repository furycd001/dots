/* This file is part of the KDE kdebase package
 *
 * Copyright (C) 1999-2000 Kurt Granroth <granroth@kde.org>
 *
 * This file is distributed under the BSD license. See the file "BSD"
 * in the subdirectory "licenses" of the package for the full license
 * text which has to be applied for this file.
 */
#include <kxmlrpcserver.h>
#include <kxmlrpcparser.h>
#include <kxmlrpcutil.h>
#include <ksock.h>

#include <dcopref.h>

#include <kapp.h>
#include <kdebug.h>

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include <qdom.h>
#include <qtimer.h>

KXmlRpcServer::KXmlRpcServer(unsigned short int _port)
    : QObject(),
      m_serverSocket(0),
      m_currentSocket(0),
      m_incomingXml(""),
      m_outgoingXml(""),
      m_server(""),
      m_keepalive(false),
      m_complete(false),
      m_port(_port)
{
    // if our incoming port is 0, then we start at 18300 and go up
    // from there up to 19300 (roughly)
    if (m_port == 0)
    {
        m_port = 18300;
        for (; m_port < 19300; m_port++)
        {
            m_serverSocket = new KServerSocket(m_port, true);
            if (m_serverSocket->socket() != -1)
              break;
        }
    }
    else
        m_serverSocket = new KServerSocket(m_port, true);

    // we see if our socket is valid.  if it is not, then we might
    // as well return now since we can't do anything about it
    if (m_serverSocket->socket() == -1)
    {
        kdFatal() << "Could not create a server socket. Exiting now!" << endl;
        exit(1);
    }

    // connect our one and only slot
    connect(m_serverSocket, SIGNAL(accepted(KSocket *)),
            this,           SLOT(acceptConnection(KSocket *)));
}

unsigned short int KXmlRpcServer::port()
{
    if (m_serverSocket && (m_serverSocket->socket() != -1))
        return m_port;
    else
        return 0;
}

void KXmlRpcServer::acceptConnection(KSocket *_socket)
{
    // woohoo, we have an incoming connection!

    // make sure that we aren't blocking this IP, first
    unsigned long ip = _socket->ipv4_addr();
    if (m_retryMap.contains(ip))
    {
        // if they have tried and failed more than 5 times in a row,
        // we ban them
        if (m_retryMap[ip] >= 5)
        {
            delete _socket; _socket = 0;
            return;
        }
    }

    // we only want to read data at first
    _socket->enableRead(true);

    // connect our read and write events to slots so we can use them
    connect(_socket, SIGNAL(readEvent(KSocket*)),
            this,    SLOT(incomingData(KSocket *)));
    connect(_socket, SIGNAL(writeEvent(KSocket*)),
            this,    SLOT(outgoingData(KSocket *)));

    // allow up to 15 seconds for the call.. including everything
    QTimer *timer = new QTimer( _socket );  
    connect( timer, SIGNAL(timeout()), this, SLOT(socketTimeout()) );
    timer->start( 15000, true );
}

KXmlRpcServer::~KXmlRpcServer()
{
    // clean up
    delete m_serverSocket; m_serverSocket = 0;
}

void KXmlRpcServer::incomingData(KSocket *_socket)
{
    // make sure that this is our current socket
    if (m_currentSocket == 0)
        m_currentSocket = _socket;
    else if (m_currentSocket->socket() != _socket->socket())
            return;

    // read in data 1K at a time
    char buffer[1024];
    memset(buffer, 0, 1024);
    if (::read(_socket->socket(), buffer, 1024) == -1)
    {
        // TODO: i'm really not sure what to do in the case of an
        // error, here.  i'll just return for now...
        kdDebug() << "Error in ::read()" << endl;
        return;
    }

    // add data until the internal processing tells us that we
    // are done (complete)
    if (addData(buffer) == false)
    {
        // something really nasty has happened.  let's nuke this
        // socket and return
        delete _socket;
        _socket         = 0;
        m_currentSocket = 0;
        return;
    }

    if (m_complete)
    {
        // parse our data
        KXmlRpcParser parser(m_incomingXml, needAuth());

        // make sure this is valid!
        if (parser.valid() == false)
        {
            reply("Invalid XML in request");
            return;
        }

        // since this is a valid request, let's "forgive" any (if any)
        // bad retries from this IP
        unsigned long ip = _socket->ipv4_addr();
        if (m_retryMap.contains(ip))
            m_retryMap.remove(ip);

        // now let some child object process this!
        dispatch(m_server, parser.object(), parser.prototype(),
                 parser.params(), parser.auth());
    }
}

void KXmlRpcServer::outgoingData(KSocket *_socket)
{
    // try to write out all of our data at once.. if we can
    unsigned int bytes;
    bytes = ::write(_socket->socket(), m_outgoingXml.ascii(), m_outgoingXml.length());

    // if we wrote out all that we have, we are done!
    if (bytes == m_outgoingXml.length())
    {
        // disable write
        _socket->enableWrite(false);

        // if we aren't keeping alive, delete (and close) our socket?
        if (m_keepalive == false)
        {
            delete _socket;
            _socket         = 0;
            m_currentSocket = 0;
        }

        // reset some stuff
        m_incomingXml = "";
        m_outgoingXml = "";
        m_server      = "";
        m_keepalive   = false;
        m_complete    = false;
        return;
    }

    // okay, we still need to write out more later
    m_outgoingXml = m_outgoingXml.mid(bytes);
}

void KXmlRpcServer::reply()
{
    sendReply("", "");
}

void KXmlRpcServer::reply(const QString& _return)
{
    sendReply("string", _return);
}

void KXmlRpcServer::reply(int _return)
{
    sendReply("int", QString().setNum(_return));
}

void KXmlRpcServer::reply(double _return)
{
    sendReply("double", QString().setNum(_return));
}

void KXmlRpcServer::reply(const QByteArray& _return)
{
    QString encoded;
    KXmlRpcUtil::encodeBase64(_return, encoded);
    sendReply("base64", encoded);
}

void KXmlRpcServer::reply(const QDateTime& _return)
{
    QString encoded;
    KXmlRpcUtil::encodeISO8601(_return, encoded);
    sendReply("dateTime.iso8601", encoded);
}

void KXmlRpcServer::reply(const QValueList<int>& _return)
{
    QString str("<data>");
    QValueList<int>::ConstIterator it;
    for (it = _return.begin(); it != _return.end(); ++it)
    {
        str += "<value><int>" + QString().setNum(*it) + "</int></value>";
    }
    str += "</data>";
    sendReply("array", str);
}

void KXmlRpcServer::reply(const QValueList<double>& _return)
{
    QString str("<data>");
    QValueList<double>::ConstIterator it;
    for (it = _return.begin(); it != _return.end(); ++it)
        str += "<value><double>" + QString().setNum(*it) + "</double></value>";
    str += "</data>";
    sendReply("array", str);

}

void KXmlRpcServer::reply(const QValueList<QString>& _return)
{
    QString str("<data>");
    QValueList<QString>::ConstIterator it;
    for (it = _return.begin(); it != _return.end(); ++it)
        str += "<value><string>" + *it + "</string></value>";
    str += "</data>";
    sendReply("array", str);

}

void KXmlRpcServer::reply(const QValueList<QByteArray>& _return)
{
    QString str("<data>");
    QValueList<QByteArray>::ConstIterator it;
    for (it = _return.begin(); it != _return.end(); ++it)
    {
        QString encoded;
        KXmlRpcUtil::encodeBase64(*it, encoded);
        str += "<value><base64>" + encoded + "</base64></value>";
    }
    str += "</data>";
    sendReply("array", str);
}

void KXmlRpcServer::reply(const QValueList<QDateTime>& _return)
{
    QString str("<data>");
    QValueList<QDateTime>::ConstIterator it;
    for (it = _return.begin(); it != _return.end(); ++it)
    {
        QString encoded;
        KXmlRpcUtil::encodeISO8601(*it, encoded);
        str += "<value><dateTime.iso8601>" + encoded + "</dateTime.iso8601></value>";
    }
    str += "</data>";
    sendReply("array", str);
}

void KXmlRpcServer::reply(const QMap<QString, int>& _return)
{
    QString str("");
    QMap<QString,int>::ConstIterator it;
    for (it = _return.begin(); it != _return.end(); ++it)
    {
        str += "<member>";
        str += "<name>" + it.key() + "</name>";
        str += "<value><int>" + QString().setNum(it.data()) + "</int></value>";
        str += "</member>";
    }
    sendReply("struct", str);
}

void KXmlRpcServer::reply(const QMap<QString, double>& _return)
{
    QString str("");
    QMap<QString,double>::ConstIterator it;
    for (it = _return.begin(); it != _return.end(); ++it)
    {
        str += "<member>";
        str += "<name>" + it.key() + "</name>";
        str += "<value><double>" + QString().setNum(it.data()) + "</double></value>";
        str += "</member>";
    }
    sendReply("struct", str);
}

void KXmlRpcServer::reply(const QMap<QString, QString>& _return)
{
    QString str("");
    QMap<QString,QString>::ConstIterator it;
    for (it = _return.begin(); it != _return.end(); ++it)
    {
        str += "<member>";
        str += "<name>" + it.key() + "</name>";
        str += "<value><string>" + it.data() + "</string></value>";
        str += "</member>";
    }
    sendReply("struct", str);

}

void KXmlRpcServer::reply(const QMap<QString, QByteArray>& _return)
{
    QString str("");
    QMap<QString,QByteArray>::ConstIterator it;
    for (it = _return.begin(); it != _return.end(); ++it)
    {
        QString encoded;
        KXmlRpcUtil::encodeBase64(it.data(), encoded);
        str += "<member>";
        str += "<name>" + it.key() + "</name>";
        str += "<value><base64>" + encoded + "</base64></value>";
        str += "</member>";
    }
    sendReply("struct", str);
}

void KXmlRpcServer::reply(const QMap<QString, QDateTime>& _return)
{
    QString str("");
    QMap<QString,QDateTime>::ConstIterator it;
    for (it = _return.begin(); it != _return.end(); ++it)
    {
        QString encoded;
        KXmlRpcUtil::encodeISO8601(it.data(), encoded);
        str += "<member>";
        str += "<name>" + it.key() + "</name>";
        str += "<value><dateTime.iso8601>" + encoded + "</dateTime.iso8601></value>";
        str += "</member>";
    }
    sendReply("struct", str);
}

void KXmlRpcServer::reply(const DCOPRef& _return)
{
    QString str("<member>");
    str += "<name>app</name>";
    str += "<value><string>" + _return.app() + "</string></value>";
    str += "</member>";

    str += "<member>";
    str += "<name>object</name>";
    str += "<value><string>" + _return.object() + "</string></value>";
    str += "<member>";

    str += "<member>";
    str += "<name>isNull</name>";
    str += "<value><boolean>" + _return.isNull() ? "1" : "0";
    str += "</boolean></value>";
    str += "<member>";

    sendReply("struct", str);
}
// *grumble*  stupid C++ has bool == int
void KXmlRpcServer::replyBool(bool _return)
{
    sendReply("boolean", _return ? "1" : "0");
}

void KXmlRpcServer::sendReply(const QString& _type, const QString& _value)
{
    // format a 'normal' reply.  if _type is null, then we don't have
    // any response (a 'void' return, if you will)
    m_outgoingXml  = "<?xml version=\"1.0\"?><methodResponse><params>";
    if (_type != "")
    {
        m_outgoingXml += "<param><value><" + _type + ">";
        m_outgoingXml += _value;
        m_outgoingXml += "</" + _type + "></value></param>";
    }
    m_outgoingXml += "</params></methodResponse>\r\n";

    // standard header
    QString header("HTTP/1.1 200 OK\r\n");
    if (m_keepalive)
        header += "Connection: Keep-Alive\r\n";
    else
        header += "Connection: close\r\n";
    header += "Content-Type: text/xml\r\n";
    header += "Content-Length: " + QString().setNum(m_outgoingXml.length());
    header += "\r\n\r\n";
    m_outgoingXml = header + m_outgoingXml;

    // okay, we are now all set to write back the response
    m_currentSocket->enableWrite(true);
}

void KXmlRpcServer::replyError(const QString& _msg, int _code)
{
    // format an error (fault)
    m_outgoingXml  = "<?xml version=\"1.0\"?><methodResponse><fault>";
    m_outgoingXml += "<value><struct><member><name>faultCode</name>";
    m_outgoingXml += "<value><int>" + QString().setNum(_code);
    m_outgoingXml += "</int></value></member>";
    m_outgoingXml += "<member><name>faultString</name>";
    m_outgoingXml += "<value><string>" + _msg;
    m_outgoingXml += "</string></value></member>";
    m_outgoingXml += "</struct></value></fault>";
    m_outgoingXml += "</methodResponse>\r\n";

    // standard header
    QString header("HTTP/1.1 200 OK\r\n");
    if (m_keepalive)
        header += "Connection: close\r\n";
    else
        header += "Connection: Keep-Alive\r\n";
    header += "Content-Type: text/xml\r\n";
    header += "Content-Length: " + QString().setNum(m_outgoingXml.length());
    header += "\r\n\r\n";
    m_outgoingXml = header + m_outgoingXml;

    // okay, we are now all set to write back the response
    m_currentSocket->enableWrite(true);
}

void KXmlRpcServer::shutdown()
{
    kapp->quit();
}

bool KXmlRpcServer::addData(const QString& _data)
{
    static int size = 0;
    // this is a VERY ugly function/method designed to parse
    // the incoming request.  this definitely needs to be
    // rewritten to be more robust one of these days

    // save a copy of our incoming data
    m_incomingXml += _data;

    // let's not accept anything bigger then (ARBITRARY) 16K
    if (m_incomingXml.length() >= 16384)
    {
        m_incomingXml = "";

        // let's also assume that this is an attack
        updateAttack(m_currentSocket);

        return false;
    }


    int index;
    // size is the size of our CONTENT, not the header.  that means
    // that if it is 0, then we are still in the header.  right now,
    // we want to see if we have two <CR><LF> in a row.  that, of
    // course, means that we are done with the header
    if ((size == 0) && ((index = m_incomingXml.find("\r\n\r\n")) != -1))
    {
        // now that we have our entire header, we need to search for
        // some specific stuff

        // specifically, we need search for the server.  this will be
        // in the very first line like so:
        //   POST /server HTTP/1.1
        // so we skip the 'POST ' and grab everything until the next
        // space (ugly ugly ugly)
        if (m_incomingXml.left(5).upper() == "POST ")
            m_server = m_incomingXml.mid(6, m_incomingXml.find(' ', 6) - 6); 

        // next, we need the content-length.  this is very important
        // as the actual size of the content MUST match this.
        int cl_index = 0;
        if ((cl_index = m_incomingXml.lower().find("content-length: ", 0, false)) == -1)
        {
            // if we can't find it, then something really nasty has
            // just happened.  we need to bug out now
            kdDebug() << "No Content-length header found!" << endl;
            m_incomingXml = "";
            return false;
        }

        // okay, if you thought the above code was ugly.. check THIS
        // out!  it grabs all of the data past the Content-length: up
        // until the first <CR>.  this is our content length
        QString length = m_incomingXml.mid(cl_index+16,
                                     m_incomingXml.find('\r', cl_index)-cl_index-16);
        size = length.toInt();

        // see if we want keepalive or not
        int ka_index = 0;
        if ((ka_index = m_incomingXml.lower().find("connection: ", 0, false)) > -1)
        {
            QString ka = m_incomingXml.mid(ka_index + 12, 10);
            if (ka.lower() == "keep-alive")
                m_keepalive = true;
        }

        // FINALLY, we have what we need so we will chop off (discard)
        // the rest of the header.
        m_incomingXml = m_incomingXml.mid(index+4);
    }

    // now if our size is > 0, then we know that we have content
    if (size > 0)
    {
        // and this is the ugliest code yet.  did you know that the
        // content-length does NOT seem to include <CR> but may
        // include <LF>?  yuck!  calculate our length as if we didn't
        // have any <CR>s
        int adj_len = m_incomingXml.length() - m_incomingXml.contains('\r');

        // now see if that matches our size.  if it does, then we are
        // done receiving data
        if (adj_len >= size)
        {
            size       = 0;
            m_complete = true;
        }
    }

    return true;
}

void KXmlRpcServer::socketTimeout()
{
    // okay, if we're called, then the socket has timed out

    // first, we make sure that the sender (a qtimer) is valid AND
    // that it's parent is really a KSocket
    const QObject *obj = sender();
    if (obj && obj->inherits("QTimer") && obj->parent() &&
        obj->parent()->inherits("KSocket"))
    {
        KSocket *socket = static_cast<KSocket*>(obj->parent());

        // before we delete it, we want to keep a record of the
        // incoming party.  if this times out, then it's possible that
        // it's a DoS attack
        updateAttack(socket);

        delete socket; socket = 0;
    }

    // random note: this does solve the "kxmlrpcd taking 99% of the
    // CPU" problem.. but doesn't seem to give up the memory that
    // was created by the 'new KSocket'.  Dunno where the leak is,
    // though
}

void KXmlRpcServer::updateAttack(KSocket *_socket)
{
    unsigned long addr(_socket->ipv4_addr());
    if (m_retryMap.contains(addr))
    {
        unsigned int retries(m_retryMap[addr]);
        m_retryMap[addr] = ++retries;
    }
    else
        m_retryMap.insert(addr, 1);
}

#include "kxmlrpcserver.moc"
