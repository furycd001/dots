/***************************************************************************
                          katedocument.h  -  description
                             -------------------
    begin                : Mon Jan 15 2001
    copyright            : (C) 2001 by Christoph "Crossfire" Cullmann
    email                : crossfire@babylon2k.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

/*
   Copyright (C) 1998, 1999 Jochen Wilhelmy
                            digisnap@cs.tu-berlin.de

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
*/

#ifndef kate_document_h
#define kate_document_h

#include "../main/katemain.h"

#include <qobject.h>
#include <qlist.h>
#include <qcolor.h>
#include <qfont.h>
#include <qfontmetrics.h>
#include <qdatetime.h>

#include "../view/kateview.h"
#include "katehighlight.h"
#include "katebuffer.h"
#include "katetextline.h"


#include <qptrdict.h>

class KateCmd;

#include "../interfaces/document.h"
#include "./katedocumentIface.h"

class CachedFontMetrics : public QFontMetrics {
private:
    short *warray[256];
public:
    CachedFontMetrics(const QFont& f) : QFontMetrics(f) {
        for (int i=0; i<256; i++) warray[i]=0;
    }
    ~CachedFontMetrics() {
        for (int i=0; i<256; i++)
                if (warray[i]) delete[] warray[i];
    }
    int width(QChar c) {
        uchar cell=c.cell();
        uchar row=c.row();
        short *wa=warray[row];
        if (!wa) {
                // qDebug("create row: %d",row);
                wa=warray[row]=new short[256];
                for (int i=0; i<256; i++) wa[i]=-1;
        }
        if (wa[cell]<0) wa[cell]=(short) QFontMetrics::width(c);
        return (int)wa[cell];
    }
    int width(QString s) { return QFontMetrics::width(s); }
};

class Attribute {
  public:
    Attribute() { ; };

    QColor col;
    QColor selCol;
    bool bold;
    bool italic;
};

class KateAction {
  public:
    enum Action {replace, wordWrap, wordUnWrap, newLine, delLine,
      insLine, killLine};//, doubleLine, removeLine};

    KateAction(Action, PointStruc &cursor, int len = 0,
      const QString &text = QString::null);

    Action action;
    PointStruc cursor;
    int len;
    QString text;
    KateAction *next;
};

class KateActionGroup {
  public:
    // the undo group types
    enum {  ugNone,         //
            ugPaste,        // paste
            ugDelBlock,     // delete/replace selected text
            ugIndent,       // indent
            ugUnindent,     // unindent
            ugComment,      // comment
            ugUncomment,    // uncomment
            ugReplace,      // text search/replace
            ugSpell,        // spell check
            ugInsChar,      // char type/deleting
            ugDelChar,      // ''  ''
            ugInsLine,      // line insert/delete
            ugDelLine       // ''  ''
         };

    KateActionGroup(PointStruc &aStart, int type = ugNone);
    ~KateActionGroup();
    void insertAction(KateAction *);

    static const char * typeName(int type);

    PointStruc start;
    PointStruc end;
    KateAction *action;
    int undoType;
};

/**
  The text document. It contains the textlines, controls the
  document changing operations and does undo/redo. WARNING: do not change
  the text contents directly in methods where this is not explicitly
  permitted. All changes have to be made with some basic operations,
  which are recorded by the undo/redo system.
  @see TextLine
  @author Jochen Wilhelmy
*/
class KateDocument : public Kate::Document, public KateDocumentDCOPIface
{
    Q_OBJECT
    friend class KateViewInternal;
    friend class KateView;
    friend class KateIconBorder;

  public:
    KateDocument(bool bSingleViewMode=false, bool bBrowserView=false, QWidget *parentWidget = 0, const char *widgetName = 0, QObject * = 0, const char * = 0);
    ~KateDocument();

  protected:
    QFont myFont, myFontBold, myFontItalic, myFontBI;
    CachedFontMetrics myFontMetrics, myFontMetricsBold, myFontMetricsItalic, myFontMetricsBI;

  public:
    void setFont (QFont font);
    QFont getFont () { return myFont; };
    CachedFontMetrics getFontMetrics () { return myFontMetrics; };

    virtual bool openFile();
    virtual bool saveFile();

    virtual KTextEditor::View *createView( QWidget *parent, const char *name );
    virtual QString textLine( int line ) const;

    virtual void insertLine( const QString &s, int line = -1 );

    void insert_Line(const QString& s,int line=-1, bool update=true);
    void remove_Line(int line,bool update=true);
    void replaceLine(const QString& s,int line=-1);   
    virtual void insertAt( const QString &s, int line, int col, bool mark = FALSE );
    virtual void removeLine( int line );
    virtual int length() const;

    virtual void setSelection( int row_from, int col_from, int row_to, int col_t );
    virtual bool hasSelection() const;
    virtual QString selection() const;

    // only to make part work, don't change it !
    bool m_bSingleViewMode;

// public interface
    /**
     *  gets the number of lines
     */
    virtual int numLines() const;

    /**
     * gets the last line number (numLines() -1)
     */
    int lastLine() const {return numLines()-1;}

