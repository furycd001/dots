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

#include "kjobviewer.h"
#include <kmjobviewer.h>

#include <stdlib.h>
#include <qtimer.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <klocale.h>
#include <ksystemtray.h>
#include <kdebug.h>
#include <kiconloader.h>

KJobViewerApp::KJobViewerApp() : KUniqueApplication()
{
	m_view = 0;
	m_tray = 0;
	m_timer = 0;
}

KJobViewerApp::~KJobViewerApp()
{
}

int KJobViewerApp::newInstance()
{
	initialize();
	return 0;
}

void KJobViewerApp::initialize()
{
	KCmdLineArgs	*args = KCmdLineArgs::parsedArgs();
	bool 	showIt = args->isSet("show");
	bool 	all = args->isSet("all");

	if (!m_view)
	{
		// create main window
		m_view = new KMJobViewer();
		connect(m_view,SIGNAL(jobsShown()),SLOT(slotJobsShown()));
		if (showIt)
			m_view->show();
	}

	if (!m_tray)
	{
		m_tray = new KSystemTray(m_view);
		m_tray->setPixmap(SmallIcon("fileprint"));

		if (showIt)
			m_tray->show();
	}

	QString	prname = args->getOption("d");
	if (!prname.isEmpty())
	{
		// add it to job viewer...
		kdDebug() << "adding printer to filter: " << prname << endl;
		m_view->addPrinter(prname);
	}

	if (all) m_view->selectAll();

	if (!m_timer)
	{
		m_timer = new QTimer(this);
		connect(m_timer,SIGNAL(timeout()),SLOT(slotTimer()));
	}
	slotTimer();
}

void KJobViewerApp::slotJobsShown()
{
	if (!m_tray->isVisible()) m_tray->show();
}

void KJobViewerApp::slotTimer()
{
	if (m_view) m_view->refresh();
	m_timer->start(5000,true);
}
#include "kjobviewer.moc"
