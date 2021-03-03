/*
    KSysGuard, the KDE Task Manager and System Monitor
   
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

	KSysGuard has been written with some source code and ideas from
	ktop (<1.0). Early versions of ktop have been written by Bernd
	Johannes Wuebben <wuebben@math.cornell.edu> and Nicolas Leclercq
	<nicknet@planete.net>.

	$Id: ksysguard.cc,v 1.40 2001/07/20 08:34:00 cschlaeg Exp $
*/

#include <assert.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>

#include <qstringlist.h>

#include <kapp.h>
#include <kwinmodule.h>
#include <kconfig.h>
#include <klocale.h>
#include <kcmdlineargs.h>
#include <kmessagebox.h>
#include <kaboutdata.h>
#include <kstdaccel.h>
#include <kaction.h>
#include <kstdaction.h>
#include <kedittoolbar.h>
#include <kurl.h>
#include <kdebug.h>

#include "SensorBrowser.h"
#include "StyleEngine.h"
#include "SensorManager.h"
#include "SensorAgent.h"
#include "Workspace.h"
#include "../version.h"

#include "ksysguard.moc"

static const char* Description = I18N_NOOP("KDE System Guard");
TopLevel* Toplevel;

/*
 * This is the constructor for the main widget. It sets up the menu and the
 * TaskMan widget.
 */
TopLevel::TopLevel(const char *name)
	: KMainWindow(0, name), DCOPObject("KSysGuardIface")
{
	setPlainCaption(i18n("KDE System Guard"));
	dontSaveSession = FALSE;
	timerId = -1;

	splitter = new QSplitter(this, "Splitter");
	CHECK_PTR(splitter);
	splitter->setOrientation(Horizontal);
	splitter->setOpaqueResize(TRUE);
	setCentralWidget(splitter);

	sb = new SensorBrowser(splitter, SensorMgr, "SensorBrowser");
	CHECK_PTR(sb);

	ws = new Workspace(splitter, "Workspace");
	CHECK_PTR(ws);
	connect(ws, SIGNAL(announceRecentURL(const KURL&)),
			this, SLOT(registerRecentURL(const KURL&)));
	connect(ws, SIGNAL(setCaption(const QString&, bool)),
			this, SLOT(setCaption(const QString&, bool)));
	connect(Style, SIGNAL(applyStyleToWorksheet()), ws, SLOT(applyStyle()));

	/* Create the status bar. It displays some information about the
	 * number of processes and the memory consumption of the local
	 * host. */
	statusbar = new KStatusBar(this, "statusbar");
	CHECK_PTR(statusbar);
	statusbar->insertFixedItem(i18n("88888 Processes"), 0);
	statusbar->insertFixedItem(i18n("Memory: 8888888 kB used, "
							   "8888888 kB free"), 1);
	statusbar->insertFixedItem(i18n("Swap: 8888888 kB used, "
							   "8888888 kB free"), 2);
	statusBar()->hide();

    // create actions for menue entries
    KStdAction::openNew(ws, SLOT(newWorkSheet()), actionCollection());
    KStdAction::open(ws, SLOT(loadWorkSheet()), actionCollection());
	openRecent = KStdAction::openRecent(ws, SLOT(loadWorkSheet(const KURL&)),
										actionCollection());
	KStdAction::close(ws, SLOT(deleteWorkSheet()), actionCollection());

	KStdAction::saveAs(ws, SLOT(saveWorkSheetAs()), actionCollection());
	KStdAction::save(ws, SLOT(saveWorkSheet()), actionCollection());
	KStdAction::quit(this, SLOT(close()), actionCollection());

    (void) new KAction(i18n("C&onnect Host..."), "connect_established", 0,
					   this, SLOT(connectHost()), actionCollection(),
					   "connect_host");
    (void) new KAction(i18n("D&isconnect Host"), "connect_no", 0, this,
					   SLOT(disconnectHost()), actionCollection(),
					   "disconnect_host");
	KStdAction::cut(ws, SLOT(cut()), actionCollection());
	KStdAction::copy(ws, SLOT(copy()), actionCollection());
	KStdAction::paste(ws, SLOT(paste()), actionCollection());
	(void) new KAction(i18n("Configure &Worksheet..."), "configure", 0, ws,
					   SLOT(configure()), actionCollection(),
					   "configure_sheet");
	toolbarTog = KStdAction::showToolbar(this, SLOT(toggleMainToolBar()),
										 actionCollection(), "showtoolbar");
	toolbarTog->setChecked(FALSE);
	statusBarTog = KStdAction::showStatusbar(this, SLOT(showStatusBar()),
											 actionCollection(),
											 "showstatusbar");
    KStdAction::configureToolbars(this, SLOT(editToolbars()),
								  actionCollection());
	statusBarTog->setChecked(FALSE);
	(void) new KAction(i18n("Configure &Style..."), "colorize", 0, this,
					   SLOT(editStyle()), actionCollection(),
					   "configure_style");
	createGUI();

	show();
}

