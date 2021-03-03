/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>
   Copyright (C) 2000, 2001 David Faure <faure@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License version 2 as published by the Free Software Foundation.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "kdiconview.h"
#include "krootwm.h"

#include <kapp.h>
#include <kcolordrag.h>
#include <kdesktopfile.h>
#include <kfileivi.h>
#include <kglobalsettings.h>
#include <kipc.h>
#include <klocale.h>
#include <konq_defaults.h>
#include <konq_dirlister.h>
#include <konq_drag.h>
#include <konq_fileitem.h>
#include <konq_operations.h>
#include <konq_popupmenu.h>
#include <konq_propsview.h>
#include <konq_settings.h>
#include <konq_undo.h>
#include <kstdaction.h>
#include <kstddirs.h>
#include <kurldrag.h>

#include <qdir.h>
#include <qclipboard.h>
#include <unistd.h>

// for multihead
extern int kdesktop_screen_number;

KDIconView::KDIconView( QWidget *parent, const char* name )
    : KonqIconViewWidget( parent, name, WResizeNoErase, true ),
      m_actionCollection(),
      m_accel( 0L ),
      m_bNeedRepaint( false ),
      m_bNeedSave( false ),
      m_hasExistingPos( false ),
      m_bShowDot( false ),
      m_bVertAlign( true ),
      m_tAlign( TopToBottom ),
      m_pSettings( 0L ),
      m_dirLister( 0L ),
      m_mergeDirs(),
      m_dotDirectory( 0L ),
      m_lastIcon( 0L ),
      m_preview(),
      m_eSortCriterion( NameCaseInsensitive ),
      m_bSortDirectoriesFirst( true ),
      m_itemsAlwaysFirst()
{
    setResizeMode( Fixed );

    connect( QApplication::clipboard(), SIGNAL(dataChanged()),
             this, SLOT(slotClipboardDataChanged()) );

    setURL( desktopURL() ); // sets m_url and m_dotDirectoryPath

    connect( this, SIGNAL( executed( QIconViewItem * ) ),
             SLOT( slotReturnPressed( QIconViewItem * ) ) );
    connect( this, SIGNAL( returnPressed( QIconViewItem * ) ),
             SLOT( slotReturnPressed( QIconViewItem * ) ) );
    connect( this, SIGNAL( mouseButtonPressed(int, QIconViewItem*, const QPoint&)),
             SLOT( slotMouseButtonPressed(int, QIconViewItem*, const QPoint&)) );
    connect( this, SIGNAL( mouseButtonClicked(int, QIconViewItem*, const QPoint&)),
             SLOT( slotMouseButtonClickedKDesktop(int, QIconViewItem*, const QPoint&)) );

    connect( this, SIGNAL( enableAction( const char * , bool ) ),
             SLOT( slotEnableAction( const char * , bool ) ) );
    connect( this, SIGNAL(itemRenamed(QIconViewItem*)),
             SLOT( slotItemRenamed(QIconViewItem*)) );
    connect( this, SIGNAL( dropped( QDropEvent *, const QValueList<QIconDragItem> & ) ),
             this, SLOT( slotSaveDropPosition( QDropEvent *, const QValueList<QIconDragItem> & ) ) );
}

KDIconView::~KDIconView()
{
  delete m_dirLister;
}

void KDIconView::initConfig( bool init )
{
    if ( !init )
        KonqFMSettings::reparseConfiguration();

    KConfig * config = KGlobal::config();
    config->setGroup( "Desktop Icons" );
    m_bShowDot = config->readBoolEntry("ShowHidden", DEFAULT_SHOW_HIDDEN_ROOT_ICONS);
    m_bVertAlign = config->readBoolEntry("VertAlign", DEFAULT_VERT_ALIGN);
    QStringList oldPreview = m_preview;
    m_preview = config->readListEntry( "Preview" );

    // read arrange configuration
    m_eSortCriterion  = (SortCriterion)config->readNumEntry("SortCriterion", NameCaseInsensitive);
    m_bSortDirectoriesFirst = config->readBoolEntry("DirectoriesFirst", true);
    m_itemsAlwaysFirst = config->readListEntry("AlwaysFirstItems"); // Distributor plug-in

    if ( m_dirLister ) // only when called while running - not on first startup
        m_dirLister->setShowingDotFiles( m_bShowDot );

    m_tAlign = m_bVertAlign ? TopToBottom : LeftToRight;
    setArrangement(m_tAlign);

    KonqIconViewWidget::initConfig( init );

    setAutoArrange( false );

    if ( m_preview.count() )
    {
        for ( QStringList::ConstIterator it = oldPreview.begin(); it != oldPreview.end(); ++it)
            if ( !m_preview.contains( *it ) ){
                kdDebug(1204) << "Disabling preview for " << *it << endl;
                setIcons( iconSize(), (*it).latin1() /* revert no-longer wanted previews to icons */ );
            }
        startImagePreview( m_preview, true );
    }
    else
    {
        stopImagePreview();
        setIcons( iconSize(), "" /* stopImagePreview */ );
    }

    if ( !init )
        updateContents();
}

