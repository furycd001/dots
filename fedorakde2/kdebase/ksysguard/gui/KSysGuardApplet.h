/*
    KKSysGuard, the KDE System Guard
   
	Copyright (c) 1999 - 2001 Chris Schlaeger <cs@kde.org>
    
    This program is free software; you can redistribute it and/or
    modify it under the terms of version 2 of the GNU General Public
    License as published by the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

	KSysGuard is currently maintained by Chris Schlaeger
	<cs@kde.org>. Please do not commit any changes without consulting
	me first. Thanks!

	$Id: KSysGuardApplet.h,v 1.8 2001/07/01 15:10:09 tokoe Exp $
*/

#ifndef __KSysGuardApplet_h__
#define __KSysGuardApplet_h__

#include <kpanelapplet.h>

class SensorDisplay;
class QDragEnterEvent;
class QDropEvent;
class QPoint;
class KSysGuardAppletSettings;

class KSysGuardApplet : public KPanelApplet
{
    Q_OBJECT

public:
    KSysGuardApplet(const QString& configFile, Type t = Normal,
		     int actions = 0, QWidget *parent = 0, const char *name = 0);

    virtual ~KSysGuardApplet();

	virtual int heightForWidth(int w) const;
	virtual int widthForHeight(int h) const;
	virtual void preferences();

protected:
    void resizeEvent(QResizeEvent*);
	void dragEnterEvent(QDragEnterEvent* ev);
	void dropEvent(QDropEvent* ev);
	void customEvent(QCustomEvent* ev);

private slots:
	void applySettings();

private:
	void layout();
	void resizeDocks(uint newDockCnt);

	bool load();
	bool save();

	int findDock(const QPoint& p);
	void removeDisplay(SensorDisplay* sd);

	uint dockCnt;
	QWidget** docks;
	uint updateInterval;
	double sizeRatio;
	KSysGuardAppletSettings* ksgas;
};

#endif
