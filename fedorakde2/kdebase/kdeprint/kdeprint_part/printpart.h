/*
 *  This file is part of the KDE libraries
 *  Copyright (c) 2001 Michael Goffioul <goffioul@imec.be>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License version 2 as published by the Free Software Foundation.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 *  Boston, MA 02111-1307, USA.
 **/

#ifndef PRINTPART_H
#define PRINTPART_H

#include <kparts/part.h>
#include <kparts/browserextension.h>

class PrintPartExtension;
class KMMainView;

class PrintPart : public KParts::ReadOnlyPart
{
	Q_OBJECT
public:
	PrintPart(QWidget *parent = 0, const char *name = 0);
	virtual ~PrintPart();

protected:
	bool openFile();
	void initActions();
	void connectAction(KAction*);

private:
	PrintPartExtension	*m_extension;
	KMMainView		*m_view;
};

class PrintPartExtension : public KParts::BrowserExtension
{
	Q_OBJECT
	friend class PrintPart;
public:
	PrintPartExtension(PrintPart *parent);
	virtual ~PrintPartExtension();
};

#endif
