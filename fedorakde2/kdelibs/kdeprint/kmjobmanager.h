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

#ifndef KMJOBMANAGER_H
#define KMJOBMANAGER_H

#include <qobject.h>
#include <qlist.h>
#include <qstringlist.h>

class KMJob;
class KMThreadJob;

class KMJobManager : public QObject
{
public:
	KMJobManager(QObject *parent = 0, const char *name = 0);
	virtual ~KMJobManager();

	void addPrinter(const QString& pr);
	void removePrinter(const QString& pr);
	const QStringList& filter() const;
	void clearFilter();

	KMJob* findJob(int ID);
	bool sendCommand(int ID, int action, const QString& arg = QString::null);
	bool sendCommand(const QList<KMJob>& jobs, int action, const QString& arg = QString::null);
	const QList<KMJob>& jobList();
	void addJob(KMJob*);
	KMThreadJob* threadJob();

	virtual int actions();

protected:
	void discardAllJobs();
	void removeDiscardedJobs();

protected:
	virtual bool listJobs();
	virtual bool sendCommandSystemJob(const QList<KMJob>& jobs, int action, const QString& arg = QString::null);
	bool sendCommandThreadJob(const QList<KMJob>& jobs, int action, const QString& arg = QString::null);

protected:
	QList<KMJob>	m_jobs;
	QStringList	m_printers;
	KMThreadJob	*m_threadjob;
};

inline void KMJobManager::addPrinter(const QString& pr)
{ if (m_printers.contains(pr) == 0) m_printers.append(pr); }

inline void KMJobManager::removePrinter(const QString& pr)
{ m_printers.remove(pr); }

inline const QStringList& KMJobManager::filter() const
{ return m_printers; }

inline void KMJobManager::clearFilter()
{ m_printers.clear(); }

inline KMThreadJob* KMJobManager::threadJob()
{ return m_threadjob; }

#endif