TopLevel::~TopLevel()
{
}

/*
 * DCOP Interface functions
 */
void
TopLevel::showProcesses()
{
	ws->showProcesses();
}

void
TopLevel::loadWorkSheet(const QString& fileName)
{
	ws->loadWorkSheet(KURL(fileName));
}

void
TopLevel::removeWorkSheet(const QString& fileName)
{
	ws->deleteWorkSheet(fileName);
}

QStringList
TopLevel::listSensors(const QString& hostName)
{
	return (sb->listSensors(hostName));
}

QStringList
TopLevel::listHosts()
{
	return (sb->listHosts());
}

QString
TopLevel::readIntegerSensor(const QString& sensorLocator)
{
	QString host = sensorLocator.left(sensorLocator.find(':'));
	QString sensor = sensorLocator.right(sensorLocator.length() -
										 sensorLocator.find(':') - 1);

	DCOPClientTransaction *dcopTransaction =
		kapp->dcopClient()->beginTransaction();
	dcopFIFO.prepend(dcopTransaction);

	SensorMgr->engage(host, "", "ksysguardd2");
	SensorMgr->sendRequest(host, sensor, (SensorClient*) this, 133);

	return (QString::null);
}

QStringList
TopLevel::readListSensor(const QString& sensorLocator)
{
	QStringList retval;
	
	QString host = sensorLocator.left(sensorLocator.find(':'));
	QString sensor = sensorLocator.right(sensorLocator.length() -
										 sensorLocator.find(':') - 1);

	DCOPClientTransaction *dcopTransaction =
		kapp->dcopClient()->beginTransaction();
	dcopFIFO.prepend(dcopTransaction);

	SensorMgr->engage(host, "", "ksysguardd2");
	SensorMgr->sendRequest(host, sensor, (SensorClient*) this, 134);

	return retval;
}

/*
 * End of DCOP Interface section
 */

void
TopLevel::registerRecentURL(const KURL& url)
{
	openRecent->addURL(url);
}

void
TopLevel::beATaskManager()
{
	ws->showProcesses();

	QValueList<int> sizes;
	sizes.append(0);
	sizes.append(100);
	splitter->setSizes(sizes);

	// Show window centered on the desktop.
	KWinModule kwm;
	QRect workArea = kwm.workArea();
	int w = 600;
	if (workArea.width() < w)
		w = workArea.width();
	int h = 440;
	if (workArea.height() < h)
		h = workArea.height();
	setGeometry((workArea.width() - w) / 2, (workArea.height() - h) / 2,
				w, h);

	// No toolbar and status bar in taskmanager mode.
	statusBarTog->setChecked(FALSE);
	showStatusBar();

	showMainToolBar(FALSE);

	dontSaveSession = TRUE;
}

void
TopLevel::showRequestedSheets()
{
	showMainToolBar(FALSE);

	QValueList<int> sizes;
	sizes.append(0);
	sizes.append(100);
	splitter->setSizes(sizes);

	resize(600, 440);
}

void
TopLevel::initStatusBar()
{
	SensorMgr->engage("localhost", "", "ksysguardd2");
	/* Request info about the swap space size and the units it is
	 * measured in.  The requested info will be received by
	 * answerReceived(). */
	SensorMgr->sendRequest("localhost", "mem/swap/used?",
						   (SensorClient*) this, 5);
}

void 
TopLevel::connectHost()
{
	SensorMgr->engageHost("");
}

void 
TopLevel::disconnectHost()
{
	sb->disconnect();
}

void
TopLevel::toggleMainToolBar()
{
	if (toolbarTog->isChecked())
		toolBar("mainToolBar")->show();
	else
		toolBar("mainToolBar")->hide();
}

void
TopLevel::showStatusBar()
{
	if (statusBarTog->isChecked())
	{
		statusBar()->show();
		if (timerId == -1)
		{
			timerId = startTimer(2000);
		}
		// call timerEvent to fill the status bar with real values
		timerEvent(0);
	}
	else
	{
		statusBar()->hide();
		if (timerId != -1)
		{
			killTimer(timerId);
			timerId = -1;
		} 
	}
}

void
TopLevel::editToolbars()
{
	KEditToolbar dlg(actionCollection());

	if (dlg.exec())
		createGUI();
}