void KDIconView::start()
{
    // We can only start once
    ASSERT(!m_dirLister);
    if (m_dirLister)
        return;

//    kdDebug(1204) << "KDIconView::start" << endl;

    // Create the directory lister
    m_dirLister = new KonqDirLister();

    connect( m_dirLister, SIGNAL( clear() ), this, SLOT( slotClear() ) );
    connect( m_dirLister, SIGNAL( started(const QString&) ),
             this, SLOT( slotStarted(const QString&) ) );
    connect( m_dirLister, SIGNAL( completed() ), this, SLOT( slotCompleted() ) );
    connect( m_dirLister, SIGNAL( newItems( const KFileItemList & ) ),
             this, SLOT( slotNewItems( const KFileItemList & ) ) );
    connect( m_dirLister, SIGNAL( deleteItem( KFileItem * ) ),
             this, SLOT( slotDeleteItem( KFileItem * ) ) );
    connect( m_dirLister, SIGNAL( refreshItems( const KFileItemList & ) ),
             this, SLOT( slotRefreshItems( const KFileItemList & ) ) );

    // Start the directory lister !
    m_dirLister->openURL( m_url, m_bShowDot );

    // Gather the list of directories to merge into the desktop
    // (the main URL is desktopURL(), no need for it in the m_mergeDirs list)
    m_mergeDirs.clear();
    QStringList dirs = KGlobal::dirs()->findDirs( "appdata", "Desktop" );
    for ( QStringList::ConstIterator it = dirs.begin() ; it != dirs.end() ; ++it )
    {
        kdDebug(1204) << "KDIconView::start found merge dir " << *it << endl;
        KURL u;
        u.setPath( *it );
        m_mergeDirs.append( u );
        // And start listing this dir right now
        m_dirLister->openURL( u, m_bShowDot, true );
    }

    createActions();
}

void KDIconView::createActions()
{
    KAction *undo = KStdAction::undo( KonqUndoManager::self(), SLOT( undo() ), &m_actionCollection, "undo" );
    connect( KonqUndoManager::self(), SIGNAL( undoAvailable( bool ) ),
             undo, SLOT( setEnabled( bool ) ) );
    connect( KonqUndoManager::self(), SIGNAL( undoTextChanged( const QString & ) ),
             undo, SLOT( setText( const QString & ) ) );
    undo->setEnabled( KonqUndoManager::self()->undoAvailable() );

    KStdAction::cut( this, SLOT( slotCut() ), &m_actionCollection, "cut" );
    KStdAction::copy( this, SLOT( slotCopy() ), &m_actionCollection, "copy" );
    KStdAction::paste( this, SLOT( slotPaste() ), &m_actionCollection, "paste" );

    (void) new KAction( i18n( "&Rename" ), /*"editrename",*/ Key_F2, this, SLOT( renameSelectedItem() ), &m_actionCollection, "rename" );
    (void) new KAction( i18n( "&Move to Trash" ), "edittrash", Key_Delete, this, SLOT( slotTrash() ), &m_actionCollection, "trash" );
    (void) new KAction( i18n( "&Delete" ), "editdelete", SHIFT+Key_Delete, this, SLOT( slotDelete() ), &m_actionCollection, "del" );

    (void) new KAction( i18n( "&Shred" ), "editshred", CTRL+SHIFT+Key_Delete, this, SLOT( slotShred() ), &m_actionCollection, "shred" );

    // Make the accels for all those actions available, through KAccel.
    m_accel = new KAccel( this );
    int count = actionCollection()->count();
    for ( int i = 0; i < count; i++ )
    {
        KAction *act = actionCollection()->action( i );
        if ( act->accel() )
            act->plugAccel( m_accel );
    }

    // Initial state of the actions (cut/copy/paste/...)
    slotSelectionChanged();
}

