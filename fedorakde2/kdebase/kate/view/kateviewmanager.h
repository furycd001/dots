/***************************************************************************
                          kateviewmanager.h  -  description
                             -------------------
    begin                : Wed Jan 3 2001
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
#ifndef kate_viewmanager_h__
#define kate_viewmanager_h__

#include "../main/katemain.h"
#include "../interfaces/viewmanager.h"
#include "../interfaces/view.h"
#include "../interfaces/document.h"
#include "kateview.h"

class KateSplitter;
class KSimpleConfig;
class KateViewManager : public Kate::ViewManager
{
  Q_OBJECT

  friend class KateConfigDialog;
  friend class KateMainWindow;

  public:
    KateViewManager (QWidget *parent=0, KateDocManager *docManager=0);
    ~KateViewManager ();

  protected:
    bool useOpaqueResize;
    QList<KateView> viewList;
    void saveAllDocsAtCloseDown();
    /** This will save the splitter configuration */
    void saveViewSpaceConfig();

    /** reopens documents that was open last time kate was shut down*/
    void reopenDocuments(bool isRestore);

  public slots:
    virtual void openURL (KURL url=0L);
    void openConstURL (const KURL&url=0L);
    void reloadCurrentDoc();

  private:
    QList<KateViewSpace> viewSpaceList;

    KateDocManager *docManager;
    QGridLayout *grid;

    bool createView ( bool newDoc=true, KURL url=0L, KateView *origView=0L, KateDocument *doc=0L );
    bool deleteView ( KateView *view, bool delViewSpace = true);

    void moveViewtoSplit (KateView *view);
    void moveViewtoStack (KateView *view);

    /** Save the configuration of a single splitter.
     * If child splitters are found, it calls it self with those as the argument.
     * If a viewspace child is found, it is asked to save its filelist.
     */
    void saveSplitterConfig(KateSplitter* s, int idx=0, KSimpleConfig* config=0L);

    /** Restore view configuration.
     * If only one view was present at close down, calls reopenDocuemnts.
     * The configuration will be restored so that viewspaces are created, sized
     * and populated exactly like at shotdown.
     */
    void restoreViewConfig();

    /** Restore a single splitter.
     * This is all the work is done for @see saveSplitterConfig()
     */
    void restoreSplitter ( KSimpleConfig* config, QString group, QWidget* parent );

    void removeViewSpace (KateViewSpace *viewspace);

    bool showFullPath;

  public:
    virtual KateView* activeView ();
    KateViewSpace* activeViewSpace ();

    uint viewCount ();
    uint viewSpaceCount ();

  private slots:
    void activateView ( KateView *view );
    void activateSpace ( KateView* v );
    void slotViewChanged();
    bool closeDocWithAllViews ( KateView *view );

  public:
    void deleteLastView ();

    /** Splits a KateViewSpace into two.
      * The operation is performed by creating a KateSplitter in the parent of the KateViewSpace to be split,
      * which is then moved to that splitter. Then a new KateViewSpace is created and added to the splitter,
      * and a KateView is created to populate the new viewspace. The new KateView is made the active one,
      * because createView() does that.
      * If no viewspace is provided, the result of activeViewSpace() is used.
      * The isHoriz, true pr default, decides the orientation of the splitting action.
      * If atTop is true, the new viewspace will be moved to the first position in the new splitter.
      * If a newViewUrl is provided, the new view will show the document in that URL if any, otherwise
      * the document of the current view in the viewspace to be split is used.
      */
    void splitViewSpace( KateViewSpace* vs=0L, bool isHoriz=true, bool atTop=false, KURL newViewUrl=0L );

    bool getShowFullPath() { return showFullPath; }
    void setUseOpaqueResize( bool enable );

  public slots:
    void activateView ( uint docID );
    void activateView ( int docID ) { activateView((uint) docID); };

    void slotDocumentCloseAll ();
    void slotDocumentSaveAll();

    void slotWindowNext();
    void slotWindowPrev();

    void slotDocumentNew ();
    void slotDocumentOpen ();
    void slotDocumentSave ();
    void slotDocumentSaveAs ();
    void slotDocumentClose ();
    /** Splits the active viewspace horizontally */
    void slotSplitViewSpaceHoriz () { splitViewSpace(); }
    /** Splits the active viewspace vertically */
    void slotSplitViewSpaceVert () { splitViewSpace( 0L, false ); }

    void slotCloseCurrentViewSpace();

    void slotUndo ();
    void slotRedo ();
    void slotUndoHistory ();

    void slotCut ();
    void slotCopy ();
    void slotPaste ();

    void slotSelectAll ();
    void slotDeselectAll ();
    void slotInvertSelection ();

    void slotFind ();
    void slotFindAgain ();
    void slotFindAgainB ();
    void slotReplace ();

    void slotIndent();
    void slotUnIndent();

    void slotEditCommand ();

    void slotSetHl (int n);

    void slotSpellcheck ();
    void slotGotoLine ();

    void statusMsg ();

    void printNow();
    void printDlg();

    void setActiveSpace ( KateViewSpace* vs );
    void setActiveView ( KateView* view );

    void setShowFullPath(bool enable);

    void setWindowCaption();

    void activateNextView();
    void activatePrevView();

    void toggleBookmark();
    void clearBookmarks();

    void slotComment ();
    void slotUnComment ();

    void setEol(int);
    void toggleIconBorder ();

    void toggleVertical();

    void gotoMark (Kate::Mark *mark);

    void slotApplyWordWrap ();

  signals:
    void statusChanged (KateView *, int, int, int, int, QString);
    void statChanged ();
    void viewChanged ();

  public:  //KatePluginIface
  virtual Kate::View *getActiveView(){return (Kate::View *)activeView();};
};

#endif
