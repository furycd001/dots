/* This file is part of the KDE project
   Copyright (C) 2000 Simon Hausmann <hausmann@kde.org>

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

#ifndef __konq_actions_h__
#define __konq_actions_h__ "$Id: konq_actions.h,v 1.36 2001/04/28 08:21:23 hausmann Exp $"

#include <qguardedptr.h>
#include <konq_historymgr.h>

#include <kaction.h>
#include <qlist.h>

class KBookmarkOwner;
class KonqCombo;
class HistoryEntry;
class QLabel;
class QPopupMenu;

class KonqComboAction : public KAction
{
  Q_OBJECT
public:
    KonqComboAction( const QString& text, int accel, const QObject *receiver, const char *member, QObject* parent, const char* name );
    ~KonqComboAction();

    virtual int plug( QWidget *w, int index = -1 );

    virtual void unplug( QWidget *w );

    QGuardedPtr<KonqCombo> combo() { return m_combo; }

signals:
    void plugged();

private:
    QGuardedPtr<KonqCombo> m_combo;
    const QObject *m_receiver;
    const char *m_member;
};

/**
 * Plug this action into a menu to get a bidirectional history
 * (both back and forward, including current location)
 */
class KonqBidiHistoryAction : public KAction
{
  Q_OBJECT
public:
    KonqBidiHistoryAction( const QString & text, QObject* parent = 0, const char* name = 0 );

    virtual ~KonqBidiHistoryAction() {};

    virtual int plug( QWidget *widget, int index = -1 );
    //virtual void unplug( QWidget *widget );

    void fillGoMenu( const QList<HistoryEntry> &history );

    // Used by KonqHistoryAction and KonqBidiHistoryAction
    static void fillHistoryPopup( const QList<HistoryEntry> &history,
                           QPopupMenu * popup,
                           bool onlyBack = false,
                           bool onlyForward = false,
                           bool checkCurrentItem = false,
                           uint startPos = 0 );

protected slots:
    void slotActivated( int );

signals:
    void menuAboutToShow();
    // -1 for one step back, 0 for don't move, +1 for one step forward, etc.
    void activated( int );
private:
    uint m_firstIndex; // first index in the Go menu
    int m_startPos;
    int m_currentPos; // == history.at()
    QPopupMenu *m_goMenu; // hack
};

/////

class KonqLogoAction : public KAction
{
  Q_OBJECT
public:
    KonqLogoAction( const QString& text, int accel = 0, QObject* parent = 0, const char* name = 0 );
    KonqLogoAction( const QString& text, int accel,
	            QObject* receiver, const char* slot, QObject* parent, const char* name = 0 );
    KonqLogoAction( const QString& text, const QIconSet& pix, int accel = 0,
	            QObject* parent = 0, const char* name = 0 );
    KonqLogoAction( const QString& text, const QIconSet& pix, int accel,
	            QObject* receiver, const char* slot, QObject* parent, const char* name = 0 );
    // text missing !
    KonqLogoAction( const QStringList& icons, QObject* receiver,
                    const char* slot, QObject* parent, const char* name = 0 );

    virtual int plug( QWidget *widget, int index = -1 );

    void start();
    void stop();

private:
    QStringList iconList;
};

class KonqLabelAction : public KAction
{
  Q_OBJECT
public:
  KonqLabelAction( const QString &text, QObject *parent = 0, const char *name = 0 );

  virtual int plug( QWidget *widget, int index = -1 );
  virtual void unplug( QWidget *widget );
  QLabel * label() { return m_label; }
private:
  QLabel * m_label;
};

class KonqViewModeAction : public KRadioAction
{
    Q_OBJECT
public:
    KonqViewModeAction( const QString &text, const QString &icon,
                        QObject *parent, const char *name );
    virtual ~KonqViewModeAction();

    virtual int plug( QWidget *widget, int index = -1 );

    QPopupMenu *popupMenu() const { return m_menu; }

private slots:
    void slotPopupAboutToShow();
    void slotPopupActivated();
    void slotPopupAboutToHide();

private:
    bool m_popupActivated;
    QPopupMenu *m_menu;
};

class MostOftenList : public KonqBaseHistoryList
{
protected:
    /**
     * Ensures that the items are sorted by numberOfTimesVisited
     */
    virtual int compareItems( QCollection::Item, QCollection::Item );
};

class KonqMostOftenURLSAction : public KActionMenu
{
    Q_OBJECT

public:
    KonqMostOftenURLSAction( const QString& text, QObject *parent,
			     const char *name );
    virtual ~KonqMostOftenURLSAction();

signals:
    void activated( const KURL& );

private slots:
    void slotHistoryCleared();
    void slotEntryAdded( const KonqHistoryEntry *entry );
    void slotEntryRemoved( const KonqHistoryEntry *entry );

    void slotFillMenu();
    void slotClearMenu();

    void slotActivated( int );

private:
    void parseHistory();

    static MostOftenList *s_mostEntries;
    static uint s_maxEntries;
    static bool s_bLocked;
};

class KonqGoURLAction : public KAction
{
public:
    KonqGoURLAction( const QString &text, const QString &pix, int accel,
                     const QObject *receiver, const char *slot,
                     QObject *parent, const char *name );

    virtual int plug( QWidget *widget, int index = -1 );
};

#endif
