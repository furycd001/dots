/***************************************************************************
                          document.h -  description
                             -------------------
    begin                : Mon Jan 15 2001
    copyright            : (C) 2001 by Christoph "Crossfire" Cullmann
    email                : crossfire@babylon2k.de
 ***************************************************************************/

/***************************************************************************
    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
 ***************************************************************************/

#ifndef _KATE_DOCUMENT_INCLUDE_
#define _KATE_DOCUMENT_INCLUDE_

#include <ktexteditor.h>

class KConfig;

namespace Kate
{

/** internal class for document bookmarks. */
class Mark
{
  public:
    uint line;
    uint type;
};

/** This interface provedes access to the Kate Document class.
*/
class Document : public KTextEditor::Document
{
  Q_OBJECT

  public:
    Document ();
    virtual ~Document ();

  public:
    /** Read document config.
    */
    virtual void readConfig () { ; };
    /** Save document config.
    */
    virtual void writeConfig () { ; };

    /** Read document session config.
    */
    virtual void readSessionConfig (KConfig *) { ; };
    /** Save document session config.
    */
    virtual void writeSessionConfig (KConfig *) { ; };

    /** Returns the document ID.
    */
    virtual uint docID () { return 0L; };

    /** Defines possible mark types. A line can have marks of different types.
    */
    enum marks
    {
    Bookmark = 1,
    Breakpoint = 2,
    markType0 = 4,
    markType1 = 8,
    markType2 = 16,
    markType3 = 32,
    markType4 = 64,
    markType5 = 128,
    markType6 = 256,
    markType7 = 512,
    markType8 = 1024
    };

    /** A list of all marks in a document. Use binary comparing to find marks of a specific type.
    */
    virtual QList<Mark> marks () { QList<Mark> l; return l; };

  public slots:
    // clear buffer/filename - update the views
    virtual void flush () { ; };
};

};

#endif