void KDIconView::rearrangeIcons( SortCriterion sc, bool bSortDirectoriesFirst )
{
    m_eSortCriterion = sc;
    m_bSortDirectoriesFirst = bSortDirectoriesFirst;
    rearrangeIcons();
}

void KDIconView::rearrangeIcons()
{
    setupSortKeys();
    sort();
    arrangeItemsInGrid();
    slotSaveIconPositions();
}

void KDIconView::lineupIcons()
{
    KonqIconViewWidget::lineupIcons();
    slotSaveIconPositions();
}

QStringList KDIconView::selectedURLs()
{
    QStringList seq;

    QIconViewItem *it = firstItem();
    for (; it; it = it->nextItem() )
        if ( it->isSelected() ) {
            KFileItem *fItem = ((KFileIVI *)it)->item();
            seq.append( fItem->url().url() ); // copy the URL
        }

    return seq;
}

void KDIconView::recheckDesktopURL()
{
    // Did someone change the path to the desktop ?
    kdDebug(1204) << desktopURL().url() << endl;
    kdDebug(1204) << m_url.url() << endl;
    if ( desktopURL() != m_url )
    {
        kdDebug(1204) << "Desktop path changed from " << m_url.url() <<
            " to " << desktopURL().url() << endl;
        setURL( desktopURL() ); // sets m_url and m_dotDirectoryPath
        delete m_dotDirectory;
        m_dotDirectory = 0L;
        m_dirLister->openURL( m_url, m_bShowDot );
    }
}

KURL KDIconView::desktopURL()
{
    // Support both paths and URLs
    QString desktopPath = KGlobalSettings::desktopPath();
    if (kdesktop_screen_number != 0) {
        QString dn = "Desktop";
        dn += QString::number(kdesktop_screen_number);
        desktopPath.replace(QRegExp("Desktop"), dn);
    }

    KURL desktopURL;
    if (desktopPath[0] == '/')
        desktopURL.setPath(desktopPath);
    else
        desktopURL = desktopPath;

    ASSERT( !desktopURL.isMalformed() );
    if ( desktopURL.isMalformed() ) // should never happen
        return QDir::homeDirPath() + "/" + "Desktop" + "/";

    return desktopURL;
}

// This is only emited when you click on the scrollview, not the
// viewport. We only need that if you have docks on the desktop which don't
// stretch over a whole height or width, like the kasbar was. Else you will
// never be able to click on the scrollview itself but only in the viewport.
void KDIconView::mousePressEvent( QMouseEvent *e )
{
//    kdDebug(1204) << "KDIconView::mousePressEvent" << endl;
    slotMouseButtonPressed( e->button(), 0, e->globalPos() );
}

void KDIconView::contentsMousePressEvent( QMouseEvent *e )
{
    if (!m_dirLister) return;
    //kdDebug(1204) << "KDIconView::contentsMousePressEvent" << endl;
    // QIconView, as of Qt 2.2, doesn't emit mouseButtonPressed for LMB on background
    if ( e->button() == LeftButton && KRootWm::getRootWm()->hasLeftButtonMenu() )
    {
        QIconViewItem *item = findItem( e->pos() );
        if ( !item )
        {
            // Left click menu
            KRootWm::getRootWm()->mousePressed( e->globalPos(), e->button() );
            return;
        }
    }
    KonqIconViewWidget::contentsMousePressEvent( e );
}

void KDIconView::slotMouseButtonPressed(int _button, QIconViewItem* _item, const QPoint& _global)
{
    //kdDebug(1204) << "KDIconView::slotMouseButtonPressed" << endl;
    if (!m_dirLister) return;
    m_lastIcon = 0L; // user action -> not renaming an icon
    if(_item) {
        if ( _button == RightButton )
        {
            ((KFileIVI*)_item)->setSelected( true );
            popupMenu( _global, selectedFileItems() );
        }
    }
    else
        KRootWm::getRootWm()->mousePressed( _global, _button );
}

void KDIconView::slotMouseButtonClickedKDesktop(int _button, QIconViewItem* _item, const QPoint&)
{
    if (!m_dirLister) return;
    //kdDebug(1204) << "KDIconView::slotMouseButtonClickedKDesktop" << endl;
    if ( _item && _button == MidButton )
        slotReturnPressed( _item );
}

// -----------------------------------------------------------------------------

