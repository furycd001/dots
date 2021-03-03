/*
    KSysGuard, the KDE Task Manager
   
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

	$Id: SensorShellAgent.cc,v 1.5 2001/05/25 14:46:14 cschlaeg Exp $
*/

#include <stdlib.h>
#include <iostream>

#include <qevent.h>
#include <qapplication.h>
#include <qstring.h>

#include <klocale.h>
#include <kprocess.h>
#include <kpassdlg.h> 
#include <kmessagebox.h>
#include <kdebug.h>

#include "SensorManager.h"
#include "SensorClient.h"
#include "SensorShellAgent.moc"

SensorShellAgent::SensorShellAgent(SensorManager* sm) :
	SensorAgent(sm)
{
}

SensorShellAgent::~SensorShellAgent()
{
	if (daemon)
	{
		daemon->writeStdin("quit\n", strlen("quit\n"));
		delete daemon;
		daemon = 0;
	}
}
	
bool
SensorShellAgent::start(const QString& host_, const QString& shell_,
						const QString& command_, int)
{
	daemon = new KShellProcess;
	CHECK_PTR(daemon);

	host = host_;
	shell = shell_;
	command = command_;

	connect(daemon, SIGNAL(processExited(KProcess *)),
			this, SLOT(daemonExited(KProcess*)));
	connect(daemon, SIGNAL(receivedStdout(KProcess *, char*, int)),
			this, SLOT(msgRcvd(KProcess*, char*, int)));
	connect(daemon, SIGNAL(receivedStderr(KProcess *, char*, int)),
			this, SLOT(errMsgRcvd(KProcess*, char*, int)));
	connect(daemon, SIGNAL(wroteStdin(KProcess*)), this,
			SLOT(msgSent(KProcess*)));

	QString cmd;
	if (command != "")
		cmd =  command;
	else
		cmd = shell + " " + host + " ksysguardd2";
	*daemon << cmd;

	if (!daemon->start(KProcess::NotifyOnExit, KProcess::All))
	{
		sensorManager->hostLost(this);
		kdDebug () << "Command '" << cmd << "' failed"  << endl;
		return (false);
	}

	return (true);
}

void
SensorShellAgent::msgSent(KProcess*)
{
	transmitting = false;
	// Try to send next request if available.
	executeCommand();
}

void 
SensorShellAgent::msgRcvd(KProcess*, char* buffer, int buflen)
{
	if (!buffer || buflen == 0)
		return;

	QString aux = QString::fromLocal8Bit(buffer, buflen);

	processAnswer(aux);
}

void
SensorShellAgent::errMsgRcvd(KProcess*, char* buffer, int buflen)
{
	if (!buffer || buflen == 0)
		return;

	QString buf = QString::fromLocal8Bit(buffer, buflen);

	kdDebug() << "SensorShellAgent: Warning, received text over stderr!"
			  << endl << buf << endl;
}

void
SensorShellAgent::daemonExited(KProcess*)
{
	daemonOnLine = false;
	sensorManager->hostLost(this);
	sensorManager->disengage(this);
}

bool
SensorShellAgent::writeMsg(const char* msg, int len)
{
	return (daemon->writeStdin(msg, len));
}
