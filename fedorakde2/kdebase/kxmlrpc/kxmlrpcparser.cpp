/* This file is part of the KDE kdebase package
 *
 * Copyright (C) 1999-2000 Kurt Granroth <granroth@kde.org>
 *
 * This file is distributed under the BSD license. See the file "BSD"
 * in the subdirectory "licenses" of the package for the full license
 * text which has to be applied for this file.
 */
#include <kxmlrpcparser.h>
#include <kxmlrpcutil.h>
#include <kdebug.h>

#include <qdom.h>
#include <qdatastream.h>
#include <qbitarray.h>
#include <qmap.h>

#include <stdio.h>

template QDataStream&operator<< <QString,QArray<char> >(QDataStream&,const QMap<QString,QArray<char> >&);
template QDataStream&operator<< <QString,QDateTime>(QDataStream&,const QMap<QString,QDateTime>&);
template QDataStream&operator<< <QString,QString>(QDataStream&,const QMap<QString,QString>&);
template QDataStream&operator<< <QString,double>(QDataStream&,const QMap<QString,double>&);
template QDataStream&operator<< <QString,int>(QDataStream&,const QMap<QString,int>&);

KXmlRpcParser::KXmlRpcParser(const QString& _xml, bool _auth)
  : m_intArray(0),
    m_doubleArray(0),
    m_stringArray(0),
    m_byteArray(0),
    m_dateArray(0),
    m_intMap(0),
    m_doubleMap(0),
    m_stringMap(0),
    m_byteMap(0),
    m_dateMap(0),
    m_auth(""),
    m_needAuth(_auth)
{
    m_xml = _xml;

    setValid(true);

    // make sure, first, that this is XML
    if (m_xml.left(5).lower() != "<?xml")
    {
        kdDebug() << "Not an xml document.  Aborting" << endl;
        setValid(false);
        return;
    }

    // now strip off the xml tag as DOM doesn't like it
    m_xml.remove(0, m_xml.find("?>") + 2);

    // okay, slap on the DOM stuff
    m_xml.prepend("<!DOCTYPE XMLRPC><XMLRPC>");
    m_xml.append("</XMLRPC>");

    // whew, we *should* have a valid DOM, now
    QDomDocument doc;
    doc.setContent(m_xml);

    // okay, now we should have <XMLRPC><methodCall>
    QDomElement elem = doc.documentElement().toElement();
    if (elem.tagName() != "XMLRPC")
    {
        kdDebug() << "Had serious XML parsing problems.  Aborting" << endl;
        setValid(false);
        return;
    }
    elem = elem.firstChild().toElement();
    if (elem.tagName().lower() != "methodcall")
    {
        kdDebug() << "Couldn't find <methodCall>.  Aborting" << endl;
        setValid(false);
        return;
    }

    // now we grab the methodName
    elem = elem.firstChild().toElement();
    if (elem.tagName().lower() != "methodname")
    {
        kdDebug() << "Couldn't find <methodName>.  Aborting" << endl;
        setValid(false);
        return;
    }
    m_prototype = elem.text();

    // and strip out the object if it's there
    int the_dot;
    if ((the_dot = m_prototype.find('.')) > -1)
    {
        m_object = m_prototype.left(the_dot);
        m_prototype = m_prototype.mid(the_dot + 1);
    }

    // cool.. now we are ready to parse the <params>
    QDataStream data_stream(m_params, IO_WriteOnly);
    QDomElement params = elem.nextSibling().toElement();
    m_prototype += "(";
    parseXmlParams(params, data_stream);

    // strip off the last comma, if it is there
    if (m_prototype[m_prototype.length()-1] == ',')
        m_prototype.truncate(m_prototype.length()-1);
    m_prototype += ")";
}

KXmlRpcParser::~KXmlRpcParser()
{
    delete m_intArray;
    delete m_doubleArray;
    delete m_stringArray;
    delete m_byteArray;
    delete m_dateArray;
    delete m_intMap;
    delete m_doubleMap;
    delete m_stringMap;
    delete m_byteMap;
    delete m_dateMap;
}

QString KXmlRpcParser::prototype() const
{
    return m_prototype;
}

QString KXmlRpcParser::object() const
{
    return m_object;
}

QString KXmlRpcParser::auth() const
{
    return m_auth;
}

QByteArray KXmlRpcParser::params() const
{
    return m_params;
}

void KXmlRpcParser::parseXmlParams(QDomElement& _elem, QDataStream& _ds)
{
    // sanity check.  this must be <params>
    if (_elem.tagName().lower() != "params")
    {
        kdDebug() << "Couldn't find <params>.  Aborting" << endl;
        setValid(false);
        return;
    }

    // iterate through -- and parse -- all of the params
    QDomElement param = _elem.firstChild().toElement();
    for ( ; !param.isNull(); param = param.nextSibling().toElement())
    {
        parseXmlParam(param, _ds);
        if (valid() == false)
            return;
    }
}

