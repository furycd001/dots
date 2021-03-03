/* This file is part of the KDE libraries
   Copyright (C) 2000 David Faure <faure@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/
#ifndef __kfilterdev_h
#define __kfilterdev_h

#include <qiodevice.h>
#include <qstring.h>

class QFile;
class KFilterBase;

/**
 * A class for reading and writing compressed data onto a device
 * (e.g. file, but other usages are possible, like a buffer or a socket)
 *
 * To simply read/write compressed files, @see deviceForFile.
 *
 * @author David Faure <faure@kde.org>
 */
class KFilterDev : public QIODevice
{
public:
    /**
     * Create a KFilterDev for a given filter (e.g. gzip, bzip2 etc.)
     */
    KFilterDev( KFilterBase * filter ); // BCI remove and add default value to 2nd constructor
    /**
     * Create a KFilterDev for a given filter (e.g. gzip, bzip2 etc.)
     * Call this with autodeleteFilterBase so that the KFilterDev owns the KFilterBase.
     */
    KFilterDev( KFilterBase * filter, bool autodeleteFilterBase );

    virtual ~KFilterDev();

    virtual bool open( int mode );
    virtual void close();
    virtual void flush();

    /**
     * For writing gzip compressed files only:
     * set the name of the original file, to be used in the gzip header.
     */
    void setOrigFileName( const QCString & fileName );

    // Not implemented
    virtual uint size() const;

    virtual int  at() const;
    /**
     * That one can be quite slow, when going back. Use with care.
     */
    virtual bool at( int );

    virtual bool atEnd() const;

    virtual int readBlock( char *data, uint maxlen );
    virtual int writeBlock( const char *data, uint len );
    //int readLine( char *data, uint maxlen );

    virtual int getch();
    virtual int putch( int );
    virtual int ungetch( int );

    /**
     * Call this to create the appropriate filter device for @p base
     * working on @p file . The returned QIODevice has to be deleted
     * after using.
     * @deprecated. Use @ref deviceForFile instead.
     */
    static QIODevice* createFilterDevice(KFilterBase* base, QFile* file);


    /**
     * Return an i/o device that is able to read from @p fileName,
     * whether it's compressed or not. Available compression filters
     * (gzip/bzip2 etc.) will automatically be used.
     *
     * The compression filter to be used is determined from the @p fileName
     * if @p mimetype is empty. Pass "application/x-gzip" or "application/x-bzip2"
     * to force the corresponding decompression filter, if available.
     *
     * Warning: application/x-bzip2 may not be available.
     * In that case a QFile opened on the compressed data will be returned !
     * Use KFilterBase::findFilterByMimeType and code similar to what
     * deviceForFile is doing, to better control what's happening.
     *
     * The returned QIODevice has to be deleted after using.
     */
    static QIODevice * deviceForFile( const QString & fileName, const QString & mimetype = QString::null,
                                      bool forceFilter = false );

private:
    KFilterBase *filter;
    class KFilterDevPrivate;
    KFilterDevPrivate * d;
};


#endif
