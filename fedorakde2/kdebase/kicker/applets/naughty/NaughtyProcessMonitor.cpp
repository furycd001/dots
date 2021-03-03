/*
    Naughty applet - Runaway process monitor for the KDE panel

    Copyright 2000 Rik Hemsley (rikkus) <rik@kde.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include <sys/types.h>
#include <signal.h>
#include <unistd.h>

#include <qfile.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qtextstream.h>
#include <qdir.h>
#include <qtimer.h>
#include <qmap.h>
#include <qdatetime.h>

#include <klocale.h>

#include "NaughtyProcessMonitor.h"

class NaughtyProcessMonitorPrivate
{
  public:

    NaughtyProcessMonitorPrivate()
      : interval_(0),
        timer_(0),
        oldLoad_(0),
        triggerLevel_(0)
    {
    }

    ~NaughtyProcessMonitorPrivate()
    {
      // Empty.
    }

    uint interval_;
    QTimer * timer_;
    QMap<ulong, uint> loadMap_;
    QMap<ulong, uint> scoreMap_;
    uint oldLoad_;
    uint triggerLevel_;

  private:

    NaughtyProcessMonitorPrivate(const NaughtyProcessMonitorPrivate &);

    NaughtyProcessMonitorPrivate & operator =
      (const NaughtyProcessMonitorPrivate &);
};

NaughtyProcessMonitor::NaughtyProcessMonitor
  (
   uint interval,
   uint triggerLevel,
   QObject * parent,
   const char * name
  )
  : QObject(parent, name)
{
  d = new NaughtyProcessMonitorPrivate;
  d->interval_ = interval * 1000;
  d->triggerLevel_ = triggerLevel;
  d->timer_ = new QTimer(this);
  connect(d->timer_, SIGNAL(timeout()), this, SLOT(slotTimeout()));
}

NaughtyProcessMonitor::~NaughtyProcessMonitor()
{
  delete d;
}

  void
NaughtyProcessMonitor::start()
{
  d->timer_->start(d->interval_, true);
}

  void
NaughtyProcessMonitor::stop()
{
  d->timer_->stop();
}

  uint
NaughtyProcessMonitor::interval() const
{
  return d->interval_ / 1000;
}

  void
NaughtyProcessMonitor::setInterval(uint i)
{
  stop();
  d->interval_ = i * 1000;
  start();
}

  uint
NaughtyProcessMonitor::triggerLevel() const
{
  return d->triggerLevel_;
}

  void
NaughtyProcessMonitor::setTriggerLevel(uint i)
{
  d->triggerLevel_ = i;
}

  void
NaughtyProcessMonitor::slotTimeout()
{
  uint cpu = cpuLoad();

  emit(load(cpu));

  if (cpu > d->triggerLevel_ * (d->interval_ / 1000))
  {
    uint load;
    QValueList<ulong> l(pidList());

    for (QValueList<ulong>::ConstIterator it(l.begin()); it != l.end(); ++it)
      if (getLoad(*it, load))
        _process(*it, load);
  }

  d->timer_->start(d->interval_, true);
}

  void
NaughtyProcessMonitor::_process(ulong pid, uint load)
{
  if (!d->loadMap_.contains(pid))
  {
    d->loadMap_.insert(pid, load);
    return;
  }

  uint oldLoad = d->loadMap_[pid];
  bool misbehaving = (load - oldLoad) > 40 * (d->interval_ / 1000);
  bool wasMisbehaving = d->scoreMap_.contains(pid);

  if (misbehaving)
    if (wasMisbehaving)
    {
      d->scoreMap_.replace(pid, d->scoreMap_[pid] + 1);
      if (canKill(pid))
        emit(runawayProcess(pid, processName(pid)));
    }
    else
      d->scoreMap_.insert(pid, 1);
  else
    if (wasMisbehaving)
      d->scoreMap_.remove(pid);

  d->loadMap_.replace(pid, load);
}

// Here begins the set of system-specific methods.

  bool
NaughtyProcessMonitor::canKill(ulong pid) const
{
#ifdef __linux__
  QFile f("/proc/" + QString::number(pid) + "/status");

  if (!f.open(IO_ReadOnly))
    return false;

  QTextStream t(&f);

  QString s;

  while (!t.atEnd() && s.left(4) != "Uid:")
    s = t.readLine();

  QStringList l(QStringList::split('\t', s));

  uint a(l[1].toUInt());

// What are these 3 fields for ? Would be nice if the Linux kernel docs
// were complete, eh ?
//  uint b(l[2].toUInt()); 
//  uint c(l[3].toUInt());
//  uint d(l[4].toUInt());

  return geteuid() == a;
#else
  return false;
#endif
}

  QString
NaughtyProcessMonitor::processName(ulong pid) const
{
#ifdef __linux__
  QFile f("/proc/" + QString::number(pid) + "/cmdline");

  if (!f.open(IO_ReadOnly))
    return i18n("Unknown");

  QCString s;

  while (true)
  {
    int c = f.getch();

    // Stop at NUL
    if (c == -1 || char(c) == '\0')
      break;
    else
      s += char(c);
  }

 // Now strip 'kdeinit:' prefix.
  QString unicode(QString::fromLocal8Bit(s));

  QStringList parts(QStringList::split(' ', unicode));

  QString processName = parts[0] == "kdeinit:" ? parts[1] : parts[0];

  int lastSlash = processName.findRev('/');

  // Get basename, if there's a path.
  if (-1 != lastSlash)
    processName = processName.mid(lastSlash + 1);

  return processName;

#else
  return QString::null;
#endif
}

  uint
NaughtyProcessMonitor::cpuLoad() const
{
#ifdef __linux__
  QFile f("/proc/stat");

  if (!f.open(IO_ReadOnly))
    return 0;

  bool forgetThisOne = 0 == d->oldLoad_;

  QTextStream t(&f);

  QString s = t.readLine();

  QStringList l(QStringList::split(' ', s));

  uint user  = l[1].toUInt();
  uint sys   = l[3].toUInt();

  uint load = user + sys;
  uint diff = load - d->oldLoad_;
  d->oldLoad_ = load;

  return (forgetThisOne ? 0 : diff);
#else
  return 0;
#endif
}

  QValueList<ulong>
NaughtyProcessMonitor::pidList() const
{
#ifdef __linux__
  QStringList dl(QDir("/proc").entryList());

  QValueList<ulong> pl;

  for (QStringList::ConstIterator it(dl.begin()); it != dl.end(); ++it)
    if (((*it)[0].isDigit()))
      pl << (*it).toUInt();

  return pl;
#else
  QValueList<ulong> l;
  return l;
#endif
}

  bool
NaughtyProcessMonitor::getLoad(ulong pid, uint & load) const
{
#ifdef __linux__
  QFile f("/proc/" + QString::number(pid) + "/stat");

  if (!f.open(IO_ReadOnly))
    return false;

  QTextStream t(&f);

  QString line(t.readLine());

  QStringList fields(QStringList::split(' ', line));

  uint userTime (fields[13].toUInt());
  uint sysTime  (fields[14].toUInt());

  load = userTime + sysTime;

  return true;
#else
  return false;
#endif
}

  bool
NaughtyProcessMonitor::kill(ulong pid) const
{
#ifdef __linux__
  return 0 == ::kill(pid, SIGKILL);
#else
  return false;
#endif
}

#include "NaughtyProcessMonitor.moc"
