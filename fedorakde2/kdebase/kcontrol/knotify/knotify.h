/*
    $Id: knotify.h,v 1.17 2001/04/20 00:30:40 malte Exp $

    Copyright (C) Charles Samuels <charles@altair.dhs.org>
                  2000 Carsten Pfeiffer <pfeiffer@kde.org>

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.

*/


#ifndef _KNOTIFY_H
#define _KNOTIFY_H

#include <qcheckbox.h>
#include <qlistview.h>
#include <qstring.h>
#include <qtooltip.h>

#include <kcmodule.h>

#include "events.h"

class QSlider;
class KAboutData;
class KNCheckListItem;
class KURLRequester;

class KNotifyWidget : public KCModule
{
    Q_OBJECT

public:
    KNotifyWidget(QWidget *parent, const char *name);
    virtual ~KNotifyWidget();

    void defaults();
    virtual void save();
    virtual QString quickHelp() const;
    virtual const KAboutData *aboutData() const;

public slots:
    virtual void load();

private slots:
    void slotRequesterClicked( KURLRequester * );
    void externalClicked( bool on );
    void changed();

    void slotItemActivated( QListViewItem * );
    void slotFileChanged( const QString& text );
    void playSound();

private:
    void updateView();

    QCheckBox *cbExternal;
    QListView *view;
    KURLRequester *requester, *reqExternal;
    QPushButton *playButton;
    QSlider *volumeSlider;
    Events *m_events;
    KNCheckListItem *currentItem;
    bool updating;
};

class KNListViewItem : public QObject, public QListViewItem
{
    Q_OBJECT

public:
    KNListViewItem( QListViewItem *parent, QListViewItem *after, KNEvent *e );
    void itemChanged( KNCheckListItem * );

signals:
    void changed();
    void soundActivated( KNEvent * );
    void logActivated( KNEvent * );
    void otherActivated( KNEvent * );

private:
    KNEvent *event;
    KNCheckListItem *stderrItem, *msgboxItem, *soundItem, *logItem;

};


class KNCheckListItem : public QCheckListItem
{
public:
    KNCheckListItem( QListViewItem *parent, KNEvent *e, int type,
		     const QString& text );
    int eventType() const { return _eventType; }
    KNEvent *event;

protected:
    virtual void stateChange( bool on );
    const int _eventType;
};


#endif
