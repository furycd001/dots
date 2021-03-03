/* This file is part of the KDE kdebase package
 *
 * Copyright (C) 1999-2000 Kurt Granroth <granroth@kde.org>
 *
 * This file is distributed under the BSD license. See the file "BSD"
 * in the subdirectory "licenses" of the package for the full license
 * text which has to be applied for this file.
 */
#include <kxmlrpcutil.h>
#include <kdebug.h>

#include <ctype.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

static QString generatePseudoAuthToken()
{
    // seed our random number generator with the current time
    struct timeval  tv;
    struct timezone tz;
    gettimeofday(&tv, &tz);
    srand((int)(tv.tv_sec + (tv.tv_usec << 16)));

    // now generate a 16 byte string with characters between ASCII 48
    // and 126 (0 to ~)
    QCString auth(17);
    for (int i = 0; i < 16; i++)
    {
        auth[i] = (char)((rand() % (126 - 48)) + 48);
        if ((auth[i] == '>') || (auth[i] == '<'))
            auth[i] = 'A';
    }
    auth[16] = '\0';

    return QString(auth);
}

QString KXmlRpcUtil::generateAuthToken()
{
    // we'll try to get a random 16 byte string with characters
    // between ASCII 48 and 126 (0 to ~).  There are two ways of doing
    // this.  If /dev/urandom exists, then it is used as it is quite
    // random (not nearly as much as /dev/random... but it doesn't
    // block, either).  If that device doesn't exist, then we go with
    // the standard rand() function.  WARNING!  If an attacker knows
    // when the program started, then they *might* be able to also
    // deduce the token if the rand() function is used
    FILE *rand;

    // try to open /dev/random
    if ((rand = fopen("/dev/urandom", "r")) == NULL)
        return generatePseudoAuthToken();

    // get 16 bytes of random numbers.  if it doesn't work for some
    // reason, then just return the pseudo random version
    unsigned char buffer[16];
    if (::fread(&buffer, 1, 16, rand) != 16)
    {
        fclose(rand);
        return generatePseudoAuthToken();
    }
    fclose(rand);

    // redo this data so that it's within our tolerances
    QCString auth(17);
    for (int i = 0; i < 16; i++)
    {
        auth[i] = (char)((buffer[i] % (126 - 48)) + 48);
        if ((auth[i] == '>') || (auth[i] == '<'))
            auth[i] = 'A';
    }
    auth[16] = '\0';

    return QString(auth);
}

const char *B64 =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

bool KXmlRpcUtil::encodeBase64(const QByteArray& _array, QString& _string)
{
    unsigned long srcl = _array.size();
    unsigned long destl = 0;

    const unsigned char *s = (unsigned char *)_array.data();
    unsigned long i = ((srcl + 2) / 3) * 4;
    destl = i += 2 * ((i / 60) + 1);
    char *ret = new char[destl];
    unsigned char *d((unsigned char *)ret);

    for (i = 0; srcl; s += 3)
    {
        *d++ =        B64[s[0] >> 2];
        *d++ =        B64[((s[0] << 4) + (--srcl ? s[1] >> 4 : 0)) & 0x3f];
        *d++ = srcl ? B64[((s[1] << 2) + (--srcl ? s[2] >> 6 : 0)) & 0x3f] : '=';
        *d++ = srcl ? B64[s[2] & 0x3f] : '=';

        if (srcl != 0)
            srcl--;

        if (++i == 15)
        {
            i = 0;
            *d++ = '\r';
            *d++ = '\n';
        }
    }

    *d++    = '\r';
    *d++    = '\n';
    *d      = '\0';

    _string = ret;

    delete [] ret; ret = 0;

    return true;
}

