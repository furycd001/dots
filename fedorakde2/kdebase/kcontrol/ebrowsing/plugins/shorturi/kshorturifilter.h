/*
    kshorturifilter.h

    This file is part of the KDE project
    Copyright (C) 2000 Dawit Alemayehu <adawit@kde.org>
    Copyright (C) 2000 Malte Starostik <starosti@zedat.fu-berlin.de>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#ifndef _KSHORTURIFILTER_H_
#define _KSHORTURIFILTER_H_

#include <dcopobject.h>
#include <klibloader.h>
#include <kurifilter.h>

class KInstance;

/**
* This is short URL filter class.
*
*
*
*
* @short A filter that converts short URLs into fully qualified ones.
*
* @author Dawit Alemayehu <adawit@kde.org>
* @author Malte Starostik <starosti@zedat.fu-berlin.de>
*/
class KShortURIFilter : public KURIFilterPlugin , public DCOPObject
{
    K_DCOP
public:

    /**
     * Creates a Short URI filter object
     *
     * @param parent the parent of this class.
     * @param name the internal name for this object.
     */
    KShortURIFilter( QObject *parent = 0, const char *name = 0 );

    /**
     * Destructor
     */
    virtual ~KShortURIFilter() {};

    /**
     * Converts short URIs into fully qualified valid URIs
     * whenever possible.
     *
     * Parses any given invalid URI to determine whether it
     * is a known short URI and converts it to its fully
     * qualified version.
     *
     * @param data the data to be filtered
     * @return true if the url has been filtered
     */
    virtual bool filterURI( KURIFilterData &data ) const;

    /**
     * Returns the name of the config module for
     * this plugin.
     *
     * @return the name of the config module.
     */
    virtual QString configName() const;

    /**
     * Returns an instance of the module used to configure
     * this object.
         *
         * @return the config module
         */
    virtual KCModule* configModule( QWidget*, const char* ) const;

public:
k_dcop:
    virtual void configure();

protected:

    /**
     * Validates whether the string contians a possible
     * "short URI" signature.
     *
     * @param str url to be test for validity
     * @return true if the string is a possible short uri
     */
    bool isValidShortURL( const QString& /*str*/ ) const;

    /**
     * Expands any environment variables
     *
     * This functions expands any environment variables
     * it can find.  The results are the same as what
     * you would get when enviroment variables are expanded
     * in a *nix shell.
     *
     * @param string the url that contains variable to be expanded
     * @return true if an environment variable was sucessfully expanded
     */
    bool expandEnvVar( QString& ) const;

private:

    struct URLHint
    {
        URLHint() {}
        URLHint( QString r, QString p ) : regexp(r), prepend(p) {}
        QString regexp; // if this matches, then...
        QString prepend; // ...prepend this to the url
    };

    QValueList<URLHint> m_urlHints;
    QString m_strDefaultProtocol;
};


class KShortURIFilterFactory : public KLibFactory
{
    Q_OBJECT
public:
    KShortURIFilterFactory( QObject *parent = 0, const char *name = 0 );
    ~KShortURIFilterFactory();

    virtual QObject *create( QObject *parent = 0, const char *name = 0,
                             const char* classname = "QObject",
                             const QStringList &args = QStringList() );

    static KInstance *instance();

private:
    static KInstance *s_instance;

};
#endif
