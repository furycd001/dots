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

#include <qlabel.h>
#include <qslider.h>
#include <qlayout.h>
#include <qcombobox.h>
#include "kxscontrol.h"

//===========================================================================
KXSRangeControl::KXSRangeControl(QWidget *parent, const QString &name,
                                  KConfig &config)
  : QWidget(parent), KXSRangeItem(name, config)
{
  QVBoxLayout *l = new QVBoxLayout(this);
  QLabel *label = new QLabel(mLabel, this);
  l->add(label);
  mSlider = new QSlider(mMinimum, mMaximum, 10, mValue, Qt::Horizontal, this);
  connect(mSlider, SIGNAL(valueChanged(int)), SLOT(slotValueChanged(int)));
  l->add(mSlider);
}

void KXSRangeControl::slotValueChanged(int value)
{
  mValue = value;
  emit changed();
}

//===========================================================================
KXSDoubleRangeControl::KXSDoubleRangeControl(QWidget *parent,
                                  const QString &name, KConfig &config)
  : QWidget(parent), KXSDoubleRangeItem(name, config)
{
  QVBoxLayout *l = new QVBoxLayout(this);
  QLabel *label = new QLabel(mLabel, this);
  l->add(label);

  int value = int((mValue - mMinimum) * 100 / (mMaximum - mMinimum));

  mSlider = new QSlider(0, 100, 10, value, Qt::Horizontal, this);
  connect(mSlider, SIGNAL(valueChanged(int)), SLOT(slotValueChanged(int)));
  l->add(mSlider);
}

void KXSDoubleRangeControl::slotValueChanged(int value)
{
  mValue = mMinimum + value * (mMaximum - mMinimum) / 100.0;
  emit changed();
}

//===========================================================================
KXSCheckBoxControl::KXSCheckBoxControl(QWidget *parent, const QString &name,
                                      KConfig &config)
  : QCheckBox(parent), KXSBoolItem(name, config)
{
  setText(mLabel);
  setChecked(mValue);
  connect(this, SIGNAL(toggled(bool)), SLOT(slotToggled(bool)));
}

void KXSCheckBoxControl::slotToggled(bool state)
{
  mValue = state;
  emit changed();
}

//===========================================================================
KXSDropListControl::KXSDropListControl(QWidget *parent, const QString &name,
                                      KConfig &config)
  : QWidget(parent), KXSSelectItem(name, config)
{
  QVBoxLayout *l = new QVBoxLayout(this);
  QLabel *label = new QLabel(mLabel, this);
  l->add(label);
  mCombo = new QComboBox(this);
  mCombo->insertStringList(mOptions);
  mCombo->setCurrentItem(mValue);
  connect(mCombo, SIGNAL(activated(int)), SLOT(slotActivated(int)));
  l->add(mCombo);
}

void KXSDropListControl::slotActivated(int indx)
{
  mValue = indx;
  emit changed();
}

#include "kxscontrol.moc"