void KDIconView::slotReturnPressed( QIconViewItem *item )
{
  kapp->propagateSessionManager();
  m_lastIcon = 0L; // user action -> not renaming an icon
  if (item) {
    visualActivate(item);
    ((KFileIVI*)item)->returnPressed();
  }
}

// -----------------------------------------------------------------------------

void KDIconView::slotCut()
{
  cutSelection();
}

// -----------------------------------------------------------------------------

void KDIconView::slotCopy()
{
  copySelection();
}

// -----------------------------------------------------------------------------

void KDIconView::slotPaste()
{
  pasteSelection();
}

/* for paste on desktop
  void KRootWm::slotPaste() {
    // perhaps a hack. But required by pasteSelection() currently
    m_pDesktop->unselectAll();
    m_pDesktop->slotPaste();
}*/

void KDIconView::slotTrash()
{
    KonqOperations::del(this, KonqOperations::TRASH, selectedUrls());
}

void KDIconView::slotDelete()
{
    KonqOperations::del(this, KonqOperations::DEL, selectedUrls());
}

void KDIconView::slotShred()
{
    KonqOperations::del(this, KonqOperations::SHRED, selectedUrls());
}

// -----------------------------------------------------------------------------

void KDIconView::popupMenu( const QPoint &_global, KFileItemList _items )
{
    if (!m_dirLister) return;
    KonqPopupMenu * popupMenu = new KonqPopupMenu( _items,
                                                   m_url,
                                                   m_actionCollection,
                                                   KRootWm::getRootWm()->newMenu() );

    popupMenu->exec( _global );
    delete popupMenu;
}

// -----------------------------------------------------------------------------

void KDIconView::slotEnableAction( const char * name, bool enabled )
{
  //kdDebug(1204) << "slotEnableAction " << name << " enabled=" << enabled << endl;
  QCString sName( name );
  // No such actions here... konqpopupmenu provides them.
  if ( sName == "properties" || sName == "editMimeType" )
    return;

  KAction * act = m_actionCollection.action( sName.data() );
  if (!act)
    kdWarning(1203) << "Unknown action " << sName.data() << " - can't enable" << endl;
  else
    act->setEnabled( enabled );
}

// -----------------------------------------------------------------------------

// Straight from kpropsdlg :)
bool KDIconView::isDesktopFile( KFileItem * _item ) const
{
  // only local files
  if ( !_item->isLocalFile() )
    return false;

  // only regular files
  if ( !S_ISREG( _item->mode() ) )
    return false;

  QString t( _item->url().path() );

  // only if readable
  if ( access( QFile::encodeName(t), R_OK ) != 0 )
    return false;

  // return true if desktop file
  return ( _item->mimetype() == QString::fromLatin1("application/x-desktop") );
}

QString KDIconView::stripDesktopExtension( const QString & text )
{
    if (text.right(7) == QString::fromLatin1(".kdelnk"))
      return text.left(text.length() - 7);
    else if (text.right(8) == QString::fromLatin1(".desktop"))
      return text.left(text.length() - 8);
    return text;
}

void KDIconView::makeFriendlyText( KFileIVI *fileIVI )
{
    KFileItem *item = fileIVI->item();
    QString desktopFile;
    if ( item->isDir() && item->isLocalFile() )
    {
        KURL u( item->url() );
        u.addPath( ".directory" );
        // using KStandardDirs as this one checks for path beeing
        // a file instead of a directory
        if ( KStandardDirs::exists( u.path() ) )
            desktopFile = u.path();
    }
    else if ( isDesktopFile( item ) )
    {
        desktopFile = item->url().path();
    }

    if ( !desktopFile.isEmpty() )
    {
        KSimpleConfig cfg( desktopFile, true );
        cfg.setDesktopGroup();
        QString name = cfg.readEntry("Name");
        if ( !name.isEmpty() )
            fileIVI->setText( name );
        else
            // For compatibility
            fileIVI->setText( stripDesktopExtension( fileIVI->text() ) );
    }
}

// -----------------------------------------------------------------------------

void KDIconView::slotClear()
{
    clear();
}

// -----------------------------------------------------------------------------

