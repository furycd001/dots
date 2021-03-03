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

#include "printwrapper.h"

#include <unistd.h>
#include <signal.h>

#include <qstring.h>
#include <qstringlist.h>
#include <stdlib.h>
#include <kmessagebox.h>
#include <qfile.h>
#include <qtimer.h>

#include <kapp.h>
#include <kcmdlineargs.h>
#include <klocale.h>
#include <kstddirs.h>
#include <kio/netaccess.h>
#include <kurl.h>
#include <kdebug.h>

#include <kprinter.h>

void signal_handler(int);
QString tempFile;
bool fromStdin = false;
char job_output = 0;	// 0: dialog, 1: console, 2: none

void showmsgdialog(const QString& msg, int type = 0)
{
	switch (type)
	{
	   case 0: KMessageBox::information(NULL,msg,i18n("Print information")); break;
	   case 1: KMessageBox::sorry(NULL,msg,i18n("Print warning")); break;
	   case 2: KMessageBox::error(NULL,msg,i18n("Print error")); break;
	}
}

void showmsgconsole(const QString& msg, int type = 0)
{
	QString	errmsg = QString::fromLatin1("%1 : ").arg((type == 0 ? i18n("Print info") : (type == 1 ? i18n("Print warning") : i18n("Print error"))));
	kdDebug() << errmsg << msg << endl;
}

void showmsg(const QString& msg, int type = 0)
{
	switch (job_output) {
	   case 0: showmsgdialog(msg,type); break;
	   case 1: showmsgconsole(msg,type); break;
	   default: break;
	}
}

void errormsg(const QString& msg)
{
	showmsg(msg,2);
	exit(1);
}

void signal_handler(int s)
{
	QFile::remove(tempFile);
	exit(s);
}

//******************************************************************************************************

PrintWrapper::PrintWrapper()
: QWidget()
{
}

void PrintWrapper::slotPrint()
{
	KCmdLineArgs	*args = KCmdLineArgs::parsedArgs();

#if defined(HAVE_SIGACTION) && !defined(HAVE_SIGSET)
	struct sigaction action;
#endif /* HAVE_SIGACTION && !HAVE_SIGSET*/

	// read variables from command line
	QString	printer = args->getOption("d");
	QString	title = args->getOption("t");
	QString	job_mode = args->getOption("j");
	QString	system = args->getOption("system");
	QCStringList	optlist = args->getOptionList("o");
	QMap<QString,QString>	opts;
	KURL::List	files;
	QStringList	filestoprint;
	bool	nostdin = !(args->isSet("stdin"));

	// parse options
	for (QCStringList::ConstIterator it=optlist.begin(); it!=optlist.end(); ++it)
	{
		QStringList	l = QStringList::split('=',QString(*it),false);
		if (l.count() >= 1) opts[l[0]] = (l.count() == 2 ? l[1] : QString::null);
	}

	// read file list
	for (int i=0; i<args->count(); i++)
		files.append(args->url(i));

	// some clean-up
	args->clear();

	// set default values if necessary
	if (title.isEmpty()) title = "KPrinter";
	if (job_mode == "console") job_output = 1;
	else if (job_mode == "none") job_output = 2;
	else job_output = 0;

	// check file existence / download file, and show messages if necessary
	for (KURL::List::ConstIterator it=files.begin(); it!=files.end(); ++it)
	{
		QString	target;
		if (!KIO::NetAccess::download(*it,target))
		{
       			QString	msg = i18n("\"%1\": file not found").arg((*it).prettyURL());
       			showmsg(msg,2);
		}
		else
			filestoprint.append(target);
	}

	// do nothing if we have only inexistent files
	if (files.count() > 0 && filestoprint.count() == 0)
		exit(1);

	if (filestoprint.count() == 0 && nostdin)
		errormsg(i18n("Can't print from STDIN (do not use '--nostdin' option)"));

	KPrinter::setApplicationType(KPrinter::StandAlone);

	KPrinter	kprinter;
	kprinter.setSearchName(printer);
	kprinter.setDocName(title);
	kprinter.initOptions(opts);

	if (kprinter.setup(NULL))
	{
		if (filestoprint.count() == 0)
		{
			// print from stdin

#  if defined(HAVE_SIGSET)
			sigset(SIGHUP, signal_handler);
			sigset(SIGINT, signal_handler);
			sigset(SIGTERM, signal_handler);
#  elif defined(HAVE_SIGACTION)
			memset(&action, 0, sizeof(action));
			action.sa_handler = signal_handler;

			sigaction(SIGHUP, &action, NULL);
			sigaction(SIGINT, &action, NULL);
			sigaction(SIGTERM, &action, NULL);
#  else
			signal(SIGHUP, signal_handler);
			signal(SIGINT, signal_handler);
			signal(SIGTERM, signal_handler);
#  endif

			tempFile = locateLocal("tmp","kprinter_")+QString::number(getpid());
			filestoprint.append(tempFile);
			fromStdin = true;
			FILE	*fout = fopen(QFile::encodeName(filestoprint[0]),"w");
			if (!fout) errormsg(i18n("Unable to open temporary file."));
			char	buffer[8192];
			int	s;

			// read stdin and write to temporary file
			while ((s=fread(buffer,1,sizeof(buffer),stdin)) > 0)
				fwrite(buffer,1,s,fout);

			s = ftell(fout);
			fclose(fout);
			if (s <= 0) errormsg(i18n("Stdin is empty, no job sent."));
		}

		// print all files
		bool ok = kprinter.printFiles(filestoprint);

		if (!ok) errormsg(i18n("Error while printing files"));
		else
		{
			QString	msg = i18n("<nobr>File(s) sent to printer <b>%1</b>.</nobr>").arg(kprinter.printerName());
			showmsg(msg,0);
		}

		// if printing from stdin, remove temporary file
		if (fromStdin) QFile::remove(filestoprint[0]);
	}

	QTimer::singleShot(10,kapp,SLOT(quit()));
}

#include "printwrapper.moc"