    /**
      gets the given line
      @return  the TextLine object at the given line
      @see     TextLine
    */
    TextLine::Ptr getTextLine(int line) const;

    /**
      get the length in pixels of the given line
    */
    int textLength(int line);

    void setTabWidth(int);
    int tabWidth() {return tabChars;}
    void setReadOnly(bool);
    bool isReadOnly() const;
    void setNewDoc( bool );
    bool isNewDoc() const;
    virtual void setReadWrite( bool );
    virtual bool isReadWrite() const;
    virtual void setModified(bool);
    virtual bool isModified() const;
    void setSingleSelection(bool ss) {m_singleSelection = ss;}
    bool singleSelection() {return m_singleSelection;}

    void readConfig();
    void writeConfig();
    void readSessionConfig(KConfig *);
    void writeSessionConfig(KConfig *);

    bool hasBrowserExtension() const { return m_bBrowserView; }

  protected:
    bool m_bBrowserView;

  signals:
    void selectionChanged();
    void highlightChanged();
    void modifiedChanged ();
    void preHighlightChanged(long);

  // search stuff
  protected:
    static QStringList searchForList;
    static QStringList replaceWithList;
    static uint uniqueID;

  // highlight stuff
  public:
    Highlight *highlight() {return m_highlight;}
    int highlightNum() {return hlManager->findHl(m_highlight);}
    int numAttribs() {return m_numAttribs;}
    Attribute *attribs() {return m_attribs;}
    void setDontChangeHlOnSave();

  protected:
    void setHighlight(int n);
    void makeAttribs();
    void updateFontData();

  protected slots:
    void hlChanged();

// view interaction
  public:
    virtual void addView(KTextEditor::View *);
    virtual void removeView(KTextEditor::View *);
    bool ownedView(KateView *);
    bool isLastView(int numViews);

    int getTextLineCount() {return numLines();}

    int textWidth(const TextLine::Ptr &, int cursorX);
    int textWidth(PointStruc &cursor);
    int textWidth(bool wrapCursor, PointStruc &cursor, int xPos);
    int textPos(const TextLine::Ptr &, int xPos);
//    int textPos(TextLine::Ptr &, int xPos, int &newXPos);
    int textWidth();
    int textHeight();

    void insert(VConfig &, const QString &);
    void insertFile(VConfig &, QIODevice &);

    int currentColumn(PointStruc &cursor);
    bool insertChars(VConfig &, const QString &chars);
    void newLine(VConfig &);
    void killLine(VConfig &);
    void backspace(VConfig &);
    void del(VConfig &);
    void clear();
    void cut(VConfig &);
    void copy(int flags);
    void paste(VConfig &);

    void toggleRect(int, int, int, int);
    void selectTo(VConfig &c, PointStruc &cursor, int cXPos);
    void selectAll();
    void deselectAll();
    void invertSelection();
    void selectWord(PointStruc &cursor, int flags);
    void selectLength(PointStruc &cursor, int length, int flags);

    void indent(VConfig &c) {doIndent(c, 1);}
    void unIndent(VConfig &c) {doIndent(c, -1);}
    void cleanIndent(VConfig &c) {doIndent(c, 0);}
    // called by indent/unIndent/cleanIndent
    // just does some setup and then calls optimizeLeadingSpace()
    void doIndent(VConfig &, int change);
    // optimize leading whitespace on a single line - see kwdoc.cpp for full description
    void optimizeLeadingSpace(int line, int flags, int change);

    void comment(VConfig &c) {doComment(c, 1);}
    void unComment(VConfig &c) {doComment(c, -1);}
    void doComment(VConfig &, int change);

    virtual QString text() const;
    QString getWord(PointStruc &cursor);

  public slots:
    virtual void setText(const QString &);

  public:
    long needPreHighlight(long till);
    bool hasMarkedText() {return (selectEnd >= selectStart);}
    QString markedText(int flags);
    void delMarkedText(VConfig &/*, bool undo = true*/);

    void tagLineRange(int line, int x1, int x2);
    void tagLines(int start, int end);
    void tagAll();
    void updateLines(int startLine = 0, int endLine = 0xffffff, int flags = 0, int cursorY = -1);
    void updateMaxLength(TextLine::Ptr &);
    void updateViews(KateView *exclude = 0L);

    QColor &cursorCol(int x, int y);
    void paintTextLine(QPainter &, int line, int xStart, int xEnd, bool showTabs);
    void paintTextLine(QPainter &, int line, int y, int xStart, int xEnd, bool showTabs);

    bool doSearch(SConfig &s, const QString &searchFor);

// internal
    void tagLine(int line);
    void insLine(int line);
    void delLine(int line);
    void optimizeSelection();

    void doAction(KateAction *);
    void doReplace(KateAction *);
    void doWordWrap(KateAction *);
    void doWordUnWrap(KateAction *);
    void doNewLine(KateAction *);
    void doDelLine(KateAction *);
    void doInsLine(KateAction *);
    void doKillLine(KateAction *);
    void newUndo();