void KDIconView::slotNewItems( const KFileItemList & entries )
{
  // We have new items, so we'll need to repaint in slotCompleted
  m_bNeedRepaint = true;
//  kdDebug(1204) << "KDIconView::slotNewItems count=" << entries.count() << endl;
  KFileItemListIterator it(entries);
  KFileIVI* fileIVI = 0L;
  for (; it.current(); ++it)
  {
    fileIVI = new KFileIVI( this, static_cast<KonqFileItem *>(it.current()),
                            iconSize() );
    makeFriendlyText( fileIVI );

//    kdDebug(1204) << " slotNewItems: " << it.current()->url().url() << " slotNewItems: " << fileIVI->text() << endl;
    fileIVI->setRenameEnabled( false );

    if ( m_dotDirectory )
    {
      QString group = m_iconPositionGroupPrefix;
      QString filename = it.current()->url().fileName();
      if ( filename.right(5) == ".part" && !m_dotDirectory->hasGroup( group + filename ) )
          filename = filename.left( filename.length() - 5 );
      group.append( filename );
      //kdDebug(1204) << "slotNewItems : looking for group " << group << endl;
      if ( m_dotDirectory->hasGroup( group ) )
      {
        m_dotDirectory->setGroup( group );
        m_hasExistingPos = true;
        int x = m_dotDirectory->readNumEntry( "X" );
        int y = m_dotDirectory->readNumEntry( "Y" );

        QRect oldPos = fileIVI->rect();
        fileIVI->move( x, y );
        if ( !isFreePosition( fileIVI ) ) // if we can't put it there, then let QIconView decide
        {
            fileIVI->move( oldPos.x(), oldPos.y() );
            m_dotDirectory->deleteGroup( group );
        }
      }
      else // Not found, we'll need to save the new pos
          m_bNeedSave = true;
    }
  }
  if (fileIVI)
    m_lastIcon = fileIVI;
}

// -----------------------------------------------------------------------------

// see also KonqKfmIconView::slotRefreshItems
void KDIconView::slotRefreshItems( const KFileItemList & entries )
{
//    kdDebug(1204) << "KDIconView::slotRefreshItems" << endl;
    KFileItemListIterator rit(entries);
    for (; rit.current(); ++rit)
    {
        QIconViewItem *it = firstItem();
        for ( ; it ; it = it->nextItem() )
        {
            KFileIVI * fileIVI = static_cast<KFileIVI *>(it);
            if ( fileIVI->item() == rit.current() ) // compare the pointers
            {
//                kdDebug(1204) << "KDIconView::slotRefreshItems refreshing icon " << fileIVI->item()->url().url() << endl;
                fileIVI->refreshIcon( true );
                makeFriendlyText( fileIVI );
                break;
            }
        }
    }
    // In case we replace a big icon with a small one, need to repaint.
    updateContents();
    // Can't do that with m_bNeedRepaint since slotCompleted isn't called
}


void KDIconView::refreshIcons()
{
    QIconViewItem *it = firstItem();
    for ( ; it ; it = it->nextItem() )
    {
        KFileIVI * fileIVI = static_cast<KFileIVI *>(it);
        fileIVI->refreshIcon( true );
        makeFriendlyText( fileIVI );
    }
}


// -----------------------------------------------------------------------------

void KDIconView::slotDeleteItem( KFileItem * _fileitem )
{
//    kdDebug(1204) << "KDIconView::slotDeleteItems" << endl;
    // we need to find out the KFileIVI containing the fileitem
    QIconViewItem *it = firstItem();
    while ( it ) {
      KFileIVI * fileIVI = static_cast<KFileIVI *>(it);
      if ( fileIVI->item() == _fileitem ) { // compare the pointers
        // Delete this item.
        //kdDebug(1204) << fileIVI->text() << endl;
        // It may be that it has been renamed. In this case,
        // m_lastIcon should be moved to this icon's position.
        // (We rely on newItems being emitted before deleteItem)
        if ( m_lastIcon )
          // Problem is: I'd like to compare those two file's attributes
          // (size, creation time, modification time... etc.) but since renaming
          // is done by kpropsdlg, all of those can have changed (and creation time
          // is different since the new file is a copy!)
        {
          kdDebug(1204) << "moving " << m_lastIcon->text() << endl;
          m_lastIcon->move( fileIVI->x(), fileIVI->y() );
          m_lastIcon = 0L;
        }

        if ( m_dotDirectory )
        {
            QString group = m_iconPositionGroupPrefix;
            group.append( fileIVI->item()->url().fileName() );
            if ( m_dotDirectory->hasGroup( group ) )
                m_dotDirectory->deleteGroup( group );
        }

        delete fileIVI;
        break;
      }
      it = it->nextItem();
    }
    m_bNeedRepaint = true;
}

