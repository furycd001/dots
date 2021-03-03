/* This file is part of the KDE kdebase package
 *
 * Copyright (C) 1999-2000 Kurt Granroth <granroth@kde.org>
 *
 * This file is distributed under the BSD license. See the file "BSD"
 * in the subdirectory "licenses" of the package for the full license
 * text which has to be applied for this file.
 */
#ifndef KXMLRPCUTIL_H
#define KXMLRPCUTIL_H

#include <qstring.h>
#include <qdatetime.h>

/**
 * This is a container class for utility functions.
 *
 * @short XmlRpc Utility Functions
 * @author Kurt Granroth <granroth@kde.org>
 * @version 0.1
 */
class KXmlRpcUtil
{
public:
    /**
     * Static method for encoding Base64.  This takes a QByteArray
     * array and encodes it into a string
     */
    static bool encodeBase64(const QByteArray& _array, QString& _string);

    /**
     * Static method for decoding Base64.  This takes a Base64-encoded
     * string and decodes it into a byte array
     */
    static bool decodeBase64(const QString& _string, QByteArray& _array);

    /**
     * Static method for encoding QDateTime into an iso8601 date
     */
    static bool encodeISO8601(const QDateTime& _date, QString& _string);

    /**
     * Static method for decoding iso8601 dates into QDateTime
     */
    static bool decodeISO8601(const QString& _string, QDateTime& _date);

    /**
     * Generate a 16 byte authorization token ("cookie")
     */
    static QString generateAuthToken();
};

#endif // KXMLRPCUTIL_H