void
TopLevel::editStyle()
{
	Style->configure();
}

void
TopLevel::customEvent(QCustomEvent* ev)
{
	if (ev->type() == QEvent::User)
	{
		/* Due to the asynchronous communication between ksysguard and its
		 * back-ends, we sometimes need to show message boxes that were
		 * triggered by objects that have died already. */
		KMessageBox::error(this, *((QString*) ev->data()));
		delete (QString*) ev->data();
	}
}

void
TopLevel::timerEvent(QTimerEvent*)
{
	if (statusbar->isVisibleTo(this))
	{
		/* Request some info about the memory status. The requested
		 * information will be received by answerReceived(). */
		SensorMgr->sendRequest("localhost", "pscount", (SensorClient*) this,
							   0);
		SensorMgr->sendRequest("localhost", "mem/physical/free",
							   (SensorClient*) this, 1);
		SensorMgr->sendRequest("localhost", "mem/physical/used",
							   (SensorClient*) this, 2);
		SensorMgr->sendRequest("localhost", "mem/swap/free",
							   (SensorClient*) this, 3);
	}
}

bool
TopLevel::queryClose()
{
	if (!dontSaveSession)
	{
		if (!ws->saveOnQuit())
			return (FALSE);

		saveProperties(kapp->config());
		kapp->config()->sync();
	}

	return (TRUE);
}

void
TopLevel::readProperties(KConfig* cfg)
{
	int wx = cfg->readNumEntry("PosX", 100);
	int wy = cfg->readNumEntry("PosY", 100);
	int ww = cfg->readNumEntry("SizeX", 600);
	int wh = cfg->readNumEntry("SizeY", 375);
	setGeometry(wx, wy, ww, wh);

	QValueList<int> sizes = cfg->readIntListEntry("SplitterSizeList");
	if (sizes.isEmpty())
	{
		// start with a 30/70 ratio
		sizes.append(30);
		sizes.append(70);
	}
	splitter->setSizes(sizes);

	showMainToolBar(!cfg->readNumEntry("ToolBarHidden", 1));

	if (!cfg->readNumEntry("StatusBarHidden", 1))
	{
		statusBarTog->setChecked(TRUE);
		showStatusBar();
	}

	SensorMgr->readProperties(cfg);
	Style->readProperties(cfg);

	ws->readProperties(cfg);

	setMinimumSize(sizeHint());

	openRecent->loadEntries(cfg);
}

void
TopLevel::saveProperties(KConfig* cfg)
{
	openRecent->saveEntries(cfg);

	// Save window geometry. TODO: x/y is not exaclty correct. Needs fixing.
	cfg->writeEntry("PosX", x());
	cfg->writeEntry("PosY", y());
	cfg->writeEntry("SizeX", width());
	cfg->writeEntry("SizeY", height());
	cfg->writeEntry("SplitterSizeList", splitter->sizes());
	cfg->writeEntry("ToolBarHidden", !toolbarTog->isChecked());
	cfg->writeEntry("StatusBarHidden", !statusBarTog->isChecked());

	Style->saveProperties(cfg);
	SensorMgr->saveProperties(cfg);

	ws->saveProperties(cfg);
}

void
TopLevel::answerReceived(int id, const QString& answer)
{
	QString s;
	static QString unit;
	static long mUsed = 0;
	static long mFree = 0;
	static long sTotal = 0;
	static long sFree = 0;

	switch (id)
	{
	case 0:
		s = i18n("%1 Processes").arg(answer);
		statusbar->changeItem(s, 0);
		break;
	case 1:
		mFree = answer.toLong();
		break;
	case 2:
		mUsed = answer.toLong();
		s = i18n("Memory: %1 %2 used, %3 %4 free")
			.arg(mUsed).arg(unit).arg(mFree).arg(unit);
		statusbar->changeItem(s, 1);
		break;
	case 3:
		sFree = answer.toLong();
		s = i18n("Swap: %1 %2 used, %3 %4 free")
			.arg(sTotal - sFree).arg(unit).arg(sFree).arg(unit);
		statusbar->changeItem(s, 2);
		break;
	case 5:
	{
		SensorIntegerInfo info(answer);
		sTotal = info.getMax();
		unit = SensorMgr->translateUnit(info.getUnit());
		break;
	}
	case 133:
	{
		QCString replyType = "QString";
		QByteArray replyData;
		QDataStream reply(replyData, IO_WriteOnly);
		reply << answer;

		DCOPClientTransaction *dcopTransaction = dcopFIFO.last();
		kapp->dcopClient()->endTransaction(dcopTransaction, replyType,
										   replyData);
		dcopFIFO.removeLast();
		break;
	}

	case 134:
	{
		QStringList resultList;
		QCString replyType = "QStringList";
		QByteArray replyData;
		QDataStream reply(replyData, IO_WriteOnly);

		SensorTokenizer lines(answer, '\n');

		for (unsigned int i = 0; i < lines.numberOfTokens(); i++)
			resultList.append(lines[i]);

		reply << resultList;

		DCOPClientTransaction *dcopTransaction = dcopFIFO.last();
		kapp->dcopClient()->endTransaction(dcopTransaction, replyType,
										   replyData);
		dcopFIFO.removeLast();
		break;
	}
	}
}

