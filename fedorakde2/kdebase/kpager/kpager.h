/**************************************************************************

    kpager.h  - KPager's main window
    Copyright (C) 2000  Antonio Larrosa Jimenez
			Matthias Ettrich
			Matthias Elter

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

    Send comments and bug fixes to larrosa@kde.org

***************************************************************************/
#ifndef __KPAGER_H
#define __KPAGER_H

#include <qwidget.h>
#include <kmainwindow.h>
#include <qintdict.h>
#include <kwin.h>

class KPager;
class QPopupMenu;

class KPagerMainWindow : public KMainWindow
{
    Q_OBJECT

public:
    KPagerMainWindow(QWidget *parent=0, const char *name=0);
    ~KPagerMainWindow();

protected:
    bool queryClose();

    KPager *m_pPager;
};

class KPager : public QFrame
{
    Q_OBJECT

    friend class KPagerMainWindow;

public:
    KPager(QWidget *parent=0, const char *name=0, WId parentWId = 0);
    ~KPager();

    class KWinModule *kwin() const { return m_winmodule; };
    void updateLayout();

    void redrawDesktops();

    void showPopupMenu( WId wid, QPoint pos);

    KWin::Info* info( WId win );

    bool isEmbedded() { return m_bIsEmbedded; };

    QSize sizeHint() const;

		enum LayoutTypes { Classical=0, Horizontal, Vertical };

public slots:
    void configureDialog();

    void slotActiveWindowChanged( WId win );
    void slotWindowAdded( WId );
    void slotWindowRemoved( WId );
    void slotWindowChanged( WId, unsigned int );
    void slotStackingOrderChanged();
    void slotDesktopNamesChanged();
    void slotNumberOfDesktopsChanged(int ndesktops);
    void slotCurrentDesktopChanged(int);

    void slotGrabWindows();

protected slots:
    void slotBackgroundChanged(int);
    void clientPopupAboutToShow();
    void clientPopupActivated(int);
    void desktopPopupAboutToShow();
    void sendToDesktop(int desk);

protected:
    enum WindowOperation {
        MaximizeOp = 100,
        IconifyOp,
        StickyOp,
        CloseOp
    };

protected:
    KWinModule *m_winmodule;
    QValueList<class Desktop *> m_desktops;

    QIntDict<KWin::Info> m_windows;
    WId m_activeWin;

    const QString lWidth();
    const QString lHeight();

    LayoutTypes m_layoutType;

    class QGridLayout *m_layout;
    KPopupMenu *m_mnu;
    QPopupMenu *m_smnu, *m_dmnu;
    KWin::Info m_winfo;
    bool m_showStickyOption; // To be removed after the message freeze

    QTimer *m_grabWinTimer;
    int     m_currentDesktop;

    bool m_bIsEmbedded;
public:
    static const LayoutTypes c_defLayout;
};

#endif
