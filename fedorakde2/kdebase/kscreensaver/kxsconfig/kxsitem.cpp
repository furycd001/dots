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

#include "kxsconfig.h"
#include <klocale.h>

//===========================================================================
KXSConfigItem::KXSConfigItem(const QString &name, KConfig &config)
  : mName(name)
{
  config.setGroup(name);
  mLabel = i18n(config.readEntry("Label").utf8());
}

//===========================================================================
KXSRangeItem::KXSRangeItem(const QString &name, KConfig &config)
  : KXSConfigItem(name, config)
{
  mMinimum = config.readNumEntry("Minimum");
  mMaximum = config.readNumEntry("Maximum");
  mValue   = config.readNumEntry("Value");
  mSwitch  = config.readEntry("Switch");
}

QString KXSRangeItem::command()
{
  return mSwitch.arg(mValue);
}

void KXSRangeItem::save(KConfig &config)
{
  config.setGroup(mName);
  config.writeEntry("Value", mValue);
}

//===========================================================================
KXSDoubleRangeItem::KXSDoubleRangeItem(const QString &name, KConfig &config)
  : KXSConfigItem(name, config)
{
  mMinimum = config.readDoubleNumEntry("Minimum");
  mMaximum = config.readDoubleNumEntry("Maximum");
  mValue   = config.readDoubleNumEntry("Value");
  mSwitch  = config.readEntry("Switch");
}

QString KXSDoubleRangeItem::command()
{
  return mSwitch.arg(mValue);
}

void KXSDoubleRangeItem::save(KConfig &config)
{
  config.setGroup(mName);
  config.writeEntry("Value", mValue);
}


//===========================================================================
KXSBoolItem::KXSBoolItem(const QString &name, KConfig &config)
  : KXSConfigItem(name, config)
{
  mValue = config.readBoolEntry("Value");
  mSwitchOn  = config.readEntry("SwitchOn");
  mSwitchOff = config.readEntry("SwitchOff");
}

QString KXSBoolItem::command()
{
  return mValue ? mSwitchOn : mSwitchOff;
}

void KXSBoolItem::save(KConfig &config)
{
  config.setGroup(mName);
  config.writeEntry("Value", mValue);
}

//===========================================================================
KXSSelectItem::KXSSelectItem(const QString &name, KConfig &config)
  : KXSConfigItem(name, config)
{
  mOptions = config.readListEntry("Options");
  mSwitches = config.readListEntry("Switches");
  mValue = config.readNumEntry("Value");
}

QString KXSSelectItem::command()
{
  QStringList::Iterator it = mSwitches.at(mValue);
  return (*it);
}

void KXSSelectItem::save(KConfig &config)
{
  config.setGroup(mName);
  config.writeEntry("Value", mValue);
}


