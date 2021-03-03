/* This file is part of the KDE kdebase package
 *
 * Copyright (C) 1999-2000 Kurt Granroth <granroth@kde.org>
 *
 * This file is distributed under the BSD license. See the file "BSD"
 * in the subdirectory "licenses" of the package for the full license
 * text which has to be applied for this file.
 */
#ifndef XMLRPCPARSER_H
#define XMLRPCPARSER_H

class QDomElement;
class QDataStream;
class QDateTime;

#include <qcstring.h>
#include <qmap.h>
#include <qvaluelist.h>

/**
 * This is a basic XmlRpc parser.  Given an XmlRpc <methodCall>
 * structure, it will parse out the 'object', function prototype, and
 * all of the parameters as a Qt datastream.
 *
 * Say you have an XmlRpc request like so:
 * <code>
 * <methodCall>
 * <methodName>server.function</methodName>
 * <params>
 *    <param>
 *      <value><string>Two</string></value>
 *    </param>
 *    <param>
 *      <value><int>2</int></value>
 *    </param>
 * </params>
 * </methodCall>
 * </code>
 *
 * The parser would return 'server' as the object and
 * 'function(QString,int)' as the prototype.  It would then create a
 * datastream with a QString("Two") and int(2).
 *
 * @short Basic XmlRpc Parser
 * @author Kurt Granroth <granroth@kde.org>
 * @version 0.1
 */
class KXmlRpcParser
{
public:
    KXmlRpcParser(const QString& _xml, bool _auth = false);
    virtual ~KXmlRpcParser();

    QString object() const;
    QString prototype() const;
    QString auth() const;
    QByteArray params() const;

    bool valid();

protected:
    void parseXmlParams(QDomElement& _elem, QDataStream& _ds);
    void parseXmlParam(QDomElement& _elem, QDataStream& _ds);
    void parseXmlValue(QDomElement& _elem, QDataStream& _ds);
    void parseXmlStruct(QDomElement& _elem, QDataStream& _ds, QString& _type);
    void parseXmlStructMember(QDomElement& _elem, QString& _type);
    void parseXmlArray(QDomElement& _elem, QDataStream& _ds, QString& _type);
    void parseXmlArrayData(QDomElement& _elem, QDataStream& _ds,
                           QString& _type);
    void parseXmlArrayValue(QDomElement& _elem, QString& _type);


    void setValid(bool _valid = true);

private:
    QString   m_xml;
    QByteArray m_params;

    bool m_valid;

    QValueList<int>        *m_intArray;
    QValueList<double>     *m_doubleArray;
    QValueList<QString>    *m_stringArray;
    QValueList<QByteArray> *m_byteArray;
    QValueList<QDateTime>  *m_dateArray;

    QMap<QString, int>        *m_intMap;
    QMap<QString, double>     *m_doubleMap;
    QMap<QString, QString>    *m_stringMap;
    QMap<QString, QByteArray> *m_byteMap;
    QMap<QString, QDateTime>  *m_dateMap;

    QString m_prototype;
    QString m_object;
    QString m_auth;

    bool m_needAuth;
};

#endif // XMLRPCPARSER_H
