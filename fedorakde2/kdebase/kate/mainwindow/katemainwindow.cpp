/***************************************************************************
                          katemainwindow.cpp  -  description
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

#include "katemainwindow.h"
#include "katemainwindow.moc"

#include "kateconfigdialog.h"

#include "../console/kateconsole.h"
#include "../document/katedocument.h"
#include "../document/katedocmanager.h"
#include "../pluginmanager/katepluginmanager.h"
#include "../pluginmanager/kateconfigplugindialogpage.h"
#include "../view/kateviewmanager.h"
#include "../app/kateapp.h"
#include "../fileselector/katefileselector.h"
#include "../filelist/katefilelist.h"
#include "../factory/katefactory.h"

#include "../view/kateviewdialog.h"
#include "../document/katedialogs.h"
#include "../document/katehighlight.h"

#include <qcheckbox.h>
#include <qiconview.h>
#include <qinputdialog.h>
#include <qlayout.h>
#include <qlayout.h>
#include <qlist.h>
#include <qsplitter.h>
#include <qsplitter.h>
#include <qvbox.h>
#include <qwhatsthis.h>
#include <qwidgetstack.h>

#include <dcopclient.h>
#include <kinstance.h>
#include <kaction.h>
#include <kapp.h>
#include <kcmdlineargs.h>
#include <kdebug.h>
#include <kdialogbase.h>
#include <kdiroperator.h>
#include <kdockwidget.h>
#include <kedittoolbar.h>
#include <kfiledialog.h>
#include <kglobalaccel.h>
#include <kglobal.h>
#include <kglobalsettings.h>
#include <kiconloader.h>
#include <kio/netaccess.h>
#include <kkeydialog.h>
#include <klistbox.h>
#include <klocale.h>
#include <kmimetype.h>
#include <kopenwith.h>
#include <kparts/event.h>
#include <kparts/part.h>
#include <ksimpleconfig.h>
#include <kstdaction.h>
#include <kstddirs.h>
#include <ktrader.h>
#include <kuniqueapp.h>
#include <kurldrag.h>

uint KateMainWindow::uniqueID = 0;

KateMainWindow::KateMainWindow(KateDocManager *_docManager, KatePluginManager *_pluginManager) :
	Kate::MainWindow (),
             DCOPObject ((QString("KateMainWindow%1").arg(uniqueID)).latin1())
{
  docManager =  _docManager;
  pluginManager =_pluginManager;
  config = KateFactory::instance()->config();

  myID = uniqueID;
  uniqueID++;

  consoleDock = 0L;
  console = 0L;

  setXMLFile( "kateui.rc" );

  setAcceptDrops(true);

  setupMainWindow();
  setupActions();

  createGUI();

  pluginManager->enableAllPluginsGUI (this);

  // connect settings menu aboutToshow
  QPopupMenu* pm_set = (QPopupMenu*)factory()->container("settings", this);
  connect(pm_set, SIGNAL(aboutToShow()), this, SLOT(settingsMenuAboutToShow()));

  // connect settings menu aboutToshow
  documentMenu = (QPopupMenu*)factory()->container("documents", this);
  connect(documentMenu, SIGNAL(aboutToShow()), this, SLOT(documentMenuAboutToShow()));

  // connect settings menu aboutToshow
  bookmarkMenu = (QPopupMenu*)factory()->container("bookmarks", this);
  connect(bookmarkMenu, SIGNAL(aboutToShow()), this, SLOT(bookmarkMenuAboutToShow()));

  readOptions(config);
}

KateMainWindow::~KateMainWindow()
{
}

void KateMainWindow::setupMainWindow ()
{
  mainDock = createDockWidget( "mainDock", 0L );
  filelistDock =  createDockWidget( "filelistDock",  SmallIcon("kmultiple"), 0L, "Open Files", "");
  fileselectorDock = createDockWidget( "fileselectorDock", SmallIcon("fileopen"), 0L, "Selector", "");

  mainDock->setGeometry(100, 100, 100, 100);
  viewManager = new KateViewManager (mainDock, docManager);
  viewManager->setMinimumSize(200,200);
  mainDock->setWidget(viewManager);

  setMainDockWidget( mainDock );
  setView( mainDock );

  filelist = new KateFileList (docManager, viewManager, filelistDock, "filelist");
  filelistDock->setWidget (filelist);

  fileselector = new KateFileSelector(fileselectorDock, "operator");
  fileselector->dirOperator()->setView(KFile::Simple);
  fileselectorDock->setWidget (fileselector);

#ifdef  _KDOCKWIDGET_2_2_
  filelistDock->setDockWindowType (NET::Tool);
  fileselectorDock->setDockWindowType (NET::Tool);
  filelistDock->setDockWindowTransient (this, true);
  fileselectorDock->setDockWindowTransient (this, true);
#endif

  connect(fileselector->dirOperator(),SIGNAL(fileSelected(const KFileViewItem*)),this,SLOT(fileSelected(const KFileViewItem*)));

  mainDock->setEnableDocking ( KDockWidget::DockNone );
  mainDock->setDockSite( KDockWidget::DockCorner );

  filelistDock->manualDock ( mainDock, KDockWidget::DockLeft, 20 );
  fileselectorDock ->manualDock(filelistDock, KDockWidget::DockCenter);

  statusBar()->hide();
}

bool KateMainWindow::eventFilter(QObject* o, QEvent* e)
{
  if (e->type() == QEvent::KeyPress)
        {
                QKeyEvent *ke=(QKeyEvent*)e;
                if (ke->key()==goNext->accel())
                        {
                                kdDebug(13000)<<"Jump next view  registered in Konsole";
				slotGoNext();
                                return true;
                        }
		else
	                if (ke->key()==goPrev->accel())
        	                {
                	                kdDebug(13000)<<"Jump prev view  registered in Konsole";
                        	        slotGoPrev();
                                	return true;
	                        }
        }
  return QWidget::eventFilter(o,e);
}

void KateMainWindow::setupActions()
{
  KStdAction::openNew( viewManager, SLOT( slotDocumentNew() ), actionCollection(), "file_new" );
  KStdAction::open( viewManager, SLOT( slotDocumentOpen() ), actionCollection(), "file_open" );

  fileOpenRecent = KStdAction::openRecent (viewManager, SLOT(openConstURL (const KURL&)), actionCollection());

  KStdAction::save( viewManager, SLOT( slotDocumentSave() ), actionCollection(), "file_save" );
  new KAction( i18n("Save A&ll"),"save_all", CTRL+Key_L, viewManager, SLOT( slotDocumentSaveAll() ), actionCollection(), "file_save_all" );
  KStdAction::saveAs( viewManager, SLOT( slotDocumentSaveAs() ), actionCollection(), "file_save_as" );
  KStdAction::print(viewManager, SLOT(printDlg()), actionCollection());
  KStdAction::close( viewManager, SLOT( slotDocumentClose() ), actionCollection(), "file_close" );
  new KAction( i18n( "&Close All" ), 0, viewManager, SLOT( slotDocumentCloseAll() ), actionCollection(), "file_close_all" );

  new KAction(i18n("New &Window"), 0, this, SLOT(newWindow()), actionCollection(), "file_newWindow");

  KStdAction::quit( this, SLOT( slotFileQuit() ), actionCollection(), "file_quit" );

  editUndo = KStdAction::undo(viewManager, SLOT(slotUndo()), actionCollection());
  editRedo = KStdAction::redo(viewManager, SLOT(slotRedo()), actionCollection());
  editUndoHist = new KAction(i18n("Undo/Redo &History..."), "history", 0, viewManager, SLOT(slotUndoHistory()), actionCollection(), "edit_undoHistory");

  KStdAction::cut(viewManager, SLOT(slotCut()), actionCollection());
  KStdAction::copy(viewManager, SLOT(slotCopy()), actionCollection()) ;
  KStdAction::paste(viewManager, SLOT(slotPaste()), actionCollection());

  KStdAction::selectAll(viewManager, SLOT(slotSelectAll()), actionCollection());
  new KAction(i18n("&Deselect All"), 0, viewManager, SLOT(slotDeselectAll()), actionCollection(), "edit_deselectAll");
  new KAction(i18n("Invert &Selection"), 0, viewManager, SLOT(slotInvertSelection()), actionCollection(), "edit_invertSelection");

  KStdAction::find(viewManager, SLOT(slotFind()), actionCollection());
  KStdAction::findNext(viewManager, SLOT(slotFindAgain()), actionCollection());
  KStdAction::findPrev(viewManager, SLOT(slotFindAgainB()), actionCollection(), "edit_find_prev");
  KStdAction::replace(viewManager, SLOT(slotReplace()), actionCollection());

  new KAction(i18n("&Indent"), "indent", CTRL+Key_I, viewManager, SLOT(slotIndent()), actionCollection(), "edit_indent");
  new KAction(i18n("&Unindent"), "unindent", CTRL+SHIFT+Key_I, viewManager, SLOT(slotUnIndent()), actionCollection(), "edit_unindent");

  new KAction(i18n("Comme&nt"), CTRL+Qt::Key_NumberSign, viewManager, SLOT(slotComment()), actionCollection(), "edit_comment");
  new KAction(i18n("Uncommen&t"), CTRL+SHIFT+Qt::Key_NumberSign, viewManager, SLOT(slotUnComment()), actionCollection(), "edit_uncomment");

  new KAction(i18n("Apply Word Wrap"), 0, viewManager, SLOT(slotApplyWordWrap()), actionCollection(), "edit_apply_wordwrap");

  new KAction(i18n("Editing Co&mmand"), Qt::CTRL+Qt::Key_M, viewManager, SLOT(slotEditCommand()),
                                  actionCollection(), "edit_cmd");

  KStdAction::gotoLine(viewManager, SLOT(slotGotoLine()), actionCollection());

  bookmarkToggle = new KAction(i18n("Toggle &Bookmark"), Qt::CTRL+Qt::Key_B, viewManager, SLOT(toggleBookmark()), actionCollection(), "edit_bookmarkToggle");
  bookmarkClear = new KAction(i18n("Clear Bookmarks"), 0, viewManager, SLOT(clearBookmarks()), actionCollection(), "edit_bookmarksClear");

  KStdAction::spelling(viewManager, SLOT(slotSpellcheck()), actionCollection());

  new KAction( i18n("Split &Vertical"), "view_left_right", CTRL+SHIFT+Key_L, viewManager, SLOT( slotSplitViewSpaceVert() ), actionCollection(), "view_split_vert");
  new KAction( i18n("Split &Horizontal"), "view_top_bottom", CTRL+SHIFT+Key_T, viewManager, SLOT( slotSplitViewSpaceHoriz() ), actionCollection(), "view_split_horiz");
  closeCurrentViewSpace = new KAction( i18n("Close &Current"), "view_remove", CTRL+SHIFT+Key_R, viewManager, SLOT( slotCloseCurrentViewSpace() ), actionCollection(), "view_close_current_space");

  viewBorder =  new KToggleAction(i18n("Show &IconBorder"), Key_F6, viewManager, SLOT(toggleIconBorder()), actionCollection(), "view_border");

  goNext=new KAction(i18n("Next View"),Key_F8,viewManager, SLOT(activateNextView()),actionCollection(),"go_next");
  goPrev=new KAction(i18n("Previous View"),SHIFT+Key_F8,viewManager, SLOT(activatePrevView()),actionCollection(),"go_prev");

  windowNext = KStdAction::back(viewManager, SLOT(slotWindowNext()), actionCollection());
  windowPrev = KStdAction::forward(viewManager, SLOT(slotWindowPrev()), actionCollection());

  setEndOfLine = new KSelectAction(i18n("&End Of Line"), 0, actionCollection(), "set_eol");
  connect(setEndOfLine, SIGNAL(activated(int)), viewManager, SLOT(setEol(int)));
  connect(setEndOfLine->popupMenu(), SIGNAL(aboutToShow()), this, SLOT(setEOLMenuAboutToShow()));
  QStringList list;
  list.append("&Unix");
  list.append("&Windows/Dos");
  list.append("&Macintosh");
  setEndOfLine->setItems(list);

  documentReload = new KAction(i18n("&Reload"), "reload", Key_F5, viewManager, SLOT(reloadCurrentDoc()), actionCollection(), "file_reload");

  documentOpenWith = new KActionMenu(i18n("O&pen with"), actionCollection(), "file_open_with");
  connect(documentOpenWith->popupMenu(), SIGNAL(aboutToShow()), this, SLOT(mSlotFixOpenWithMenu()));
  connect(documentOpenWith->popupMenu(), SIGNAL(activated(int)), this, SLOT(slotOpenWithMenuAction(int)));

  setVerticalSelection = new KToggleAction(i18n("&Vertical Selection"), Key_F4, viewManager, SLOT(toggleVertical()),
                                             actionCollection(), "set_verticalSelect");

  if (pluginManager->myPluginList.count() > 0)
    new KAction(i18n("Contents &Plugins"), 0, this, SLOT(pluginHelp()), actionCollection(), "help_plugins_contents");

  KStdAction::keyBindings(this, SLOT(editKeys()), actionCollection());
  KStdAction::configureToolbars(this, SLOT(slotEditToolbars()), actionCollection(), "set_configure_toolbars");

  // toggle dockwidgets
  settingsShowFilelist = new KToggleAction(i18n("Show Filelist"), 0, filelistDock, SLOT(changeHideShowState()), actionCollection(), "settings_show_filelist");
  settingsShowFileselector = new KToggleAction(i18n("Show Fileselector"), 0, fileselectorDock, SLOT(changeHideShowState()), actionCollection(), "settings_show_fileselector");
  settingsShowConsole = new KToggleAction(i18n("Show &Console"), Qt::Key_F7, this, SLOT(slotSettingsShowConsole()), actionCollection(), "settings_show_console");

  // allow full path in title -anders
  settingsShowFullPath = new KToggleAction(i18n("Show Full &Path in Title"), 0, this, SLOT(slotSettingsShowFullPath()), actionCollection(), "settings_show_full_path");
  settingsShowToolbar = KStdAction::showToolbar(this, SLOT(slotSettingsShowToolbar()), actionCollection(), "settings_show_toolbar");
  settingsConfigure = KStdAction::preferences(this, SLOT(slotConfigure()), actionCollection(), "settings_configure");

  connect(viewManager,SIGNAL(viewChanged()),this,SLOT(slotWindowActivated()));
  connect(viewManager,SIGNAL(statChanged()),this,SLOT(slotCurrentDocChanged()));

  setHighlight = new KActionMenu (i18n("&Highlight Mode"), actionCollection(), "set_highlight");
  connect(setHighlight->popupMenu(), SIGNAL(aboutToShow()), this, SLOT(setHighlightMenuAboutToShow()));

  slotWindowActivated ();
}

bool KateMainWindow::queryClose()
{
  bool val = false;

  if ( ((KateApp *)kapp)->mainWindowsCount () < 2 )
  {
    saveOptions(config);

    viewManager->saveAllDocsAtCloseDown(  );

    if ( (!docManager->currentDoc()) || ((!viewManager->activeView()->doc()->isModified()) && (docManager->docCount() == 1)))
    {
      if (viewManager->activeView()) viewManager->deleteLastView ();
      val = true;
    }
  }
  else
    val = true;

  if (val)
  {
    ((KateApp *)kapp)->removeMainWindow (this);
    if (consoleDock && console)
      if (consoleDock->isVisible())
        consoleDock->changeHideShowState();
  }

  return val;
}
// anders: attempt to get session manager to restore kate.
void KateMainWindow::saveProperties(KConfig* cfg)
{
  cfg->writeEntry("hello", "world");
}

void KateMainWindow::newWindow ()
{
  ((KateApp *)kapp)->newMainWindow ();
}

void KateMainWindow::slotEditToolbars()
{
  KEditToolbar dlg(factory());

  if (dlg.exec())
    createGUI();
}

void KateMainWindow::slotFileQuit()
{
  queryClose ();
}

void KateMainWindow::readOptions(KConfig *config)
{
  config->setGroup("General");
  syncKonsole =  config->readBoolEntry("Sync Konsole", true);

  if (config->readBoolEntry("Show Console", false))
  {
    slotSettingsShowConsole();
  }

  resize( config->readSizeEntry( "size", new QSize(600, 400) ) );
  viewManager->setShowFullPath(config->readBoolEntry("Show Full Path in Title", false));
  settingsShowFullPath->setChecked(viewManager->getShowFullPath());
  settingsShowToolbar->setChecked(config->readBoolEntry("Show Toolbar", true));
  slotSettingsShowToolbar();
  viewManager->setUseOpaqueResize(config->readBoolEntry("Opaque Resize", true));

  fileOpenRecent->loadEntries(config, "Recent Files");

  fileselector->readConfig(config, "fileselector");
  fileselector->setView(KFile::Default);

  readDockConfig();
}

void KateMainWindow::saveOptions(KConfig *config)
{
  config->setGroup("General");

  if (consoleDock && console)
    config->writeEntry("Show Console", console->isVisible());
  else
    config->writeEntry("Show Console", false);

  config->writeEntry("size", size());
  config->writeEntry("Show Full Path in Title", viewManager->getShowFullPath());
  config->writeEntry("Show Toolbar", settingsShowToolbar->isChecked());
  config->writeEntry("Opaque Resize", viewManager->useOpaqueResize);
  config->writeEntry("Sync Konsole", syncKonsole);

  fileOpenRecent->saveEntries(config, "Recent Files");

  fileselector->saveConfig(config, "fileselector");

  writeDockConfig();

  if (viewManager->activeView())
    KateFactory::instance()->config()->sync();

  viewManager->saveViewSpaceConfig();
}

void KateMainWindow::slotWindowActivated ()
{
  static QString path;

  if (viewManager->activeView() != 0)
  {
    if (console && syncKonsole)
    {
      QString newPath = viewManager->activeView()->doc()->url().directory();

      if ( newPath != path )
      {
        path = newPath;
        console->cd (path);
      }
    }

    viewBorder->setChecked(viewManager->activeView()->iconBorder());
  }

  if (viewManager->viewCount ()  > 1)
  {
    windowNext->setEnabled(true);
    windowPrev->setEnabled(true);
  }
  else
  {
    windowNext->setEnabled(false);
    windowPrev->setEnabled(false);
  }

  if (viewManager->viewSpaceCount() == 1)
    closeCurrentViewSpace->setEnabled(false);
  else
    closeCurrentViewSpace->setEnabled(true);
}

void KateMainWindow::slotCurrentDocChanged()
{
  if (!viewManager->activeView())
    return;

  if (!(viewManager->activeView()->undoState() & 1))
  {
    editUndo->setEnabled(false);
  }
  else
  {
    editUndo->setEnabled(true);
  }

  if (!(viewManager->activeView()->undoState() & 2))
  {
    editRedo->setEnabled(false);
  }
  else
  {
    editRedo->setEnabled(true);
  }

  if (!(viewManager->activeView()->undoState() & 1) && !(viewManager->activeView()->undoState() & 2))
  {
    editUndoHist->setEnabled(false);
  }
  else
  {
    editUndoHist->setEnabled(true);
  }
}

 void KateMainWindow::documentMenuAboutToShow()
{
  documentMenu->clear ();
  windowNext->plug (documentMenu);
  windowPrev->plug (documentMenu);
  documentMenu->insertSeparator ();
  setHighlight->plug (documentMenu);
  setEndOfLine->plug (documentMenu);
  documentMenu->insertSeparator ();

  uint z=0;
  int i=1;

  QString entry;
  while ( z<docManager->docCount() )
  {
    if ( (!docManager->nthDoc(z)->url().isEmpty()) && (docManager->nthDoc(z)->url().filename() != 0) )
      entry=QString("&%1 ").arg(i)+docManager->nthDoc(z)->url().filename();
    else
      entry=QString("&%1 ").arg(i)+i18n("Untitled %1").arg(docManager->nthDoc(z)->docID());

    if (docManager->nthDoc(z)->isModified())
      entry.append (i18n(" - Modified"));

    documentMenu->insertItem ( entry, viewManager, SLOT (activateView (int)), 0,  docManager->nthDoc(z)->docID());

    if (viewManager->activeView())
      documentMenu->setItemChecked( docManager->nthDoc(z)->docID(), ((KateDocument *)viewManager->activeView()->doc())->docID() == docManager->nthDoc(z)->docID() );

    z++;
    i++;
  }
}

void KateMainWindow::bookmarkMenuAboutToShow()
{
  bookmarkMenu->clear ();
  bookmarkToggle->plug (bookmarkMenu);
  bookmarkClear->plug (bookmarkMenu);
  bookmarkMenu->insertSeparator ();

  list = viewManager->activeView()->doc()->marks();
  for (int i=0; (uint) i < list.count(); i++)
  {
    if (list.at(i)->type&KateDocument::Bookmark)
    {
      QString bText = viewManager->activeView()->textLine(list.at(i)->line);
      bText.truncate(32);
      bText.append ("...");
      bookmarkMenu->insertItem ( QString("%1 - \"%2\"").arg(list.at(i)->line).arg(bText), this, SLOT (gotoBookmark(int)), 0, i );
    }
  }
}

void KateMainWindow::gotoBookmark (int n)
{
  viewManager->gotoMark (list.at(n));
}

void KateMainWindow::setHighlightMenuAboutToShow()
{
  static QStringList subMenusName;
  static QStringList names;
  static QList<QPopupMenu> subMenus;
  int count = HlManager::self()->highlights();
  static QString oldActiveSec;
  static int oldActiveID;

  for (int z=0; z<count; z++)
  {
    QString hlName = HlManager::self()->hlName (z);
    QString hlSection = HlManager::self()->hlSection (z);

    if ((hlSection != "") && (names.contains(hlName) < 1))
    {
      if (subMenusName.contains(hlSection) < 1)
      {
        subMenusName << hlSection;
        QPopupMenu *menu = new QPopupMenu ();
        subMenus.append(menu);
        setHighlight->popupMenu()->insertItem (hlSection, menu);
      }

      int m = subMenusName.findIndex (hlSection);
      names << hlName;
      subMenus.at(m)->insertItem ( hlName, viewManager, SLOT(slotSetHl(int)), 0,  z);
    }
    else if (names.contains(hlName) < 1)
    {
      names << hlName;
      setHighlight->popupMenu()->insertItem ( hlName, viewManager, SLOT(slotSetHl(int)), 0,  z);
    }
  }

  if (viewManager->activeView())
  {
    int s = subMenusName.findIndex (oldActiveSec);
    if (subMenus.at(s))
      subMenus.at(s)->setItemChecked (oldActiveID, false);
    else
      setHighlight->popupMenu()->setItemChecked (0, false);

    int i = subMenusName.findIndex (HlManager::self()->hlSection (viewManager->activeView()->getHl()));
    if (subMenus.at(i))
      subMenus.at(i)->setItemChecked (viewManager->activeView()->getHl(), true);
    else
      setHighlight->popupMenu()->setItemChecked (0, true);

    oldActiveSec = HlManager::self()->hlSection (viewManager->activeView()->getHl());
    oldActiveID = viewManager->activeView()->getHl();
  }
}

void KateMainWindow::dragEnterEvent( QDragEnterEvent *event )
{
  event->accept(QUriDrag::canDecode(event));
}

void KateMainWindow::dropEvent( QDropEvent *event )
{
  slotDropEvent(event);
}

void KateMainWindow::slotDropEvent( QDropEvent * event )
{
  KURL::List textlist;
  if (!KURLDrag::decode(event, textlist)) return;

  for (KURL::List::Iterator i=textlist.begin(); i != textlist.end(); ++i)
  {
    viewManager->openURL (*i);
  }
}

void KateMainWindow::editKeys()
{
  KKeyDialog::configureKeys(actionCollection(), xmlFile());
}

void KateMainWindow::slotSettingsShowConsole()
{
  if (!consoleDock && !console)
  {
    consoleDock = createDockWidget( "consoleDock", 0, 0L, "Console", "" );
    console = new KateConsole (consoleDock, "console");
    console->installEventFilter( this );
    console->setMinimumSize(50,50);
    consoleDock->setWidget( console );
    consoleDock->manualDock ( mainDock, KDockWidget::DockBottom, 20 );
    consoleDock->changeHideShowState();

#ifdef  _KDOCKWIDGET_2_2_
    consoleDock->setDockWindowType (NET::Tool);
    consoleDock->setDockWindowTransient (this, true);
#endif
  }

  consoleDock->changeHideShowState();

  if( consoleDock->isVisible() )
    console->setFocus();
  else
    if (viewManager->activeView())
      viewManager->activeView()->setFocus();
}

void KateMainWindow::settingsMenuAboutToShow()
{
  settingsShowFilelist->setChecked( filelistDock->isVisible() );
  settingsShowFileselector->setChecked( fileselectorDock->isVisible() );

  if (consoleDock)
    settingsShowConsole->setChecked( consoleDock->isVisible() );
}

void KateMainWindow::setEOLMenuAboutToShow()
{
  int eol = viewManager->activeView()->getEol();
  eol = eol>=0? eol: 0;
  setEndOfLine->setCurrentItem( eol );
}

void KateMainWindow::openURL (const QString &name)
{
  viewManager->openURL (KURL(name));
}

void KateMainWindow::slotSettingsShowFullPath()
{
  viewManager->setShowFullPath( settingsShowFullPath->isChecked() );
}

void KateMainWindow::slotSettingsShowToolbar()
{
  if (settingsShowToolbar->isChecked())
    toolBar()->show();
  else
    toolBar()->hide();
}

void KateMainWindow::slotConfigure()
{
  KateConfigDialog* dlg = new KateConfigDialog (this, "configdialog");
  dlg->exec();
  delete dlg;
}

//Set focus to next input element
void KateMainWindow::slotGoNext()
{
  QFocusEvent::setReason(QFocusEvent::Tab);
  /*res= */focusNextPrevChild(true); //TRUE == NEXT , FALSE = PREV
  QFocusEvent::resetReason();
}

