//-----------------------------------------------------------------------------
//
// KDE xscreensaver configuration dialog
//
// Copyright (c)  Martin R. Jones <mjones@kde.org> 1999
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation;
// version 2 of the License.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; see the file COPYING.  If not, write to
// the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
// Boston, MA 02111-1307, USA.

#include <stdlib.h>
#include <qlayout.h>
#include <qtextstream.h>
#include <qtimer.h>

#include <kdebug.h>
#include <kapp.h>
#include <kconfig.h>
#include <kdialogbase.h>
#include <kprocess.h>
#include <kstddirs.h>
#include <klocale.h>
#include <kcmdlineargs.h>

#include "kxsconfig.h"
#include "kxscontrol.h"

template class QList<KXSConfigItem>;

//===========================================================================
KXSConfigDialog::KXSConfigDialog(const QString &filename)
  : KDialogBase(Plain, filename, Ok| Cancel, Ok, 0, 0, false),
    mFilename(filename)
{
  int slash = filename.findRev('/');
  if (slash >= 0)
  {
    mConfigFile = filename.mid(slash+1);
  }
  else
  {
    mConfigFile = filename;
  }

  mConfigFile += "rc";

  KConfig config(mConfigFile);

  QHBoxLayout *layout = new QHBoxLayout(plainPage(), spacingHint());
  QVBoxLayout *controlLayout = new QVBoxLayout(layout, spacingHint());
  controlLayout->addStrut(120);

  int idx = 0;

  while (true)
  {
    QString group = QString("Arg%1").arg(idx);
    if (config.hasGroup(group))
    {
      config.setGroup(group);
      QString type = config.readEntry("Type");
      if (type == "Range")
      {
        KXSRangeControl *rc = new KXSRangeControl(plainPage(), group, config);
        connect(rc, SIGNAL(changed()), SLOT(slotChanged()));
        controlLayout->add(rc);
        mConfigItemList.append(rc);
      }
      else if (type == "DoubleRange")
      {
        KXSDoubleRangeControl *rc =
          new KXSDoubleRangeControl(plainPage(), group, config);
        connect(rc, SIGNAL(changed()), SLOT(slotChanged()));
        controlLayout->add(rc);
        mConfigItemList.append(rc);
      }
      else if (type == "Check")
      {
        KXSCheckBoxControl *cc = new KXSCheckBoxControl(plainPage(), group,
                                                        config);
        connect(cc, SIGNAL(changed()), SLOT(slotChanged()));
        controlLayout->add(cc);
        mConfigItemList.append(cc);
      }
      else if (type == "DropList")
      {
        KXSDropListControl *dl = new KXSDropListControl(plainPage(), group,
                                                        config);
        connect(dl, SIGNAL(changed()), SLOT(slotChanged()));
        controlLayout->add(dl);
        mConfigItemList.append(dl);
      }
    }
    else
    {
      break;
    }
    idx++;
  }

  controlLayout->addStretch(1);

  mPreviewProc = new KProcess;
  connect(mPreviewProc, SIGNAL(processExited(KProcess *)),
                        SLOT(slotPreviewExited(KProcess *)));

  mPreviewTimer = new QTimer(this);
  connect(mPreviewTimer, SIGNAL(timeout()), SLOT(slotNewPreview()));

  mPreview = new QWidget(plainPage());
  mPreview->setFixedSize(250, 200);
//  mPreview->setBackgroundMode(QWidget::NoBackground);
  mPreview->setBackgroundColor(Qt::black);

  layout->add(mPreview);

  slotPreviewExited(0);
}

//---------------------------------------------------------------------------
KXSConfigDialog::~KXSConfigDialog()
{
  if (mPreviewProc->isRunning())
  {
    int pid = mPreviewProc->getPid();
    mPreviewProc->kill();
    waitpid(pid, (int *)0, 0);
    delete mPreviewProc;
  }
}

//---------------------------------------------------------------------------
QString KXSConfigDialog::command()
{
  QString cmd;
  KXSConfigItem *item;

  for (item = mConfigItemList.first(); item != 0; item = mConfigItemList.next())
  {
    cmd += " " + item->command();
  }

  return cmd;
}

//---------------------------------------------------------------------------
void KXSConfigDialog::slotPreviewExited(KProcess *)
{
  mPreviewProc->clearArguments();

  QString saver = mFilename + " -window-id %w";
  saver += command();
  kdDebug() << "Command: " <<  saver << endl;
  QTextStream ts(&saver, IO_ReadOnly);

  QString word;
  ts >> word;
  QString path = KStandardDirs::findExe(word);

  if (!path.isEmpty())
  {
    (*mPreviewProc) << path;

    while (!ts.atEnd())
    {
      ts >> word;
      word = word.stripWhiteSpace();
      if (word == "%w")
      {
        word = word.setNum(mPreview->winId());
      }
      if (!word.isEmpty())
      {
        (*mPreviewProc) << word;
      }
    }

    mPreviewProc->start();
  }
}

//---------------------------------------------------------------------------
void KXSConfigDialog::slotNewPreview()
{
  if (mPreviewProc->isRunning())
  {
    mPreviewProc->kill(); // restarted in slotPreviewExited()
  }
}

//---------------------------------------------------------------------------
void KXSConfigDialog::slotChanged()
{
  if (mPreviewTimer->isActive())
  {
    mPreviewTimer->changeInterval(1000);
  }
  else
  {
    mPreviewTimer->start(1000, true);
  }
}

//---------------------------------------------------------------------------
void KXSConfigDialog::slotOk()
{
  KXSConfigItem *item;
  KConfig config(mConfigFile);

  for (item = mConfigItemList.first(); item != 0; item = mConfigItemList.next())
  {
    item->save(config);
  }

  kdDebug() << command() << endl;
  kapp->quit();
}

//---------------------------------------------------------------------------
void KXSConfigDialog::slotCancel()
{
  kapp->quit();
}

//===========================================================================

static const char *appName = "kxsconfig";

static const char *description = I18N_NOOP("KDE X Screensaver Configuration tool");

static const char *version = "2.0.0";

static const KCmdLineOptions options[] =
{
   {"+screensaver", I18N_NOOP("Filename of the screensaver to configure."), 0},
   {0,0,0}
};

int main(int argc, char *argv[])
{
  KCmdLineArgs::init(argc, argv, appName, description, version);

  KCmdLineArgs::addCmdLineOptions(options);

  KApplication app;

  KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

  KXSConfigDialog *dialog = new KXSConfigDialog(args->arg(0));
  dialog->show();

  app.setMainWidget(dialog);

  app.exec();

  delete dialog;
}

#include "kxsconfig.moc"