// -----------------------------------------------------------------------------

void KDIconView::slotStarted( const QString& url )
{
//    kdDebug(1204) << "KDIconView::slotStarted " << url << endl;
    // main directory only
    if ( url == m_url.url() )
    {
        m_dotDirectory = new KSimpleConfig( m_dotDirectoryPath, true );
        m_bNeedSave = false;
        m_bNeedRepaint = false;
    }
}

void KDIconView::slotCompleted()
{
    // Root item ? Store in konqiconviewwidget (used for drops onto the background, for instance)
    if ( m_dirLister->rootItem() )
      setRootItem( static_cast<KonqFileItem *>(m_dirLister->rootItem()) );

    if ( m_dotDirectory )
    {
      delete m_dotDirectory;
      m_dotDirectory = 0;
    }

    if ( m_preview.count() )
        startImagePreview( m_preview, true );
    else
    {
        stopImagePreview();
        setIcons( iconSize(), "" /* stopImagePreview */ );
    }

    // during first run need to rearrange all icons so default config settings will be used
    if (!m_hasExistingPos)
        rearrangeIcons();

//    kdDebug(1204) << "KDIconView::slotCompleted save:" << m_bNeedSave << " repaint:" << m_bNeedRepaint << endl;
    if ( m_bNeedSave )
    {
        slotSaveIconPositions();
        m_hasExistingPos = true; // if we didn't have positions, we have now.
    }
    if ( m_bNeedRepaint )
        viewport()->repaint();
}

void KDIconView::slotClipboardDataChanged()
{
    // This is very related to KonqDirPart::slotClipboardDataChanged

    KURL::List lst;
    QMimeSource *data = QApplication::clipboard()->data();
    if ( data->provides( "application/x-kde-cutselection" ) && data->provides( "text/uri-list" ) )
        if ( KonqDrag::decodeIsCutSelection( data ) )
            (void) KURLDrag::decode( data, lst );

    disableIcons( lst );

    bool paste = ( data->encodedData( data->format() ).size() != 0 );
    slotEnableAction( "paste", paste );
}

void KDIconView::slotItemRenamed(QIconViewItem* _item)
{
//    kdDebug(1204) << "KDIconView::slotItemRenamed(item)" << endl;
    if ( !_item )
      return;

    KFileIVI *fileItem = static_cast< KFileIVI* >( _item );
    if ( !fileItem->item() )
      return;

    // first and foremost, we make sure that this is a .desktop file
    // before we write anything to it
    KMimeType::Ptr type = KMimeType::findByURL( fileItem->item()->url() );
    if ( type->name() != "application/x-desktop" )
      return;

    QString desktopFile( fileItem->item()->url().path() );
    if ( desktopFile.isEmpty() )
      return;

    KDesktopFile cfg( desktopFile, false );

    // if we don't have the desktop entry group, then we assume that
    // it's not a config file (and we don't nuke it!)
    if ( !cfg.hasGroup( "Desktop Entry" ) )
      return;

    if ( cfg.readName() == _item->text() )
      return;

    cfg.writeEntry( "Name", _item->text(), true, false, true );
    cfg.sync();
}

void KDIconView::slotSaveDropPosition( QDropEvent *ev, const QValueList<QIconDragItem> & )
{
    m_lastIcon = 0L;
    if (!m_dirLister) return; // too early
    if (m_dotDirectory) return; // we are listing the dir...
    m_dotDirectory = new KSimpleConfig( m_dotDirectoryPath );
    if (ev->provides( "text/uri-list" ))
    {
        KURL::List lst;
        if ( KURLDrag::decode( ev, lst ) ) // Are they urls ?
        {
            // For now, deal with only one icon
            // TODO: if we can decode as application/x-qiconlist then we
            // can even store the position of each icon (to keep their relative position)
            if ( lst.count() == 1 )
            {
                KURL u = lst.first();
                int x = ev->pos().x() - gridXValue()/2;
                int y = ev->pos().y() - (firstItem() ? firstItem()->height()/2 : 20);
                kdDebug(1204) << "Saving drop position for " << u.fileName() << " at " << x << "," << y << endl;
                m_dotDirectory->setGroup( QString( m_iconPositionGroupPrefix ).append( u.fileName() ) );
                m_dotDirectory->writeEntry( "X", x );
                m_dotDirectory->writeEntry( "Y", y );
            }
        }
    }
    m_dotDirectory->sync();
    delete m_dotDirectory;
    m_dotDirectory = 0L;
}