void
TopLevel::showMainToolBar(bool show)
{
	toolbarTog->setChecked(show);
	toggleMainToolBar();
}

static const KCmdLineOptions options[] =
{
	{ "showprocesses", I18N_NOOP("Show only process list of local host"), 0 },
	{ "+[worksheet]", I18N_NOOP("Optional worksheet files to load"), 0 },
	{ 0, 0, 0}
};

/*
 * Once upon a time...
 */
int
main(int argc, char** argv)
{
#if 0
//#ifndef NDEBUG
	/* This forking will put ksysguard in it's on session not having a
	 * controlling terminal attached to it. This prevents ssh from
	 * using this terminal for password requests. Unfortunately you
	 * now need a ssh with ssh-askpass support to popup an X dialog to
	 * enter the password. Currently only the original ssh provides this
	 * but not open-ssh. */
	pid_t pid;
	if ((pid = fork()) < 0)
		return (-1);
	else
		if (pid != 0)
		{
			exit(0);
		}
	setsid();
#endif

	KAboutData aboutData("ksysguard2", I18N_NOOP("KDE System Guard"),
						 KSYSGUARD_VERSION, Description,
						 KAboutData::License_GPL,
						 I18N_NOOP("(c) 1996-2001, "
								   "The KSysGuard Developers"));
	aboutData.addAuthor("Chris Schlaeger", "Current Maintainer",
						"cs@kde.org");
	aboutData.addAuthor("Tobias Koenig", 0, "tokoe82@yahoo.de");
	aboutData.addAuthor("Nicolas Leclercq", 0, "nicknet@planete.net");
	aboutData.addAuthor("Alex Sanda", 0, "alex@darkstart.ping.at");
	aboutData.addAuthor("Bernd Johannes Wuebben", 0,
						"wuebben@math.cornell.edu");
	aboutData.addAuthor("Ralf Mueller", 0, "rlaf@bj-ig.de");
	
	KCmdLineArgs::init(argc, argv, &aboutData);
	KCmdLineArgs::addCmdLineOptions(options);
	
	// initialize KDE application
	KApplication *a = new KApplication;

	SensorMgr = new SensorManager();
	CHECK_PTR(SensorMgr);
	Style = new StyleEngine();
	CHECK_PTR(Style);

	KCmdLineArgs* args = KCmdLineArgs::parsedArgs();

	int result = 0;

	if (args->isSet("showprocesses"))
	{
		/* To avoid having multiple instances of ksysguard in
		 * taskmanager mode we check if another taskmanager is running
		 * already. If so, we terminate this one immediately. */
		if (a->dcopClient()->registerAs("ksysguard_taskmanager", FALSE) ==
			"ksysguard_taskmanager")
		{
			Toplevel = new TopLevel("KSysGuard");
			Toplevel->beATaskManager();
			SensorMgr->setBroadcaster(Toplevel);

			// run the application
			result = a->exec();
		}
	}
	else
	{
		a->dcopClient()->registerAs("ksysguard2", FALSE);
		a->dcopClient()->setDefaultObject("KSysGuardIface");

		Toplevel = new TopLevel("KSysGuard");
		CHECK_PTR(Toplevel);

		// create top-level widget
		if (args->count() > 0)
		{
			/* The user has specified a list of worksheets to load. In this
			 * case we do not restore any previous settings but load all the
			 * requested worksheets. */
			Toplevel->showRequestedSheets();
			for (int i = 0; i < args->count(); ++i)
				Toplevel->loadWorkSheet(args->arg(i));
		}
		else
		{
			if (a->isRestored())
				Toplevel->restore(1);
			else
				Toplevel->readProperties(a->config());
		}

		Toplevel->initStatusBar();
		SensorMgr->setBroadcaster(Toplevel);

		// run the application
		result = a->exec();
	}

	delete Style;
	delete SensorMgr;
	delete a;

	return (result);
}