void KXmlRpcParser::parseXmlParam(QDomElement& _elem, QDataStream& _ds)
{
    // sanity check.  this must be <param>
    if (_elem.tagName().lower() != "param")
    {
        kdDebug() << "Couldn't find <param>.  Aborting" << endl;
        setValid(false);
        return;
    }

    // now parse the <value>
    QDomElement value = _elem.firstChild().toElement();
    parseXmlValue(value, _ds);
}

void KXmlRpcParser::parseXmlValue(QDomElement& _elem, QDataStream& _ds)
{
    // sanity check.  this must be <value>
    if (_elem.tagName().lower() != "value")
    {
        kdDebug() << "Couldn't find <value>.  Aborting" << endl;
        setValid(false);
        return;
    }

    // now get the type of this value
    QDomElement type = _elem.firstChild().toElement();
    QString type_name(type.tagName().lower());

    // if it is null, then assume <string>
    if (type.isNull())
    {
        if (m_needAuth && (m_auth == ""))
            m_auth = QString(_elem.text());
        else
        {
            _ds << _elem.text();
            m_prototype += "QString,";
        }
    }

    else if (type_name == "string")
    {
        if (m_needAuth && (m_auth == ""))
            m_auth = type.text();
        else
        {
            _ds << type.text();
            m_prototype += "QString,";
        }
    }

    else if ((type_name == "i4") || (type_name == "int"))
    {
        m_prototype += "int,";
        _ds << (Q_INT32)type.text().toInt();
    }

    else if (type_name == "double")
    {
        m_prototype += "double,";
        _ds << type.text().toDouble();
    }

    else if (type_name == "boolean")
    {
        m_prototype += "bool,";
        if ((type.text().lower() == "true") || (type.text() == "1"))
            _ds << (unsigned int)true;
        else
            _ds << (unsigned int)false;
    }

    else if (type_name == "base64")
    {
        m_prototype += "QByteArray,";
        QByteArray base64;
        KXmlRpcUtil::decodeBase64(type.text(), base64);
        _ds << base64;
    }

    else if ((type_name == "datetime") ||
             (type_name == "datetime.iso8601"))
    {
        m_prototype += "QDateTime,";
        QDateTime date_time;
        KXmlRpcUtil::decodeISO8601(type.text(), date_time);
        _ds << date_time;
    }

    else if (type_name == "array")
    {
        QDomElement array = type;
        QString data_type(QString::null);
        parseXmlArray(array, _ds, data_type);
        m_prototype += "QValueList<" + data_type + ">,";
    }

    else if (type_name == "struct")
    {
        QDomElement stct = type;
        QString member_type(QString::null);
        parseXmlStruct(stct, _ds, member_type);
        m_prototype += "QMap<QString, " + member_type + ">,";
    }

    else
    {
        kdDebug() << "Don't know tag <" << type_name << ">" << endl;
        setValid(false);
    }
}

void KXmlRpcParser::parseXmlStruct(QDomElement& _elem, QDataStream& _ds,
                                   QString& _type)
{
    // sanity check.  this must be <struct>
    if (_elem.tagName().lower() != "struct")
    {
        kdDebug() << "Couldn't find expected <struct>" << endl;
        setValid(false);
        return;
    }

    // iterate through all of the members.  the type is passed along
    // as it must be the same for all members
    QDomElement member = _elem.firstChild().toElement();
    _type = "";
    for ( ; !member.isNull(); member = member.nextSibling().toElement())
    {
        parseXmlStructMember(member, _type);
        if (valid() == false)
            return;
    }

    // now store our map
    if ((_type == "int") || (_type == "i4"))
    {
        _type = "int";
        _ds << *m_intMap;
    }
    else if (_type == "string")
    {
        _type = "QString";
        _ds << *m_stringMap;
    }
    else if (_type == "double")
    {
        _ds << *m_doubleMap;
    }
    else if (_type == "base64")
    {
        _type = "QByteArray";
        _ds << *m_byteMap;
    }
    else if ((_type == "datetime") || (_type == "datetime.iso8601"))
    {
        _type = "QDateTime";
        _ds << *m_dateMap;
    }
}