// -----------------------------------------------------------------------------

void KDIconView::showEvent( QShowEvent *e )
{
    //HACK to avoid QIconView calling arrangeItemsInGrid (Simon)
    //EVEN MORE HACK: unfortunately, QScrollView has no concept of
    //TopToBottom, therefore, it always adds LeftToRight.  So, if any of
    //the icons have a setting, we'll use QScrollView.. but otherwise,
    //we use the iconview
    if (m_hasExistingPos)
        QScrollView::showEvent( e );
    else
        KIconView::showEvent( e );
}

void KDIconView::dropEvent( QDropEvent * e )
{
    // mind: if it's a filedrag which itself is an image, libkonq is called. There's a popup for drops as well
    // that contains the same line "Set as Wallpaper" in void KonqOperations::asyncDrop
    bool isColorDrag = KColorDrag::canDecode(e);
    bool isImageDrag = QImageDrag::canDecode(e);

    if ( isColorDrag )
        emit colorDropEvent( e );
    else if ( isImageDrag )
        emit imageDropEvent( e );
    else
        KonqIconViewWidget::dropEvent( e );
}

// don't scroll when someone uses his nifty mouse wheel
void KDIconView::viewportWheelEvent( QWheelEvent * e )
{
    e->ignore();
}

void KDIconView::updateWorkArea( const QRect &wr )
{
//    kdDebug(1204) << "KDIconView::updateWorkArea wr: " << wr.x() << "," << wr.y()
//              << " " << wr.width() << "x" << wr.height() << endl;
    setMargins( wr.left(), wr.top(),
                QApplication::desktop()->width() - wr.right() - 1,
                QApplication::desktop()->height() - wr.bottom() - 1 );
    resizeContents( viewport()->width(), viewport()->height() );
//    kdDebug(1204) << "resizeContents " << viewport()->width() << "x" << viewport()->height() << endl;

    for ( QIconViewItem *item = firstItem(); item; item = item->nextItem() ) {
        QRect r( item->rect() );
        int dx = 0, dy = 0;
        if ( r.bottom() > visibleHeight() )
            dy = visibleHeight() - r.bottom() - 1;
        if ( r.right() > visibleWidth() )
            dx = visibleWidth() - r.right() - 1;
        if ( dx != 0 || dy != 0 )
            item->moveBy( dx, dy );
    }

    viewport()->repaint( FALSE );
    repaint( FALSE );
}

void KDIconView::setupSortKeys()
{
    // can't use sorting in KFileIVI::setKey()
    setProperty("sortDirectoriesFirst", QVariant(false, 0));

    for (QIconViewItem *it = firstItem(); it; it = it->nextItem())
    {
        QString strKey;

        if (!m_itemsAlwaysFirst.isEmpty())
        {
            QString strFileName = static_cast<KFileIVI *>( it )->item()->url().fileName();
            int nFind = m_itemsAlwaysFirst.findIndex(strFileName);
            if (nFind >= 0)
                strKey = "0" + QString::number(nFind);
        }

        if (strKey.isEmpty())
        {
            switch (m_eSortCriterion)
            {
            case NameCaseSensitive:
                strKey = it->text();
                break;
            case NameCaseInsensitive:
                strKey = it->text().lower();
                break;
            case Size:
                strKey = QString::number(static_cast<KFileIVI *>( it )->item()->size()).rightJustify(20, '0');
                break;
            case Type:
                // Sort by Type + Name (#17014)
                strKey = static_cast<KFileIVI *>( it )->item()->mimetype() + '~' + it->text().lower();
                break;
            }

            if (m_bSortDirectoriesFirst)
            {
                if (S_ISDIR(static_cast<KFileIVI *>( it )->item()->mode()))
                    strKey.prepend(sortDirection() ? '1' : '2');
                else
                    strKey.prepend(sortDirection() ? '2' : '1' );
            }
            else
                strKey.prepend('1');
        }

        it->setKey(strKey);
    }
}

bool KDIconView::isFreePosition( const QIconViewItem *item )
{
    QRect r = item->rect();
    QIconViewItem *it = firstItem();
    for (; it; it = it->nextItem() )
    {
        if ( !it->rect().isValid() || it == item )
            continue;

        if ( it->intersects( r ) )
            return false;
    }

    return true;
}

#include "kdiconview.moc"
