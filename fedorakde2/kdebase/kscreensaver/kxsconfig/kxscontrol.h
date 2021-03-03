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

#ifndef __KXSCONTROL_H__
#define __KXSCONTROL_H__

#include <qwidget.h>
#include <qcheckbox.h>

#include "kxsitem.h"

class QLabel;
class QSlider;
class QComboBox;

//===========================================================================
class KXSRangeControl : public QWidget, public KXSRangeItem
{
  Q_OBJECT
public:
  KXSRangeControl(QWidget *parent, const QString &name, KConfig &config);

signals:
  void changed();

protected slots:
  void slotValueChanged(int value);

protected:
  QSlider *mSlider;
};

//===========================================================================
class KXSDoubleRangeControl : public QWidget, public KXSDoubleRangeItem
{
  Q_OBJECT
public:
  KXSDoubleRangeControl(QWidget *parent, const QString &name, KConfig &config);

signals:
  void changed();

protected slots:
  void slotValueChanged(int value);

protected:
  QSlider *mSlider;
  double  mStep;
};

//===========================================================================
class KXSCheckBoxControl : public QCheckBox, public KXSBoolItem
{
  Q_OBJECT
public:
  KXSCheckBoxControl(QWidget *parent, const QString &name, KConfig &config);

signals:
  void changed();

protected slots:
  void slotToggled(bool);
};

//===========================================================================
class KXSDropListControl : public QWidget, public KXSSelectItem
{
  Q_OBJECT
public:
  KXSDropListControl(QWidget *parent, const QString &name, KConfig &config);

signals:
  void changed();

protected slots:
  void slotActivated(int);

protected:
  QComboBox *mCombo;
};

#endif

