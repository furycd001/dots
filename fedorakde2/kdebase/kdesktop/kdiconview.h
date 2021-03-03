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

#ifndef kdiconview_h
#define kdiconview_h

#include <konq_iconviewwidget.h>
#include <kaction.h>

class KonqDirLister;
class KonqSettings;
class KSimpleConfig;
class KAccel;

/**
 * This class is KDesktop's icon view.
 * The icon view is a child widget of the KDesktop widget.
 */
class KDIconView : public KonqIconViewWidget
{
    Q_OBJECT
public:
    KDIconView( QWidget *parent, const char* name = 0L );
    ~KDIconView();

    virtual void initConfig( bool init );

    /**
     * Start listing
     */
    void start();

    KActionCollection *actionCollection() { return &m_actionCollection; }

    enum SortCriterion { NameCaseSensitive = 0, NameCaseInsensitive, Size, Type };

    void rearrangeIcons( SortCriterion sc, bool bSortDirectoriesFirst);

    /**
     * Re-arrange the desktop icons without confirmation.
     */
    void rearrangeIcons();
    /**
     * Lineup the desktop icons.
     */
    void lineupIcons();

    void refreshIcons();
    QStringList selectedURLs();

    /**
     * Check if the URL to the desktop has changed
     */
    void recheckDesktopURL();

    /**
     * Called when the work area has changed
     */
    void updateWorkArea( const QRect &wr );

    /**
     * Reimplemented from KonqIconViewWidget (for image drops)
     */
    virtual void setWallpaper(const KURL &url) { emit newWallpaper( url ); }

protected slots:
    // slots connected to the icon view
    void slotReturnPressed( QIconViewItem *item );
    void slotMouseButtonPressed(int _button, QIconViewItem* _item, const QPoint& _global);
    void slotMouseButtonClickedKDesktop(int _button, QIconViewItem* _item, const QPoint& _global);
    void slotEnableAction( const char * name, bool enabled );
    void slotSaveDropPosition( QDropEvent *ev, const QValueList<QIconDragItem> & );

    void slotItemRenamed(QIconViewItem*);

    // slots connected to the directory lister
    void slotClear();
    void slotStarted( const QString& url );
    void slotCompleted();
    void slotNewItems( const KFileItemList& );
    void slotDeleteItem( KFileItem * );
    void slotRefreshItems( const KFileItemList& );

    // slots connected to the popupmenu (actions)
    void slotCut();
    void slotCopy();
    void slotTrash();
    void slotDelete();
    void slotShred();

    void slotClipboardDataChanged();

    // For communication with KDesktop
signals:
    void colorDropEvent( QDropEvent *e );
    void imageDropEvent( QDropEvent *e );
    void newWallpaper( const KURL & );

public slots: // for krootwm
    void slotPaste();

protected:
    void createActions();
    void setupSortKeys();
    KURL desktopURL();

    void makeFriendlyText( KFileIVI *fileIVI );
    static QString stripDesktopExtension( const QString & text );
    bool isDesktopFile( KFileItem * _item ) const;
    bool isFreePosition( const QIconViewItem *item );

    virtual void popupMenu( const QPoint &_global, KFileItemList _items );
    virtual void showEvent( QShowEvent *e );
    virtual void dropEvent( QDropEvent *e );
    virtual void viewportWheelEvent( QWheelEvent * );
    virtual void mousePressEvent( QMouseEvent *e );
    virtual void contentsMousePressEvent( QMouseEvent *e );

private:

    /** Our action collection, parent of all our actions */
    KActionCollection m_actionCollection;

    /** KAccel object, to make the actions shortcuts work */
    KAccel *m_accel;

    bool m_bNeedRepaint;
    bool m_bNeedSave;

    /** true if even one icon has an icon-position entry in the .directory */
    bool m_hasExistingPos;

    /** Show dot files ? */
    bool m_bShowDot;

    /** Vertical or Horizontal align of icons on desktop */
    bool m_bVertAlign;

    QIconView::Arrangement m_tAlign;

    /** Settings - see libkonq/konqsettings.* */
    KonqSettings* m_pSettings;

    /** The directory lister - created only in start() */
    KonqDirLister* m_dirLister;

    /** The list of dirs to be merged into the desktop, in addition to desktopURL */
    KURL::List m_mergeDirs;

    /** The desktop's .directory, used for storing icon positions */
    KSimpleConfig *m_dotDirectory;

    /** Last added icon - used when renaming a file */
    KFileIVI * m_lastIcon;

    /** List of active previews (mimetypes) */
    QStringList m_preview;

    /** Sorting */
    SortCriterion m_eSortCriterion;
    bool m_bSortDirectoriesFirst;
    QStringList m_itemsAlwaysFirst;
};

#endif