void KXmlRpcParser::parseXmlStructMember(QDomElement& _elem, QString& _type)
{
    // sanity check.  this must be <member>
    if (_elem.tagName().lower() != "member")
    {
        kdDebug() << "Couldn't find expected <member>" << endl;
        setValid(false);
        return;
    }

    // each member has a <name> and a <value>
    QDomElement name_elem = _elem.firstChild().toElement();
    QString name = name_elem.text();
    if (name == QString::null)
    {
        name = name_elem.firstChild().toElement().text();
        if (name == QString::null)
            return;
    }
    QDomElement value = name_elem.nextSibling().toElement();

    // get our type
    QDomElement type_elem = value.firstChild().toElement();
    QString type(type_elem.tagName().lower());

    if (type == QString::null)
        type = "string";

    // save our type if we haven't already
    if (_type == QString::null);
        _type = type;

    // we can allow only ONE type per struct
    if (_type != type)
    {
        kdDebug() << "There can be only one type per struct!" << endl;
        setValid(false);
        return;
    }

    // okay, we know that our type is the same...
    if ((type == "int") || (type == "i4"))
    {
        if (!m_intMap) m_intMap = new QMap<QString, int>;
        m_intMap->insert(name, type_elem.text().toInt());
    }
    else if (type == "string")
    {
        if (!m_stringMap) m_stringMap = new QMap<QString, QString>;
        m_stringMap->insert(name, type_elem.text());
    }
    else if (type == "double")
    {
        if (!m_doubleMap) m_doubleMap = new QMap<QString, double>;
        m_doubleMap->insert(name, type_elem.text().toDouble());
    }
    else if (type == "base64")
    {
        if (!m_byteMap) m_byteMap = new QMap<QString, QByteArray>;

        QByteArray base64;
        KXmlRpcUtil::decodeBase64(type_elem.text(), base64);

        m_byteMap->insert(name, base64);
    }
    else if ((type == "datetime") || (type == "datetime.iso8601"))
    {
        if (!m_dateMap) m_dateMap = new QMap<QString, QDateTime>;

        QDateTime date_time;
        KXmlRpcUtil::decodeISO8601(type_elem.text(), date_time);

        m_dateMap->insert(name, date_time);
    }
    else
    {
        kdDebug() << "Unknown or invalid type: " << type << endl;
        setValid(false);
        return;
     }
}

void KXmlRpcParser::parseXmlArray(QDomElement& _elem, QDataStream& _ds,
                                  QString& _type)
{
    // sanity check.  this must be <array>
    if (_elem.tagName().lower() != "array")
    {
        kdDebug() << "Couldn't find expected <array>" << endl;
        setValid(false);
        return;
    }

    // we should have one element -- a <data> field
    QDomElement data = _elem.firstChild().toElement();
    parseXmlArrayData(data, _ds, _type);
}

void KXmlRpcParser::parseXmlArrayData(QDomElement& _elem, QDataStream& _ds,
                                      QString& _type)
{
    // sanity check.  this must be <data>
    if (_elem.tagName().lower() != "data")
    {
        kdDebug() << "Couldn't find expected <data>" << endl;
        setValid(false);
        return;
    }

    // iterate through all of the values.  the type is passed along
    // as it must be the same for all members
    QDomElement value = _elem.firstChild().toElement();
    _type = QString::null;
    for ( ; !value.isNull(); value = value.nextSibling().toElement())
        parseXmlArrayValue(value, _type);

    // now store our array
    if ((_type == "int") || (_type == "i4"))
    {
        _type = "int";
        _ds << *m_intArray;
    }
    else if (_type == "string")
    {
        _type = "QString";
        _ds << *m_stringArray;
    }
    else if (_type == "double")
    {
        _ds << *m_doubleArray;
    }
    else if (_type == "base64")
    {
        _type = "QByteArray";
        _ds << *m_byteArray;
    }
    else if ((_type == "datetime") || (_type == "datetime.iso8601"))
    {
        _type = "QDateTime";
        _ds << *m_dateArray;
    }

}

void KXmlRpcParser::parseXmlArrayValue(QDomElement& _elem, QString& _type)
{
    // sanity check.  this must be <value>
    if (_elem.tagName().lower() != "value")
    {
        kdDebug() << "Couldn't find <value>.  Aborting" << endl;
        setValid(false);
        return;
    }

    // now get the type of this value
    QDomElement type_elem = _elem.firstChild().toElement();
    QString type(type_elem.tagName().lower());

    // take care of the null case
    if (type == "")
        type = "string";

    // save our type if we haven't already
    if (_type == "");
        _type = type;

    // we can allow only ONE type per array
    if (_type != type)
    {
        kdDebug() << "There can be only one type per struct or array!" << endl;
        setValid(false);
        return;
    }

    // okay, we know that our type is the same...
    if ((type == "int") || (type == "i4"))
    {
        if (!m_intArray) m_intArray = new QValueList<int>;
        m_intArray->append(type_elem.text().toInt());
    }
    else if (type == "string")
    {
        if (!m_stringArray) m_stringArray = new QValueList<QString>;
        m_stringArray->append(type_elem.text());
    }
    else if (type == "double")
    {
        if (!m_doubleArray) m_doubleArray = new QValueList<double>;
        m_doubleArray->append(type_elem.text().toDouble());
    }
    else if (type == "base64")
    {
        if (!m_byteArray) m_byteArray = new QValueList<QByteArray>;

        QByteArray base64;
        KXmlRpcUtil::decodeBase64(type_elem.text(), base64);

        m_byteArray->append(base64);
    }
    else if ((type == "datetime") || (type == "datetime.iso8601"))
    {
        if (!m_dateArray) m_dateArray = new QValueList<QDateTime>;

        QDateTime date_time;
        KXmlRpcUtil::decodeISO8601(type_elem.text(), date_time);

        m_dateArray->append(date_time);
    }
    else
    {
        kdDebug() << "Unknown or invalid type: " << type << endl;
        setValid(false);
        return;
     }

}

bool KXmlRpcParser::valid()
{
    return m_valid;
}

void KXmlRpcParser::setValid(bool _valid)
{
    m_valid = _valid;
}
