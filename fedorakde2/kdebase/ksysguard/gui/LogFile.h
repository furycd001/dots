/*
    KSysGuard, the KDE System Guard
   
	Copyright (c) 2001 Tobias Koenig <tokoe82@yahoo.de>
    
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
*/

#ifndef _LogFile_h
#define _LogFile_h

#define MAXLINES 500

#include <qdom.h>
#include <qfile.h>
#include <qlistbox.h>
#include <qpopupmenu.h>
#include <qstring.h>
#include <qstringlist.h>

#include <SensorDisplay.h>
#include "LogFileSettings.h"

class LogFile : public SensorDisplay
{
	Q_OBJECT
public:
	LogFile(QWidget *parent = 0, const char *name = 0, const QString& title = 0);
	~LogFile(void);

	bool addSensor(const QString& hostName, const QString& sensorName,
				   const QString& sensorDescr);
	void answerReceived(int id, const QString& answer);
	void resizeEvent(QResizeEvent*);

	bool createFromDOM(QDomElement& domEl);
	bool addToDOM(QDomDocument& doc, QDomElement& display, bool save = true);

	void updateMonitor(void);

	void settings(void);

	virtual void timerEvent(QTimerEvent*)
	{
		updateMonitor();
	}

	virtual bool hasSettingsDialog() const
	{
		return (TRUE);
	}

	virtual void sensorError(bool err);

public slots:
	void applySettings();
	void applyStyle();

private:
	LogFileSettings* lfs;

	QFile* logFile;
	QLabel* errorLabel;
	QListBox* monitor;
	QString title;
	QStringList filterRules;

	unsigned long logFileID;
};

#endif // _LogFile_h
