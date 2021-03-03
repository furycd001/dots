/* This file is part of the KDE libraries
   Copyright (C) 1999 Ian Zepp (icszepp@islc.net)
   Copyright (C) 2000 Rik Hemsley (rikkus) <rik@kde.org>

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
#ifndef KSTRINGHANDLER_H
#define KSTRINGHANDLER_H

#include <stdlib.h>        // for atoi()
#include <qstring.h>
#include <qstringlist.h>
#include <qregexp.h>            // for the word ranges


/** General description of this class:
 *
 * This class is @em not a substitute for the @ref QString class. What
 * I tried to do with this class is provide an easy way to
 * cut/slice/splice words inside sentences in whatever order desired.
 * While the main focus of this class are words (ie characters
 * separated by spaces/tabs), the two core functions here (@ref split()
 * and @ref join() ) will function given any char to use as a separator.
 * This will make it easy to redefine what a 'word' means in the
 * future if needed.
 *
 * I freely stole some of the function names from python. I also think
 * some of these were influenced by mIRC (yes, believe it if you will, I
 * used to write a LOT of scripts in mIRC).
 *
 * The ranges are a fairly powerful way of getting/stripping words from
 * a string. These ranges function, for the large part, as they would in
 * python. See the @ref word and @ref remword functions for more detail.
 *
 * This class contains no data members of it own. All strings are cut
 * on the fly and returned as new qstrings/qstringlists.
 *
 * Quick example on how to use:
 *
 * <pre>
 * KStringHandler kstr;
 * QString line = "This is a test of the strings";
 *
 * cout << "1> " << kstr.word( line , "4:" ) << "\n";
 * cout << "2> " << kstr.remrange( line , "2:5" ) << "\n";
 * cout << "2> " << kstr.reverse( line ) << "\n";
 * cout << "2> " << kstr.center( kstr.word( line , 4 ) , 15 ) << "\n";
 * </pre>
 *
 * and so forth.
 *
 * @short Class for manipulating words and sentences in strings
 * @author Ian Zepp <icszepp@islc.net>
 */
class KStringHandler
{
public:
    /** Returns the nth word in the string if found
      * Returns a EMPTY (not null) string otherwise.
      * Note that the FIRST index is 0
      */
    static QString        word( const QString &text , uint pos );

    /** Returns a range of words from that string.
      * Ie:
      *
      * "0"        returns the very first word
      * "0:"    returns the first the the last word
      * "0:3"    returns the first to fourth words
      * ":3"    returns everything up to the fourth word
      *
      * If you grok python, you're set.
      */
    static QString        word( const QString &text , const char *range );

    /** Inserts a word into the string, and returns
      * a new string with the word included. the first
      * index is zero (0)
      */
    static QString        insword( const QString &text , const QString &word , uint pos );

    /** Replaces a word in the string, and returns
      * a new string with the word included. the first
      * index is zero (0)
      */
    static QString        setword( const QString &text , const QString &word , uint pos );

    /** Removes a word or ranges of words from the string,
      * and returns a new string. The ranges definitions
      * follow the definitions for the word() function.
      *
      * "0"        removes the very first word
      * "0:"    removes the first the the last word
      * "0:3"    removes the first to fourth words
      * ":3"    removes everything up to the fourth word
      */
    static QString        remrange( const QString &text , const char *range );


    /** Removes a word at the given index, and returns a
      * new string. The first index is zero (0)
      */
    static QString        remword( const QString &text , uint pos );

    /** Removes a matching word from the string, and returns
      * a new string. Note that only ONE match is removed.
      */
    static QString        remword( const QString &text , const QString &word );

    /** Capitalizes each word in the string
      * "hello there" becomes "Hello There"        (string)
      */
    static QString        capwords( const QString &text );

    /** Capitalizes each word in the list
      * [hello, there] becomes [Hello, There]    (list)
      */
    static QStringList    capwords( const QStringList &list );

    /** Reverses the order of the words in a string
      * "hello there" becomes "there hello"        (string)
      */
    static QString        reverse( const QString &text );

