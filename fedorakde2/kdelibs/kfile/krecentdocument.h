/* -*- c++ -*-
 * Copyright (C)2000 Daniel M. Duley <mosfet@kde.org>
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */
#ifndef __KRECENTDOCUMENT_H
#define __KRECENTDOCUMENT_H

#include <qstring.h>
#include <kurl.h>

/**
 * Manage the "Recent Document Menu" entries displayed by
 * applications such as Kicker and Konqueror.
 *
 * These entries are automatically generated .desktop files pointing
 * to the current application and document.  You should call the
 * static @ref add() method whenever the user opens or saves a new
 * document if you want it to show up in the menu.
 *
 * You don't have to worry about this if you are using any @ref
 * KFileBaseDialog derived class to open and save documents, as it
 * already calls this class.  User defined limits on the maximum
 * number of documents to save, etc... are all automatically handled.
 *
 * @author Daniel M. Duley <mosfet@kde.org> 
 */
class KRecentDocument
{
public:

    /**
     *
     * Return a list of absolute paths to recent document .desktop files,
     * sorted by date.
     *
     */
    static QStringList recentDocuments();
    
    /**
     * Add a new item to the Recent Document menu.
     *
     * @param url The url to add.
     */
    static void add(const KURL& url);

    /**
     *
     * Add a new item to the Recent Document menu. Calls add( url ).
     *
     * @param documentStr The full path to the document or URL to add.
     *
     * @param Set to @p true if @p documentStr is an URL and not a local file path.
     */
    static void add(const QString &documentStr, bool isURL = false);

    /**
     * Clear the recent document menu of all entries.
     */
    static void clear();

    /**
     * Retrieve the maximum amount of recent document entries allowed.
     */
    static int maximumItems();

    /**
     * Returns the path to the directory where recent document .desktop files
     * are stored.
     */
    static QString recentDocumentDirectory();
};

#endif