    void recordStart(VConfig &, int newUndoType);
    void recordStart(KateView *, PointStruc &, int flags, int newUndoType, bool keepModal = false, bool mergeUndo = false);
    void recordAction(KateAction::Action, PointStruc &);
    void recordInsert(VConfig &, const QString &text);
    void recordReplace(VConfig &, int len, const QString &text);
    void recordInsert(PointStruc &, const QString &text);
    void recordDelete(PointStruc &, int len);
    void recordReplace(PointStruc &, int len, const QString &text);
    void recordEnd(VConfig &);
    void recordEnd(KateView *, PointStruc &, int flags);
    void doActionGroup(KateActionGroup *, int flags, bool undo = false);

    int nextUndoType();
    int nextRedoType();
    void undoTypeList(QValueList<int> &lst);
    void redoTypeList(QValueList<int> &lst);
    void undo(VConfig &, int count = 1);
    void redo(VConfig &, int count = 1);
    void clearRedo();
    void setUndoSteps(int steps);

    void setPseudoModal(QWidget *);

    void newBracketMark(PointStruc &, BracketMark &);

  protected:
    virtual void guiActivateEvent( KParts::GUIActivateEvent *ev );

  protected slots:
    void clipboardChanged();
    void slotBufferChanged();
    void slotBufferHighlight(long,long);
    void doPreHighlight();

  private slots:
    void slotViewDestroyed();

// member variables
  protected:
    long PreHighlightedTill;
    long RequestPreHighlightTill;
    KWBuffer *buffer;
    QColor colors[2];
    HlManager *hlManager;
    Highlight *m_highlight;
    int m_numAttribs;
    static const int maxAttribs;
    Attribute *m_attribs;

    int eolMode;

    int tabChars;
    int m_tabWidth;
    int fontHeight;
    int fontAscent;

    QList<KateView> views;
    bool newDocGeometry;

    TextLine::Ptr longestLine;
    float maxLength;

    PointStruc select;
    PointStruc anchor;
    int aXPos;
    int selectStart;
    int selectEnd;
    bool oldMarkState;
    bool m_singleSelection; // false: windows-like, true: X11-like

    bool readOnly;
    bool newDoc;          // True if the file is a new document (used to determine whether
                          // to check for overwriting files on save)
    bool modified;

    bool myWordWrap;
    uint myWordWrapAt;

    QList<KateActionGroup> undoList;
    int currentUndo;
    int undoState;
    int undoSteps;
    int tagStart;
    int tagEnd;
    int undoCount;          //counts merged undo steps

    QWidget *pseudoModal;   //the replace prompt is pseudo modal

    public:
    /** Tjecks if the file on disk is newer than document contents.
      If forceReload is true, the document is reloaded without asking the user,
      otherwise [default] the user is asked what to do. */
    void isModOnHD(bool forceReload=false);

    uint docID () {return myDocID;};
    QString docName () {return myDocName;};

    void setDocName (QString docName);

  public slots:
    /** Reloads the current document from disk if possible */
    void reloadFile();

  private slots:
    void slotModChanged ();

  private:
    /** updates mTime to reflect file on fs.
     called from constructor and from saveFile. */
    void setMTime();
    uint myDocID;
    QFileInfo* fileInfo;
    QDateTime mTime;
    QString myDocName;

  private:
    KateCmd *myCmd;

  public:
    KateCmd *cmd () { return myCmd; };

  private:
    QString myEncoding;

  public:
    void setEncoding (QString e) { myEncoding = e; };
    QString encoding() { return myEncoding; };

    void setWordWrap (bool on);
    bool wordWrap () { return myWordWrap; };

    void setWordWrapAt (uint col);
    uint wordWrapAt () { return myWordWrapAt; };

  signals:
    void modStateChanged (KateDocument *doc);
    void nameChanged (KateDocument *doc);

  public:
    QList<Kate::Mark> marks ();

  public slots:
    // clear buffer/filename - update the views
    void flush ();

  signals:
    /**
      The file has been saved (perhaps the name has changed). The main window
      can use this to change its caption
    */
    void fileNameChanged ();

  public:
    //end of line settings
    enum Eol_settings {eolUnix=0,eolDos=1,eolMacintosh=2};

  // for the DCOP interface
  public:
    void open (const QString &name=0);

  public:
    // wrap the text of the document at the column col
    void wrapText (uint col);

  public slots:
     void applyWordWrap ();

  private:

	class KateDocPrivate
	{
		public:
	        bool hlSetByUser;
	};
 
 
// BCI: Add a real d-pointer in the next BIC release
static QPtrDict<KateDocPrivate>* d_ptr;
static void cleanup_d_ptr()
      {
          delete d_ptr;
      }
 
KateDocPrivate* d( const KateDocument* foo )
      {
           if ( !d_ptr ) {
                     d_ptr = new QPtrDict<KateDocPrivate>;
                     //qAddPostRoutine( cleanup_d_ptr );
                }
                KateDocPrivate* ret = d_ptr->find( (void*) foo );
                if ( ! ret ) {
                        ret = new KateDocPrivate;
                        d_ptr->replace( (void*) foo, ret );
                }
                return ret;
      }
 
void delete_d( const KateDocument* foo )
     {
          if ( d_ptr )
              d_ptr->remove( (void*) foo );
     }

};

#endif