    /** Reverses the order of the words in a list
      * [hello, there] becomes [there, hello]    (list)
      */
    static QStringList    reverse( const QStringList &list );

    /** These string justify a string along x, and returns a
      * string at least 'width' characters wide, and justified.
      * If the string is longer than the width, the original
      * string is returned. It is never truncated.
      * ljust    Left justify
      * rjust    Right justify
      * center    Centers
      */
    static QString        ljust( const QString &text , uint width );
    static QString        rjust( const QString &text , uint width );
    static QString        center( const QString &text , uint width );

    /** Substitute characters at the beginning of a string by "...".
     * @return the modified string
     * @param str is the string to modify
     * @param maxlen is the maximum length the modified string will have
     * If the original string is shorter than "maxlen", it is returned verbatim
     */
    static QString        lsqueeze( const QString & str, uint maxlen = 40 );

    /** Substitute characters at the middle of a string by "...".
     * @return the modified string
     * @param str is the string to modify
     * @param maxlen is the maximum length the modified string will have
     * If the original string is shorter than "maxlen", it is returned verbatim
     */
    static QString        csqueeze( const QString & str, uint maxlen = 40 );

    /** Substitute characters at the end of a string by "...".
     * @return the modified string
     * @param str is the string to modify
     * @param maxlen is the maximum length the modified string will have
     * If the original string is shorter than "maxlen", it is returned verbatim
     */
    static QString        rsqueeze( const QString & str, uint maxlen = 40 );

    /**
     * @return true if the given filename matches the given pattern
     * @param filename is the real decoded filename (or dirname
     *        without trailing '/').
     * @param pattern is a pattern like *.txt, *.tar.gz, Makefile.*, etc.
     * Patterns with two asterisks like "*.*pk" are not supported.
     */
    static bool matchFilename( const QString& filename, const QString& pattern );
      
    /**
     * Split a QString into a QStringList in a similar fashion to the static
     * QStringList function in Qt, except you can specify a maximum number
     * of tokens. If max is specified (!= 0) then only that number of tokens
     * will be extracted. The final token will be the remainder of the string.
     *
     * Example:
     * perlSplit("__", "some__string__for__you__here", 4)
     * QStringList contains: "some", "string", "for", "you__here"
     *
     * @return A QStringList containing tokens extracted from s.
     * @param sep is the string to use to delimit s.
     * @param max is the maximum number of extractions to perform, or 0.
     */
    static QStringList perlSplit
      (const QString & sep, const QString & s, uint max = 0);

    /**
     * Split a QString into a QStringList in a similar fashion to the static
     * QStringList function in Qt, except you can specify a maximum number
     * of tokens. If max is specified (!= 0) then only that number of tokens
     * will be extracted. The final token will be the remainder of the string.
     *
     * Example:
     * perlSplit(' ', "kparts reaches the parts other parts can't", 3)
     * QStringList contains: "kparts", "reaches", "the pats other parts can't"
     *
     * @return A QStringList containing tokens extracted from s.
     * @param sep is the character to use to delimit s.
     * @param max is the maximum number of extractions to perform, or 0.
     */
    static QStringList perlSplit
      (const QChar & sep, const QString & s, uint max = 0);

    /**
     * Split a QString into a QStringList in a similar fashion to the static
     * QStringList function in Qt, except you can specify a maximum number
     * of tokens. If max is specified (!= 0) then only that number of tokens
     * will be extracted. The final token will be the remainder of the string.
     *
     * Example:
     * perlSplit(QRegExp("[! ]", "Split me up ! I'm bored ! OK ?", 3)
     * QStringList contains: "Split", "me", "up ! I'm bored, OK ?"
     *
     * @return A QStringList containing tokens extracted from s.
     * @param sep is the regular expression to use to delimit s.
     * @param max is the maximum number of extractions to perform, or 0.
     */
    static QStringList perlSplit
      (const QRegExp & sep, const QString & s, uint max = 0);
};
#endif
