/* vi: ts=8 sts=4 sw=4
 *
 * $Id: pixmapserver.h,v 1.1 1999/12/18 14:58:06 jansen Exp $
 *
 * This file is part of the KDE project, module kdesktop.
 * Copyright (C) 1999 Geert Jansen <g.t.jansen@stud.tue.nl>
 * 
 * You can Freely distribute this program under the GNU General Public
 * License. See the file "COPYING" for the exact licensing terms.
 */

#ifndef __PixmapServer_h_Included__
#define __PixmapServer_h_Included__

#include <qwindowdefs.h>

#include <qwidget.h>
#include <qmap.h>

#include <X11/X.h>
#include <X11/Xlib.h>

/**
 * Used internally by KPixmapServer.
 */

struct KPixmapInode
{
    HANDLE handle;
    Atom selection;
};

struct KPixmapData
{
    QPixmap *pixmap;
    int usecount;
    int refcount;
};

struct KSelectionInode
{
    HANDLE handle;
    QString name;
};

/**
 * KPixmapServer: Share pixmaps between X clients with deletion and
 * multi-server capabilities. 
 * The sharing is implemented using X11 Selections.
 *
 * @author Geert Jansen <g.t.jansen@stud.tue.nl>
 */
class KPixmapServer: public QWidget
{
    Q_OBJECT

public:
    KPixmapServer();
    ~KPixmapServer();

    /**
     * Adds a pixmap to this server. This will make it available to all
     * other X clients on the current display.
     *
     * You must never delete a pixmap that you add()'ed. The pixmap is 
     * deleted when you call remove() and after all clients have stopped 
     * using it.
     *
     * You can add the same pixmap under multiple names.
     *
     * @param name An X11-wide unique identifier for the pixmap.
     * @param pm A pointer to the pixmap.
     * @param overwrite Should an pixmap with the same name be overwritten?
     */
    void add(QString name, QPixmap *pm, bool overwrite=true);

    /**
     * Remove a pixmap from the server. This will delete the pixmap after 
     * all clients have stopped using it.
     *
     * @param name The name of the shared pixmap.
     */
    void remove(QString name);

    /**
     * List all pixmaps currently served by this server.
     * 
     * @return A QStringList containing all the shared pixmaps.
     */
    QStringList list();

    /**
     * Re-set ownership of the selection providing the shared pixmap.
     *
     * @param name The name of the shared pixmap.
     */
    void setOwner(QString name);

signals:
    /** 
     * This signal is emitted when the selection providing the named pixmap
     * is disowned. This means that said pixmap won't be served anymore by 
     * this server, though it can be served by another. You can re-aqcuire
     * the selection by calling setOwner().
     */
    void selectionCleared(QString name);

protected:
    bool x11Event(XEvent *);

private:
    Atom pixmap;

    QMap<QString,KPixmapInode> m_Names;
    QMap<Atom,KSelectionInode> m_Selections;
    QMap<HANDLE,KPixmapData> m_Data;
    QMap<Atom,HANDLE> m_Active;

    typedef QMap<QString,KPixmapInode>::Iterator NameIterator;
    typedef QMap<Atom,KSelectionInode>::Iterator SelectionIterator;
    typedef QMap<HANDLE,KPixmapData>::Iterator DataIterator;
    typedef QMap<Atom,HANDLE>::Iterator AtomIterator;
};


#endif // __PixmapServer_h_Included__