//Set focus to previous input element
void KateMainWindow::slotGoPrev()
{
  QFocusEvent::setReason(QFocusEvent::Tab);
  /*res= */focusNextPrevChild(false); //TRUE == NEXT , FALSE = PREV
  QFocusEvent::resetReason();
}

KURL KateMainWindow::currentDocUrl()
{
  return viewManager->activeView()->doc()->url();
}

void KateMainWindow::fileSelected(const KFileViewItem *file)
{
  KURL u(file->urlString());
  viewManager->openURL( u );
}

void KateMainWindow::restore(bool isRestored)
{ viewManager->reopenDocuments(isRestored); }

void KateMainWindow::mSlotFixOpenWithMenu()
{
  //kdDebug()<<"13000"<<"fixing open with menu"<<endl;
  documentOpenWith->popupMenu()->clear();
  // get a list of appropriate services.
  KMimeType::Ptr mime = KMimeType::findByURL( viewManager->activeView()->doc()->url() );
  //kdDebug()<<"13000"<<"url: "<<viewManager->activeView()->doc()->url().prettyURL()<<"mime type: "<<mime->name()<<endl;
  // some checking goes here...
  KTrader::OfferList offers = KTrader::self()->query(mime->name(), "Type == 'Application'");
  // for each one, insert a menu item...
  for(KTrader::OfferList::Iterator it = offers.begin(); it != offers.end(); ++it) {
    if ((*it)->name() == "Kate") continue;
    documentOpenWith->popupMenu()->insertItem( SmallIcon( (*it)->icon() ), (*it)->name() );
  }
  // append "Other..." to call the KDE "open with" dialog.
  documentOpenWith->popupMenu()->insertItem(i18n("&Other..."));
}

