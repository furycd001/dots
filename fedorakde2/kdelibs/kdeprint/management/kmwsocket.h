/*
 *  This file is part of the KDE libraries
 *  Copyright (c) 2001 Michael Goffioul <goffioul@imec.be>
 *
 *  $Id$
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

#ifndef	KMWSOCKET_H
#define	KMWSOCKET_H

#include "kmwizardpage.h"

class KListView;
class QListViewItem;
class QProgressBar;
class QLineEdit;
class KMWSocketUtil;

class KMWSocket : public KMWizardPage
{
	Q_OBJECT
public:
	KMWSocket(QWidget *parent = 0, const char *name = 0);
	~KMWSocket();

	bool isValid(QString&);
	void updatePrinter(KMPrinter*);

protected slots:
	void slotSettings();
	void slotScan();
	void slotPrinterSelected(QListViewItem*);

private:
	KListView	*m_list;
	QProgressBar	*m_bar;
	QLineEdit	*m_printer, *m_port;
	KMWSocketUtil	*m_util;
};

#endif
