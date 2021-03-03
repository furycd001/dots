/***************************************************************************
                          view.h -  description
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

#ifndef _KATE_VIEW_INCLUDE_
#define _KATE_VIEW_INCLUDE_

#include <ktexteditor.h>

class KConfig;

namespace Kate
{

class Document;
class Mark;

/** This interface provides access to the view.
*/
class View : public KTextEditor::View
{
  Q_OBJECT

  public:
    View ( KTextEditor::Document *doc, QWidget *parent, const char *name = 0 );
    virtual ~View ();

    /** Returns a pointer to the document of the view.
    */
    virtual Document *getDoc () { return 0L; };

    /** Returns the marked text in the view.
    */
    virtual QString markedText () { return 0L; };

  public slots:
    /** popup a config dialog for the editor part.
    */
    virtual void configDialog () { ; };

    // Highlighting slots
    virtual void setHl (int) { ; };
    virtual int getHl () { return 0; };
    virtual int getHlCount () { return 0; };
    virtual QString getHlName (int) { return 0L; };
    virtual QString getHlSection (int) { return 0L; };

    // undo/redo stuff
    virtual void undo () { ; };
    virtual void redo () { ; };
    virtual void undoHistory() { ; };

  public:
    // read/save config of the view
    virtual void readConfig () { ; };
    virtual void writeConfig () { ; };

    // read/save sessionconfig of the view
    virtual void readSessionConfig (KConfig *) { ; };
    virtual void writeSessionConfig (KConfig *) { ; };

  public slots:
    // some simply key commands
    virtual void keyReturn () { ; };
    virtual void keyDelete () { ; };
    virtual void backspace () { ; };
    virtual void killLine () { ; };

    // move cursor in the view
    virtual void cursorLeft () { ; };
    virtual void shiftCursorLeft () { ; };
    virtual void cursorRight () { ; };
    virtual void shiftCursorRight () { ; };
    virtual void wordLeft () { ; };
    virtual void shiftWordLeft () { ; };
    virtual void wordRight () { ; };
    virtual void shiftWordRight () { ; };
    virtual void home () { ; };
    virtual void shiftHome () { ; };
    virtual void end () { ; };
    virtual void shiftEnd () { ; };
    virtual void up () { ; };
    virtual void shiftUp () { ; };
    virtual void down () { ; };
    virtual void shiftDown () { ; };
    virtual void scrollUp () { ; };
    virtual void scrollDown () { ; };
    virtual void topOfView () { ; };
    virtual void bottomOfView () { ; };
    virtual void pageUp () { ; };
    virtual void shiftPageUp () { ; };
    virtual void pageDown () { ; };
    virtual void shiftPageDown () { ; };
    virtual void top () { ; };
    virtual void shiftTop () { ; };
    virtual void bottom () { ; };
    virtual void shiftBottom () { ; };

  public slots:
    // edit command popup window
    virtual void slotEditCommand () { ; };

    // icon border enable/disable
    virtual void setIconBorder (bool) { ; };
    virtual void toggleIconBorder () { ; };

    // goto mark
    virtual void gotoMark (Mark *) { ; };

    // toggle current line bookmark or clear all bookmarks
    virtual void toggleBookmark () { ; };
    virtual void clearBookmarks () { ; };

  public:
    // is iconborder visible ?
    virtual bool iconBorder() { return false; };

  public slots:
     /**
      Flushes the document of the text widget. The user is given
      a chance to save the current document if the current document has
      been modified.
    */
    virtual void flush () { ; };

  public:
    /**
      Returns true if the current document can be
      discarded. If the document is modified, the user is asked if he wants
      to save it. On "cancel" the function returns false.
    */
    virtual bool canDiscard() { return false; };
};

};

#endif
