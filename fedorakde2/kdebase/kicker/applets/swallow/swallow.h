/*
 *  Copyright (c) 2000 Matthias Hölzer-Klüpfel <hoelzer@kde.org>
                  2000 Carsten Pfeiffer <pfeiffer@kde.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 */

#ifndef __swallow_h__
#define __swallow_h__


#include <qevent.h>
#include <qlist.h>
#include <qxembed.h>

#include <kpanelapplet.h>

class QBoxLayout;
class KConfig;
class KProcess;
class KWinModule;

class SwallowApp;

typedef struct _SwallowCommand {
    QString cmdline;
    QString title;
} SwallowCommand;


class SwallowApplet : public KPanelApplet
{
    Q_OBJECT

public:
    SwallowApplet( const QString& configFile, QWidget *parent,
                          const char *name = 0L );
    ~SwallowApplet();

    // returns 0L if we don't have a SwallowApplet object yet,
    // but who cares
    static KWinModule * winModule() { return wModule; }
    static void removeApplet( SwallowApp * );

public: // for KPanelApplet
    int widthForHeight( int w );
    int heightForWidth( int h );

    void windowAdded(WId win);
    void processExited(KProcess *proc);

public slots:
    virtual void preferences();

private slots:
    void embedded( SwallowApp * );

private:
    void layoutApps();
    QList<SwallowCommand>* readConfig();
    void createApps( QList<SwallowCommand> * );


    static SwallowApplet *self;
    static QList<SwallowApp> *appList;
    static QList<SwallowApp> *embeddedList;
    static KWinModule *wModule;

    QList<SwallowCommand> * swcList;
    QBoxLayout *layout;

};


class SwallowApp : public QXEmbed
{
    Q_OBJECT
	
public:
    SwallowApp( const SwallowCommand * swc, QWidget* parent = 0,
		const char* name = 0);
    ~SwallowApp();

    float sizeRatio() const { return wh_ratio; }

signals:
    void embedded( SwallowApp * );

protected slots:
    void windowAdded(WId win);
    void processExited(KProcess *proc);

private:
    KProcess   	*process;
    QString  	winTitle;
    float 	wh_ratio;

};

#endif // __swallow_h__
