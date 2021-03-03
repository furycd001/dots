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

#ifndef __KXSITEM_H__
#define __KXSITEM_H__

#include <kconfig.h>

class KXSConfigItem
{
public:
  KXSConfigItem(const QString &name, KConfig &config);
  virtual ~KXSConfigItem() {}

  virtual QString command() = 0;
  virtual void save(KConfig &confg) = 0;

protected:
  QString mName;
  QString mLabel;
};

class KXSRangeItem : public KXSConfigItem
{
public:
  KXSRangeItem(const QString &name, KConfig &config);

  virtual QString command();
  virtual void save(KConfig &config);

protected:
  QString mSwitch;
  int mMinimum;
  int mMaximum;
  int mValue;
};

class KXSDoubleRangeItem : public KXSConfigItem
{
public:
  KXSDoubleRangeItem(const QString &name, KConfig &config);

  virtual QString command();
  virtual void save(KConfig &config);

protected:
  QString mSwitch;
  double mMinimum;
  double mMaximum;
  double mValue;
};

class KXSBoolItem : public KXSConfigItem
{
public:
  KXSBoolItem(const QString &name, KConfig &config);

  virtual QString command();
  virtual void save(KConfig &config);

protected:
  QString mSwitchOn;
  QString mSwitchOff;
  bool    mValue;
};

class KXSSelectItem : public KXSConfigItem
{
public:
  KXSSelectItem(const QString &name, KConfig &config);

  virtual QString command();
  virtual void save(KConfig &config);

protected:
  QStringList mOptions;
  QStringList mSwitches;
  int         mValue;
};

#endif