bool KXmlRpcUtil::decodeBase64(const QString& _string, QByteArray& _array)
{
    char c;
    int e(0);

    unsigned long len = 0;

    unsigned char *src = (unsigned char*) _string.local8Bit().data();

    unsigned long srcl = _string.length();

    char *ret = new char[srcl + (srcl / 4 + 1)];

    char *d((char *)ret);

    while (srcl--)
    {
        c = *src++;

        if      (isupper(c))  c -= 'A';
        else if (islower(c))  c -= 'a' - 26;
        else if (isdigit(c))  c -= '0' - 52;
        else if (c == '+')    c = 62;
        else if (c == '/')    c = 63;

        else if (c == '=')
        {
            switch (e++)
            {
                case 3:        e = 0;              break;
                case 2:        if (*src == '=')    break;
                default:                           return false;
            }

            continue;
        }

        else continue;

        switch (e++)
        {
            case 0:    *d = c << 2;                    break;
            case 1:    *d++ |= c >> 4; *d = c << 4;    break;
            case 2:    *d++ |= c >> 2; *d = c << 6;    break;
            case 3:    *d++ |= c; e = 0;               break;
        }
    }

    len = d - (char *)ret;

    _array.duplicate(ret, len);

    delete [] ret; ret = 0;

    return true;
}

bool KXmlRpcUtil::encodeISO8601(const QDateTime& _date, QString& _string)
{
    // we don't care if this date is valid
    int year  = _date.date().year();
    int month = _date.date().month();
    int day   = _date.date().day();

    int hour = _date.time().hour();
    int min  = _date.time().minute();
    int sec  = _date.time().second();

    // format this as YYYYMMDDTHH:MM:SS
    _string.sprintf("%04d%02d%02dT%02d:%02d:%02d",
                    year, month, day, hour, min, sec);
    return true;
}

bool KXmlRpcUtil::decodeISO8601(const QString& _string, QDateTime& _date)
{
    // Unfortunately, ISO8601 allows *lots* of different formats for
    // the date/time.  For now, I will only support ONE (more probably
    // will come later): YYYYMMDDTHH:MM:SS (e.g., 19980717T14:08:55)

    // two quick sanity checks: the 'T' must be in position 8 (9th
    // char) and the entire thing must be 17 chars long
    if ((_string.length() != 17) || (_string[8] != 'T'))
    {
        kdDebug() << _string << " is an invalid iso8601 date/time" << endl;
        return false;
    }

    // first, parse out the date
    QString year  = _string.left(4);
    QString month = _string.mid(4, 2);
    QString day   = _string.mid(6, 2);
    QDate date(year.toInt(), month.toInt(), day.toInt());

    if (date.isValid() == false)
    {
        kdDebug() << _string << " has an invalid iso8601 date" << endl;
        return false;
    }

    // next, grok our time
    QString hour = _string.mid(9, 2);
    QString min  = _string.mid(12, 2);
    QString sec  = _string.mid(15, 2);
    QTime time(hour.toInt(), min.toInt(), sec.toInt());

    if (time.isValid() == false)
    {
        kdDebug() << _string << " has an invalid iso8601 time" << endl;
        return false;
    }

    // whew, they both look fine.  now cat 'em together
    _date = QDateTime(date, time);
    return true;
}

// The copyright notice below refers to the original base 64 code.
// Some modifications are Copyright (C) 1998, 1999 Rik Hemsley rik@kde.org
/*
 * Original version Copyright 1988 by The Leland Stanford Junior University
 * Copyright 1998 by the University of Washington
 *
 *  Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notices appear in all copies and that both the
 * above copyright notices and this permission notice appear in supporting
 * documentation, and that the name of the University of Washington or The
 * Leland Stanford Junior University not be used in advertising or publicity
 * pertaining to distribution of the software without specific, written prior
 * permission.  This software is made available "as is", and
 * THE UNIVERSITY OF WASHINGTON AND THE LELAND STANFORD JUNIOR UNIVERSITY
 * DISCLAIM ALL WARRANTIES, EXPRESS OR IMPLIED, WITH REGARD TO THIS SOFTWARE,
 * INCLUDING WITHOUT LIMITATION ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE, AND IN NO EVENT SHALL THE UNIVERSITY OF
 * WASHINGTON OR THE LELAND STANFORD JUNIOR UNIVERSITY BE LIABLE FOR ANY
 * SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
 * CONTRACT, TORT (INCLUDING NEGLIGENCE) OR STRICT LIABILITY, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 */