void KateMainWindow::slotOpenWithMenuAction(int idx)
{
  KURL::List list;
  list.append( viewManager->activeView()->doc()->url() );
  QString* appname = new QString( documentOpenWith->popupMenu()->text(idx) );
  if ( appname->compare(i18n("&Other...")) == 0 ) {
    // display "open with" dialog
    KOpenWithDlg* dlg = new KOpenWithDlg(list);
    if (dlg->exec())
      KRun::run(*dlg->service(), list);
    return;
  }
  QString qry = QString("((Type == 'Application') and (Name == '%1'))").arg( appname->latin1() );
  KMimeType::Ptr mime = KMimeType::findByURL( viewManager->activeView()->doc()->url() );
  KTrader::OfferList offers = KTrader::self()->query(mime->name(), qry);
  KService::Ptr app = offers.first();
  // some checking here: pop a wacko message it the app wasn't found.
  KRun::run(*app, list);
}

Kate::ViewManager *KateMainWindow::getViewManager ()
{
  return ((Kate::ViewManager *)viewManager);
}

Kate::DocManager *KateMainWindow::getDocManager ()
{
  return ((Kate::DocManager *)docManager);
}

KDockWidget *KateMainWindow::getMainDock ()
{
  return mainDock;
}

void KateMainWindow::pluginHelp()
{
  kapp->invokeHelp (QString::null, "kate-plugins");
}
