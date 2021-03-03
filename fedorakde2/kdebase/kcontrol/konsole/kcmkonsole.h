/***************************************************************************
                          kcmkonsole.h 
                             -------------------
    begin                : mar apr 17 16:44:59 CEST 2001
    copyright            : (C) 2001 by Andrea Rizzi
    email                : rizzi@kde.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KCMKONSOLE_H
#define KCMKONSOLE_H

#include <kcmodule.h>
#include <kaboutdata.h>
#include "kcmkonsoledialog.h"
class QFont;

class KCMKonsole
	: public KCModule
{
	Q_OBJECT

public:
	KCMKonsole (QWidget *parent = 0, const char *name = 0);
	
	void load();
	void load(const QString &);
	void save();
	void defaults();
	QString quickHelp() const;
	virtual const KAboutData * aboutData() const;
public slots:
	void setupFont();
	void configChanged();
private:
	KCMKonsoleDialog *dialog;
	QFont currentFont;
};

#endif
